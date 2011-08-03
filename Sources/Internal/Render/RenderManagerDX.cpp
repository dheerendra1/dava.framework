/*==================================================================================
    Copyright (c) 2008, DAVA Consulting, LLC
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the DAVA Consulting, LLC nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE DAVA CONSULTING, LLC AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL DAVA CONSULTING, LLC BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    Revision History:
        * Created by Vitaliy Borodovsky 
=====================================================================================*/
#include "Render/RenderManager.h"
#include "Render/Texture.h"
#include "Render/2D/Sprite.h"
#include "Utils/Utils.h"
#include "Render/RenderBase.h"
#include "Utils/StringFormat.h"
#include "Core/Core.h"
#include <limits>

#ifdef __DAVAENGINE_DIRECTX9__
#include "Render/D3D9Helpers.h"

#include "Render/DynamicVertexBuffer.h"
#include "Render/DynamicIndexBuffer.h"

namespace DAVA
{

static IDirect3D9 * direct3D = 0;
static IDirect3DDevice9 * direct3DDevice = 0;
static Matrix4 identity;
static Matrix4 projection;
static DynamicVertexBuffer * vertexBuffer = 0;
static DynamicIndexBuffer * indexBuffer = 0;
static IDirect3DQuery9* renderQuery = 0;

struct RenderManagerVertex2D
{
	float32 x, y, z;
	uint32 color;
	float32 u, v;
};

LPDIRECT3D9	RenderManager::GetD3D()
{
	return direct3D;
}

LPDIRECT3DDEVICE9 RenderManager::GetD3DDevice()
{
	return direct3DDevice;
}
void SwitchDisplayMode(DisplayMode mode, bool isFullscreen);

bool RenderManager::Create(HINSTANCE _hInstance, HWND _hWnd)
{
	hInstance = _hInstance;
	hWnd = _hWnd;

	if( NULL == (direct3D = Direct3DCreate9(D3D_SDK_VERSION)))
	{
		return false;
	};

	// print device information
	D3DADAPTER_IDENTIFIER9 dai;
	if (!FAILED(direct3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &dai)))
	{
		int32 product = HIWORD(dai.DriverVersion.HighPart);
		int32 version = LOWORD(dai.DriverVersion.HighPart);
		int32 subVersion = HIWORD(dai.DriverVersion.LowPart);
		int32 build = LOWORD(dai.DriverVersion.LowPart);

		Logger::Debug("[RenderManager::Create] device info: %s %s %d.%d.%d.%d", dai.Description, dai.Driver, product, version, subVersion, build);
	} else
	{
		Logger::Error("[RenderManager::Create] failed to create direct 3d object");
		return false;
	}

	depthStencilSurface = 0;
	backBufferSurface = 0;

	return true;
}

// D3DPRESENT_PARAMETERS RenderManager::BuildPresentParameters( DisplayMode mode, bool isFullscreen )
// {
// 	D3DPRESENT_PARAMETERS   d3dpp;
// 
// 	ZeroMemory ( &d3dpp, sizeof(D3DPRESENT_PARAMETERS) );
// 
// 	// Fill out our common present parameters
// 	d3dpp.BackBufferCount           = 1;
// 	d3dpp.BackBufferFormat          = pSettings->BackBufferFormat;
// 	d3dpp.Windowed                  = !isFullscreen;
// 	d3dpp.MultiSampleType           = pSettings->MultisampleType;
// 	d3dpp.MultiSampleQuality        = pSettings->MultisampleQuality;
// 	d3dpp.EnableAutoDepthStencil    = TRUE;
// 	d3dpp.AutoDepthStencilFormat    = pSettings->DepthStencilFormat;
// 	d3dpp.PresentationInterval      = pSettings->PresentInterval;
// 	d3dpp.Flags                     = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL | Flags;
// 	d3dpp.SwapEffect                = D3DSWAPEFFECT_DISCARD;
// 
// 	// Is this fullscreen ?
// 	if ( !d3dpp.Windowed )
// 	{
// 		d3dpp.FullScreen_RefreshRateInHz = pSettings->DisplayMode.RefreshRate;
// 		d3dpp.BackBufferWidth            = pSettings->DisplayMode.Width;
// 		d3dpp.BackBufferHeight           = pSettings->DisplayMode.Height;
// 
// 	} // End if fullscreen
// 
// 	// Success
// 	return d3dpp;
// }

bool RenderManager::ChangeDisplayMode(DisplayMode mode, bool isFullscreen)
{
	RenderResource::SaveAllResourcesToSystemMem();

	D3DDISPLAYMODE d3ddm;
	HRESULT hr = direct3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
	if (FAILED(hr))
	{
		Logger::Debug("Error: Could not get adapter display mode.");
		return false;
	}

	d3ddm.Width = mode.width;
	d3ddm.Height = mode.height;
	d3ddm.RefreshRate = mode.refreshRate;
	d3ddm.Format = (mode.bpp == 16) ? (D3DFMT_R5G6B5) : (D3DFMT_X8R8G8B8);
	int32 bitsPerPixel = mode.bpp;
	bool isStencilBuffer = true;
	bool isVSyncEnabled = true;
		

	ZeroMemory(&presentParams, sizeof(presentParams));

	
	presentParams.SwapEffect = isFullscreen ? D3DSWAPEFFECT_FLIP : D3DSWAPEFFECT_COPY;
	presentParams.Windowed = isFullscreen ? FALSE : TRUE;
	presentParams.BackBufferFormat = d3ddm.Format;
	presentParams.EnableAutoDepthStencil = TRUE;
	presentParams.hDeviceWindow = hWnd;
	//Present.PresentationInterval		= VSync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;

	if (isFullscreen)
	{
		presentParams.BackBufferWidth = mode.width;
		presentParams.BackBufferHeight = mode.height;
		presentParams.BackBufferFormat = d3ddm.Format;
		presentParams.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		presentParams.PresentationInterval = isVSyncEnabled ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;

		// request 32bit mode if user specified 32 bit, added by Thomas Stüfe
		//if ( bitsPerPixel == 32 ) 
		//	presentParams.BackBufferFormat = D3DFMT_X8R8G8B8;
		
		// FOR TEST
		presentParams.BackBufferCount = 2;
	}

	// check stencil buffer compatibility
	if (isStencilBuffer)
	{
		presentParams.AutoDepthStencilFormat = D3DFMT_D24S8;
		if(FAILED(direct3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
			presentParams.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL, 
			D3DRTYPE_SURFACE, D3DFMT_D24S8)))
		{
			Logger::Warning("[RenderManagerDX] device does not support stencilbuffer, disabling stencil buffer.");
			isStencilBuffer = false;
		}
		else
			if(FAILED(direct3D->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
				presentParams.BackBufferFormat, presentParams.BackBufferFormat, D3DFMT_D24S8)))
			{
				Logger::Warning("[RenderManagerDX] Depth-stencil format is not compatible with display format, disabling stencil buffer.");
				isStencilBuffer  = false;
			} 		
	}

	if (!isStencilBuffer)
		presentParams.AutoDepthStencilFormat = D3DFMT_D16;


	// create device


	//hr = pID3D->CreateDevice(	D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,	hWnd,
	//							D3DCREATE_HARDWARE_VERTEXPROCESSING, &Present, &pID3DDevice);
	//
	//if(FAILED(hr))
	//{
	//	hr = pID3D->CreateDevice(	D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,	hWnd,
	//								D3DCREATE_MIXED_VERTEXPROCESSING , &Present, &pID3DDevice);

	//	if(FAILED(hr))
	//	{
	//		hr = pID3D->CreateDevice(	D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
	//									D3DCREATE_SOFTWARE_VERTEXPROCESSING, &Present, &pID3DDevice);
	//	}
	//}

	// NVPerfHUD mode

	UINT adapterToUse = D3DADAPTER_DEFAULT;
	D3DDEVTYPE deviceType = D3DDEVTYPE_HAL;

	// Look for 'NVIDIA NVPerfHUD' adapter
	// If it is present, override default settings
	for (UINT adapter = 0; adapter < direct3D->GetAdapterCount(); adapter++)
	{
		D3DADAPTER_IDENTIFIER9 identifier;
		HRESULT res;
		res = direct3D->GetAdapterIdentifier(adapter,0,&identifier);

		Logger::Debug(Format("[RenderManagerDX9] %s", identifier.Description));
		if (strstr(identifier.Description,"PerfHUD") != 0)
		{
			adapterToUse=adapter;
			deviceType=D3DDEVTYPE_REF;
			Logger::Debug("[RenderManagerDX9] NV PerfHUD device found");
			break;
		}
	}

	if (!direct3DDevice)
	{
		hr = direct3D->CreateDevice(adapterToUse, deviceType,	hWnd,
			D3DCREATE_HARDWARE_VERTEXPROCESSING /*| D3DCREATE_MULTITHREADED*/ , &presentParams, &direct3DDevice);

		if (!direct3DDevice)
		{
			hr = direct3D->CreateDevice(adapterToUse, deviceType,	hWnd,
										D3DCREATE_SOFTWARE_VERTEXPROCESSING /*| D3DCREATE_MULTITHREADED*/ , &presentParams, &direct3DDevice);
			if (!direct3DDevice)
			{
				Logger::Error("[RenderManagerDX9] Was not able to create DirectX9 device.\n");
				return false;
			}
		} 
		
		RENDER_VERIFY(direct3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBufferSurface));
		RENDER_VERIFY(direct3DDevice->GetDepthStencilSurface(&depthStencilSurface));
	}else 
	{
		OnDeviceLost();

		hr = direct3DDevice->Reset(&presentParams);
		if (FAILED(hr))
		{
			Logger::Error("[RenderManagerDX9] Was not able to reset DirectX9 device to new params.\n");
			return false;			
		}

		OnDeviceRestore();
	}

	// Write common device information (need for debug)
	direct3DDevice->GetDeviceCaps(&deviceCaps);

	D3DCAPS9 & caps = deviceCaps; 

	//Logger::Info(Format("[RenderSystemDX9] streamCount:%d maxStreamStride:%d", caps.MaxStreams, caps.MaxStreamStride));
	//Logger::Info(Format("[RenderSystemDX9] lightCount:%d", caps.MaxActiveLights));
	//Logger::Info(Format("[RenderSystemDX9] vs:%d.%d ps:%d.%d", 
	//	(caps.VertexShaderVersion >> 8) & 0xF,
	//	(caps.VertexShaderVersion) & 0xF, 
	//	(caps.PixelShaderVersion >> 8) & 0xF,
	//	(caps.PixelShaderVersion) & 0xF ));
	//Logger::Info(Format("[RenderSystemDX9] maxVSinstr:%d maxPSinstr:%d", 
	//	caps.MaxVShaderInstructionsExecuted,
	//	caps.MaxPShaderInstructionsExecuted));
	//Logger::Info(Format("[RenderSystemDX9] maxTextures:%d", caps.MaxSimultaneousTextures));
	//Logger::Info(Format("[RenderSystemDX9] maxVertexIndex:%d", caps.MaxVertexIndex));
	//Logger::Info(Format("[RenderSystemDX9] renderTargetCount:%d", caps.NumSimultaneousRTs));
	//Logger::Info("[RenderSystemDX9] create successfull.");
	DetectRenderingCapabilities();
	SetupDefaultDeviceState();

	D3DSafeRelease(renderQuery);
	RENDER_VERIFY(direct3DDevice->CreateQuery(D3DQUERYTYPE_EVENT, &renderQuery));

	Logger::Debug("[RenderManager::Create] successfull");
	return true;
}


void RenderManager::DetectRenderingCapabilities()
{
	caps.isHardwareCursorSupported = false;
	//if (D3DCURSORCAPS_COLOR & deviceCaps.CursorCaps)
	//	caps.isHardwareCursorSupported = true;
}



void RenderManager::SetupDefaultDeviceState()
{
	Logger::Debug("[RenderManagerDX9 setup default device state");
	
	RENDER_VERIFY(direct3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE));

	RENDER_VERIFY(direct3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE));

	RENDER_VERIFY(direct3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE));
	RENDER_VERIFY(direct3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA));
	RENDER_VERIFY(direct3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA));

	RENDER_VERIFY(direct3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE ));
	RENDER_VERIFY(direct3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE ));
	RENDER_VERIFY(direct3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE ));

	RENDER_VERIFY(direct3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE ));
	RENDER_VERIFY(direct3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE ));
	RENDER_VERIFY(direct3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE ));


	RENDER_VERIFY(direct3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR));
	RENDER_VERIFY(direct3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR));
	RENDER_VERIFY(direct3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR));
	RENDER_VERIFY(direct3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP));
	RENDER_VERIFY(direct3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP));


	RENDER_VERIFY(direct3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE));
	RENDER_VERIFY(direct3DDevice->SetRenderState(D3DRS_ZWRITEENABLE , FALSE));
	RENDER_VERIFY(direct3DDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE));
	scissorTestEnabled = false;

	RENDER_VERIFY(direct3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE));
	RENDER_VERIFY(direct3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE));
	RENDER_VERIFY(direct3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));

	identity.Identity();

	if (!vertexBuffer)
	{
		vertexBuffer = new DynamicVertexBuffer(EVF_COORD | EVF_COLOR | EVF_TEXCOORD0, 4096);
		if (!vertexBuffer)return;
	}
	
	if (!indexBuffer)
	{
		indexBuffer = new DynamicIndexBuffer(EIF_16, 4096 * 3);
		if (!indexBuffer)return;
	}

	Reset();
	FlushState();
}

void RenderManager::Release()
{
	for (int32 k = 0; k < 4; ++k)
	{
		if (direct3DDevice)
			RENDER_VERIFY(direct3DDevice->SetTexture(k, 0));
	}


	D3DSafeRelease(depthStencilSurface);
	D3DSafeRelease(backBufferSurface);

	SafeRelease(vertexBuffer);
	SafeRelease(indexBuffer);

	D3DSafeRelease(renderQuery);
	D3DSafeRelease(direct3DDevice);
	D3DSafeRelease(direct3D);

	Logger::Debug("[RenderManager::Release] successfull");
}

void RenderManager::OnDeviceLost()
{
	RenderResource::LostAllResources();
	
	D3DSafeRelease(renderQuery);
	D3DSafeRelease(depthStencilSurface);
	D3DSafeRelease(backBufferSurface);
}

void RenderManager::OnDeviceRestore()
{
	RenderResource::InvalidateAllResources();
	SetupDefaultDeviceState();
	RENDER_VERIFY(direct3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBufferSurface));
	RENDER_VERIFY(direct3DDevice->GetDepthStencilSurface(&depthStencilSurface));

	D3DSafeRelease(renderQuery);
	RENDER_VERIFY(direct3DDevice->CreateQuery(D3DQUERYTYPE_EVENT, &renderQuery));
}

bool RenderManager::IsDeviceLost()
{
	HRESULT hr = direct3DDevice->TestCooperativeLevel(); 
	if (hr == D3DERR_DEVICELOST)
	{
		Logger::Debug("[RenderManagerDX9] device lost");
		OnDeviceLost();
		Sleep(50);
		return true;
	}
	else if (hr == D3DERR_DEVICENOTRESET)
	{	
		OnDeviceLost();
		Sleep(50);
		hr = direct3DDevice->Reset(&presentParams);
		if (SUCCEEDED(hr))
		{
			OnDeviceRestore();
		}
		else{
			Logger::Error("[RenderManagerDX9] Was not able to reset DirectX9 device after lost device. %s\n", DXHelperGetErrorDescription(hr));
		}
		return true;
	}
	else if (hr == S_OK)
	{
		//Logger::Debug("[RenderManagerDX9] device active");
		return false;
	}
	return true;
}

void RenderManager::BeginFrame()
{
	//RENDER_VERIFY(renderQuery->Issue(D3DISSUE_BEGIN));
	RENDER_VERIFY(direct3DDevice->BeginScene());



	direct3DDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&identity);
	direct3DDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&identity);
	
	Reset();

	vertexBuffer->Flush();
	SetRenderOrientation(Core::Instance()->GetScreenOrientation());
	isInsideDraw = true;
}

void RenderManager::EndFrame()
{

	POINT ptCursor;
	GetCursorPos( &ptCursor );
	ScreenToClient(hWnd, &ptCursor);
	if (caps.isHardwareCursorSupported)
	{
		LPDIRECT3DDEVICE9 device = RenderManager::Instance()->GetD3DDevice();
		if (device)device->SetCursorPosition( ptCursor.x, ptCursor.y, D3DCURSOR_IMMEDIATE_UPDATE);
	}else
	{
		Vector2 cursorPos((float32)ptCursor.x, (float32)ptCursor.y);
		cursorPos -= Core::Instance()->GetPhysicalDrawOffset();	
		cursorPos *= Core::Instance()->GetPhysicalToVirtualFactor();
		if(cursor)
			cursor->SoftwareDraw(cursorPos);
	}

	isInsideDraw = false;
	RENDER_VERIFY(direct3DDevice->EndScene());


	HRESULT hr = S_OK;
// 	if (!presentParams.Windowed)
// 	{
	hr = direct3DDevice->Present( NULL, NULL, NULL, NULL );
// 	}else
// 	{
// 		RECT sourceRect;
// 		sourceRect.left = 0;
// 		sourceRect.top = 0;
// 		sourceRect.right = frameBufferWidth;
// 		sourceRect.bottom = frameBufferHeight;
// 		Logger::Debug("Present: %d %d", sourceRect.right, sourceRect.bottom);
// 		hr = direct3DDevice->Present(&sourceRect, 0, hWnd, 0);
// 	}
	if (hr == D3DERR_DEVICELOST)
	{
		Logger::Debug("RenderManager::EndFrame() device lost");
		IsDeviceLost();
	}


	RENDER_VERIFY(renderQuery->Issue(D3DISSUE_END));

	// Force the driver to execute the commands from the command buffer.
	// Empty the command buffer and wait until the GPU is idle.
	//DWORD numberOfPixelsDrawn;
	while(S_FALSE == renderQuery->GetData( NULL, 0, D3DGETDATA_FLUSH ));

	/*while(1)
	{
		HRESULT queryResult = renderQuery->GetData( &numberOfPixelsDrawn, sizeof(DWORD), D3DGETDATA_FLUSH ));
		if (queryResult == D3DERR_DEVICELOST)
		{
			IsDeviceLost();
			break;
		}
		if (queryResult == S_OK)break;
	}*/
	
	RENDER_VERIFY(hr);
}


void RenderManager::SetViewport(const Rect & rect)
{
	int32 x = (int32)(rect.x * currentDrawScale.x + currentDrawOffset.x);
	int32 y = (int32)(rect.y * currentDrawScale.y + currentDrawOffset.y);
	int32 width = (int32)(rect.dx * currentDrawScale.x);
	int32 height = (int32)(rect.dy * currentDrawScale.y);    
    
    switch(renderOrientation)
    {
        case Core::SCREEN_ORIENTATION_PORTRAIT:
        break;    
		default:
			Logger::Error("");
			break;
            
    }  
	D3DVIEWPORT9 vp;
	vp.X      = x;
	vp.Y      = y;
	vp.Width  = width;
	vp.Height = height;
	vp.MinZ   = 0.0f;
	vp.MaxZ   = 1.0f;

	direct3DDevice->SetViewport(&vp);
}



// Viewport management
void RenderManager::SetRenderOrientation(int32 orientation)
{
	renderOrientation = orientation;
	

	projection.BuildOrthoLH(0.0f, (float32)frameBufferWidth, (float32)frameBufferHeight, 0.0f, 0.0f, 1.0f);
	direct3DDevice->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&projection);

	IdentityTotalMatrix();
	SetVirtualViewScale();
	SetVirtualViewOffset();

	switch (orientation) 
	{
		case Core::SCREEN_ORIENTATION_PORTRAIT:
		case Core::SCREEN_ORIENTATION_TEXTURE:
			retScreenWidth = frameBufferWidth;
			retScreenHeight = frameBufferHeight;
			break;
		case Core::SCREEN_ORIENTATION_LANDSCAPE_LEFT:
			Logger::Error("[RenderManagerDX9] Unsupported orientation in direct X (landscape left)");
			break;
		case Core::SCREEN_ORIENTATION_LANDSCAPE_RIGHT:
			Logger::Error("[RenderManagerDX9] Unsupported orientation in direct X (landscape right)");
			break;
	}
}


void RenderManager::SetBlendMode(eBlendMode sfactor, eBlendMode dfactor)
{
	newSFactor = sfactor;
	newDFactor = dfactor;
}
	
eBlendMode RenderManager::GetSrcBlend()
{
	return newSFactor;
}

eBlendMode RenderManager::GetDestBlend()
{
	return newDFactor;
}

void RenderManager::EnableBlending(bool isEnabled)
{
	if(isEnabled != oldBlendingEnabled)
	{
		RENDER_VERIFY(direct3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, isEnabled));
		oldBlendingEnabled = isEnabled;
	}
}

void RenderManager::EnableDepthTest(bool isEnabled)
{
	RENDER_VERIFY(direct3DDevice->SetRenderState(D3DRS_ZENABLE, isEnabled));
}
    
void RenderManager::EnableDepthWrite(bool isEnabled)
{
	RENDER_VERIFY(direct3DDevice->SetRenderState(D3DRS_ZWRITEENABLE , isEnabled));
}
    

static GLint BLEND_MODE_MAP[BLEND_MODE_COUNT] = 
{
		0,	// not a valid blend mode
		D3DBLEND_ZERO,
		D3DBLEND_ONE,
		D3DBLEND_DESTCOLOR,
		D3DBLEND_INVDESTCOLOR,
		D3DBLEND_SRCALPHA,
		D3DBLEND_INVSRCALPHA,
		D3DBLEND_DESTALPHA,
		D3DBLEND_INVDESTALPHA,
		D3DBLEND_SRCALPHASAT,
};

/*
	D3DBLEND_ZERO              = 1,
	D3DBLEND_ONE               = 2,
	D3DBLEND_SRCCOLOR          = 3,
	D3DBLEND_INVSRCCOLOR       = 4,
	D3DBLEND_SRCALPHA          = 5,
	D3DBLEND_INVSRCALPHA       = 6,
	D3DBLEND_DESTALPHA         = 7,
	D3DBLEND_INVDESTALPHA      = 8,
	D3DBLEND_DESTCOLOR         = 9,
	D3DBLEND_INVDESTCOLOR      = 10,
	D3DBLEND_SRCALPHASAT       = 11,
	D3DBLEND_BOTHSRCALPHA      = 12,
	D3DBLEND_BOTHINVSRCALPHA   = 13,
	D3DBLEND_BLENDFACTOR       = 14,
	D3DBLEND_INVBLENDFACTOR    = 15,
	D3DBLEND_SRCCOLOR2         = 16,
	D3DBLEND_INVSRCCOLOR2      = 17,
*/

void RenderManager::FlushState()
{
	if(newSFactor != oldSFactor || newDFactor != oldDFactor)
	{
		if(newSFactor != oldSFactor )
			RENDER_VERIFY(direct3DDevice->SetRenderState(D3DRS_SRCBLEND, BLEND_MODE_MAP[newSFactor]));
		if (newDFactor != oldDFactor)
			RENDER_VERIFY(direct3DDevice->SetRenderState(D3DRS_DESTBLEND, BLEND_MODE_MAP[newDFactor]));
		
		oldSFactor = newSFactor;
		oldDFactor = newDFactor;
	}
	if(oldR != newR || oldG != newG || oldB != newB || oldA != newA)
	{
		// TODO: FIX COLORS & RenderEffects
		oldR = newR;
		oldG = newG;
		oldB = newB;
		oldA = newA;
	}
	if(newTextureEnabled != oldTextureEnabled)
	{
		if(newTextureEnabled)
		{
			//direct3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);


			RENDER_VERIFY(direct3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE ));
			RENDER_VERIFY(direct3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE ));

	
		}
		else
		{
			RENDER_VERIFY(direct3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2 ));
			RENDER_VERIFY(direct3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2));
			//direct3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		}
		oldTextureEnabled = newTextureEnabled;
	}

	PrepareRealMatrix();
}

void RenderManager::EnableVertexArray(bool isEnabled)
{
	if(isEnabled != oldVertexArrayEnabled)
	{
		buffers[BUFFER_VERTEX].isEnabled = isEnabled;
		oldVertexArrayEnabled = isEnabled;
	}
}
void RenderManager::EnableTextureCoordArray(bool isEnabled)
{
	if(isEnabled != oldTextureCoordArrayEnabled)
	{
		buffers[BUFFER_TEXCOORD0].isEnabled = isEnabled;
		oldTextureCoordArrayEnabled = isEnabled;
	}
}
	
void RenderManager::EnableColorArray(bool isEnabled)
{
	if(isEnabled != oldColorArrayEnabled)
	{
		buffers[BUFFER_COLOR].isEnabled = isEnabled;
		oldTextureCoordArrayEnabled = isEnabled;
		oldColorArrayEnabled = isEnabled;
	}
}	

GLint types[TYPE_COUNT] = {GL_FLOAT};

void RenderManager::SetVertexPointer(int size, eVertexDataType _typeIndex, int stride, const void *pointer)
{
	buffers[BUFFER_VERTEX].size = size;
	buffers[BUFFER_VERTEX].type = _typeIndex;
	buffers[BUFFER_VERTEX].stride = stride;
	buffers[BUFFER_VERTEX].pointer = pointer;

// 	uint8 * ptr = (uint8*)buffers[BUFFER_VERTEX].pointer;
// 	for(int k = 0; k < 4; ++k)
// 	{
// 		if (buffers[BUFFER_VERTEX].size == 2)
// 		{	
// 			float32 x = (*(float32*)ptr) - 0.5f;
// 			float32 y = (*(float32*)(ptr + 4)) - 0.5f;
// 
// 			Logger::Debug("vset:%d %f %f",
// 				k,
// 				x,
// 				y);
// 
//			ptr += 8;
// 		}
// 	}
}

void RenderManager::SetTexCoordPointer(int size, eVertexDataType _typeIndex, int stride, const void *pointer)
{
	buffers[BUFFER_TEXCOORD0].size = size;
	buffers[BUFFER_TEXCOORD0].type = _typeIndex;
	buffers[BUFFER_TEXCOORD0].stride = stride;
	buffers[BUFFER_TEXCOORD0].pointer = pointer;
}

void RenderManager::SetNormalPointer(eVertexDataType _typeIndex, int stride, const void *pointer)
{
	buffers[BUFFER_NORMAL].type = _typeIndex;
	buffers[BUFFER_NORMAL].stride = stride;
	buffers[BUFFER_NORMAL].pointer = pointer;
}

void RenderManager::SetColorPointer(int size, eVertexDataType _typeIndex, int stride, const void *pointer)
{
	buffers[BUFFER_COLOR].size = size;
	buffers[BUFFER_COLOR].type = _typeIndex;
	buffers[BUFFER_COLOR].stride = stride;
	buffers[BUFFER_COLOR].pointer = pointer;
}
	
	


typedef struct{
	enum { FVF = D3DFVF_XYZ| D3DFVF_DIFFUSE |D3DFVF_TEX1 };
	float		X, Y, Z;
	uint32		Diffuse;             
	float		U, V;       
}RTTRVertex;

//! Render Textured Test Rect
void RTTR()
{

	RTTRVertex     RenderTestList[] =
	{{0.0, 0.0, 0.0, 0xffffffff,1.0,1.0},
	{250.0,0.0 ,0.0, 0xffff0000,1.0,0.0},
	{0.0, 250.0 ,0.0, 0xff00ff00,0.0,0.0},
	{250.0, 250.0 ,0.0, 0xff0000ff,0.0,1.0}};


	/*d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	*/

	//d3dDevice->SetTexture(0,Texture->lpTexture);
	HRESULT d3dr=direct3DDevice->SetFVF(RTTRVertex::FVF);
	//if (d3dr!=D3D_OK);//gConsole->SendMessage("Draw Mem Set Vert Shader Failed\n");

	d3dr=direct3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,RenderTestList,sizeof(RTTRVertex));
	//if (d3dr!=D3D_OK)gConsole->SendMessage("DX8Device: DrawPrimitiveUp Failed\n");
}


void RenderManager::DrawElements(ePrimitiveType type, int32 count, eIndexFormat indexFormat, void * _inpIndices)
{
#if 1
	int32 startIndex = 0;
	uint16 * indices = (uint16*)indexBuffer->Lock(count, startIndex);
	uint8 * inpIndices = (uint8*)_inpIndices;
	int32 indexStride = (indexFormat == EIF_16) ? (2) : (4);

	int32 minIndex = std::numeric_limits<int32>::max();
	int32 maxIndex = 0;
	for (int32 i = 0; i < count; ++i)
	{
		int32 index = 0; 
		switch(indexFormat)
		{
			case EIF_16:
				index = *(uint16*)inpIndices;
				break;
			case EIF_32:
				index = *(uint32*)inpIndices;
				break;
		}
		if (index < minIndex) minIndex = index;
		if (index > maxIndex) maxIndex = index;
		*indices = (uint16)index;
		indices++;
		inpIndices += indexStride;
	}
	indexBuffer->Unlock();
	/*
		VB: It's not an index, it's actually a number of vertices after this increment. DX require number of vertices in DrawIndexedPrimitive call.
	*/
	maxIndex++;

	RENDER_VERIFY(direct3DDevice->SetIndices(indexBuffer->GetNativeBuffer()));


	int32 baseVertexIndex = 0;
	RenderManagerVertex2D * dataPtr = (RenderManagerVertex2D * )vertexBuffer->Lock(maxIndex, baseVertexIndex);


	if (buffers[BUFFER_VERTEX].isEnabled)
	{
		uint8 * ptr = (uint8*)buffers[BUFFER_VERTEX].pointer;
		int32 stride = buffers[BUFFER_VERTEX].stride;
		// Logger::Debug("cnt: %d, start: %d stride: %d ptr:%08x", count, startVertex, stride, buffers[BUFFER_VERTEX].pointer);
		for(int k = 0; k < maxIndex; ++k)
		{
			if (buffers[BUFFER_VERTEX].size == 2)
			{	
				dataPtr[k].x = (*(float32*)ptr) - 0.5f;
				dataPtr[k].y = *(float32*)(ptr + 4) - 0.5f;
				dataPtr[k].z = 0;

				// 					Logger::Debug("v:%d %f %f %f",
				// 						k,
				// 						*(float32*)ptr,
				// 						*(float32*)(ptr + 4),
				// 						*(float32*)(ptr + 8));

				ptr += 8;
			}else if (buffers[BUFFER_VERTEX].size == 3)
			{
				dataPtr[k].x = (*(float32*)ptr) - 0.5f;
				dataPtr[k].y = *(float32*)(ptr + 4) - 0.5f;
				dataPtr[k].z = *(float32*)(ptr + 8);

				ptr += 12;
			}
			ptr += stride;
		}
	}
	for(int k = 0; k < maxIndex; ++k)
	{
		dataPtr[k].color = D3DCOLOR_COLORVALUE(oldR * oldA, oldG * oldA, oldB * oldA, oldA);
		dataPtr[k].u = 0;
		dataPtr[k].v = 0;
		// Logger::Debug("v:%f %f %f %f c: #%08x", oldR, oldG, oldB, oldA, dataPtr[k].color);
	}
	if (buffers[BUFFER_TEXCOORD0].isEnabled)
	{
		uint8 * ptr = (uint8*)buffers[BUFFER_TEXCOORD0].pointer;
		int32 stride = buffers[BUFFER_TEXCOORD0].stride;
		for(int k = 0; k < maxIndex; ++k)
		{
			if (buffers[BUFFER_TEXCOORD0].size == 2)
			{	
				dataPtr[k].u = *(float32*)ptr;
				dataPtr[k].v = *(float32*)(ptr + 4);

				ptr += 8;
			}
			ptr += stride;
		}
	}

	// 		for(int k = 0; k < count; ++k)
	// 		{
	// 			Logger::Debug("v:%d %f %f %f uv: %f %f c: #%08x",
	// 				k,
	// 				dataPtr[k].x,
	// 				dataPtr[k].y,
	// 				dataPtr[k].z,
	// 				dataPtr[k].u,
	// 				dataPtr[k].v,
	// 				dataPtr[k].color);
	// 		}

	vertexBuffer->Unlock();

	

	//Logger::Debug("[RenderManager::DrawArrays] type: %d startVertex: %d count: %d", type, startVertex, count);
	RENDER_VERIFY(direct3DDevice->SetStreamSource(0, vertexBuffer->GetNativeBuffer(), 0, vertexBuffer->GetVertexSize()));
	RENDER_VERIFY(direct3DDevice->SetFVF(vertexBuffer->GetFVF()));

	int32 primitiveCount = 0;
	D3DPRIMITIVETYPE d3dtype;


	/*= D3DPT_TRIANGLESTRIP;
	primitiveCount = count - 2;

	if (type == PRIMITIVETYPE_TRIANGLEFAN)
	{
	d3dtype = D3DPT_TRIANGLEFAN;
	primitiveCount = count - 2;
	}
	else if (type == PRIMITIVETYPE_LINELIST)
	{
	d3dtype = D3DPT_LINES;
	}*/
	switch(type)
	{
	case PRIMITIVETYPE_TRIANGLEFAN:
		primitiveCount = count - 2;
		d3dtype = D3DPT_TRIANGLEFAN;
		break;
	case PRIMITIVETYPE_TRIANGLESTRIP:
		primitiveCount = count - 2;
		d3dtype = D3DPT_TRIANGLESTRIP;
		break;
	case PRIMITIVETYPE_TRIANGLELIST:
		primitiveCount = count / 3;
		d3dtype = D3DPT_TRIANGLELIST;
		break;		
	case PRIMITIVETYPE_LINELIST:
		primitiveCount = count / 2;
		d3dtype = D3DPT_LINELIST;
		break;		
	case PRIMITIVETYPE_LINESTRIP:
		primitiveCount = count - 1;
		d3dtype = D3DPT_LINESTRIP;
		break;		
	case PRIMITIVETYPE_POINTLIST:
		primitiveCount = count;
		d3dtype = D3DPT_POINTLIST;
		break;		
	default:
		Logger::Error("*** Error this type of primite is not implemented %d", type);
		break;
	}

	RENDER_VERIFY(direct3DDevice->DrawIndexedPrimitive(d3dtype, 
		baseVertexIndex,   // it will be added to all indices (base vertex index)										
		0, 
		maxIndex,
		startIndex,
		primitiveCount));

	
	RENDER_VERIFY(direct3DDevice->SetIndices(0));
#endif
}

void RenderManager::DrawArrays(ePrimitiveType type, int32 first, int32 count)
{
	//RTTR();

	if(debugEnabled)
	{
		Logger::Debug("Draw arrays texture: id %d", currentTexture->id);
	}
	//if (type == PRIMITIVETYPE_TRIANGLESTRIP || type == PRIMITIVETYPE_TRIANGLEFAN || || type == PRIMITIVETYPE_TRIANGLEFAN))
	{
		int32 startVertex = 0;

		RenderManagerVertex2D * dataPtr = (RenderManagerVertex2D * )vertexBuffer->Lock(count, startVertex);


		if (buffers[BUFFER_VERTEX].isEnabled)
		{
			uint8 * ptr = (uint8*)buffers[BUFFER_VERTEX].pointer;
			int32 stride = buffers[BUFFER_VERTEX].stride;
			// Logger::Debug("cnt: %d, start: %d stride: %d ptr:%08x", count, startVertex, stride, buffers[BUFFER_VERTEX].pointer);
			for(int k = 0; k < count; ++k)
			{
				if (buffers[BUFFER_VERTEX].size == 2)
				{	
					dataPtr[k].x = (*(float32*)ptr) - 0.5f;
					dataPtr[k].y = *(float32*)(ptr + 4) - 0.5f;
					dataPtr[k].z = 0;

// 					Logger::Debug("v:%d %f %f %f",
// 						k,
// 						*(float32*)ptr,
// 						*(float32*)(ptr + 4),
// 						*(float32*)(ptr + 8));

					ptr += 8;
				}else if (buffers[BUFFER_VERTEX].size == 3)
				{
					dataPtr[k].x = (*(float32*)ptr) - 0.5f;
					dataPtr[k].y = *(float32*)(ptr + 4) - 0.5f;
					dataPtr[k].z = *(float32*)(ptr + 8);

					ptr += 12;
				}
				ptr += stride;
			}
		}
		for(int k = 0; k < count; ++k)
		{
			dataPtr[k].color = D3DCOLOR_COLORVALUE(oldR * oldA, oldG * oldA, oldB * oldA, oldA);
			dataPtr[k].u = 0;
			dataPtr[k].v = 0;
			// Logger::Debug("v:%f %f %f %f c: #%08x", oldR, oldG, oldB, oldA, dataPtr[k].color);
		}
		if (buffers[BUFFER_TEXCOORD0].isEnabled)
		{
			uint8 * ptr = (uint8*)buffers[BUFFER_TEXCOORD0].pointer;
			int32 stride = buffers[BUFFER_TEXCOORD0].stride;
			for(int k = 0; k < count; ++k)
			{
				if (buffers[BUFFER_TEXCOORD0].size == 2)
				{	
					dataPtr[k].u = *(float32*)ptr;
					dataPtr[k].v = *(float32*)(ptr + 4);

					ptr += 8;
				}
				ptr += stride;
			}
		}

// 		for(int k = 0; k < count; ++k)
// 		{
// 			Logger::Debug("v:%d %f %f %f uv: %f %f c: #%08x",
// 				k,
// 				dataPtr[k].x,
// 				dataPtr[k].y,
// 				dataPtr[k].z,
// 				dataPtr[k].u,
// 				dataPtr[k].v,
// 				dataPtr[k].color);
// 		}

		vertexBuffer->Unlock();



		//Logger::Debug("[RenderManager::DrawArrays] type: %d startVertex: %d count: %d", type, startVertex, count);
		RENDER_VERIFY(direct3DDevice->SetStreamSource(0, vertexBuffer->GetNativeBuffer(), 0, vertexBuffer->GetVertexSize()));


		RENDER_VERIFY(direct3DDevice->SetFVF(vertexBuffer->GetFVF()));


		int32 primitiveCount = 0;
		
		D3DPRIMITIVETYPE d3dtype;
		
		
		/*= D3DPT_TRIANGLESTRIP;
		primitiveCount = count - 2;
		
		if (type == PRIMITIVETYPE_TRIANGLEFAN)
		{
			d3dtype = D3DPT_TRIANGLEFAN;
			primitiveCount = count - 2;
		}
		else if (type == PRIMITIVETYPE_LINELIST)
		{
			d3dtype = D3DPT_LINES;
		}*/
		switch(type)
		{
			case PRIMITIVETYPE_TRIANGLEFAN:
				primitiveCount = count - 2;
				d3dtype = D3DPT_TRIANGLEFAN;
				break;
			case PRIMITIVETYPE_TRIANGLESTRIP:
				primitiveCount = count - 2;
				d3dtype = D3DPT_TRIANGLESTRIP;
				break;
			case PRIMITIVETYPE_TRIANGLELIST:
				primitiveCount = count / 3;
				d3dtype = D3DPT_TRIANGLELIST;
				break;		
			case PRIMITIVETYPE_LINELIST:
				primitiveCount = count / 2;
				d3dtype = D3DPT_LINELIST;
				break;		
			case PRIMITIVETYPE_LINESTRIP:
				primitiveCount = count - 1;
				d3dtype = D3DPT_LINESTRIP;
				break;		
			case PRIMITIVETYPE_POINTLIST:
				primitiveCount = count;
				d3dtype = D3DPT_POINTLIST;
				break;		
			default:
				Logger::Error("*** Error this type of primite is not implemented %d", type);
				break;
		}
		
		RENDER_VERIFY(direct3DDevice->DrawPrimitive(d3dtype, startVertex, primitiveCount));
	}	
}

void RenderManager::ClearWithColor(float32 r, float32 g, float32 b, float32 a)
{
	RENDER_VERIFY(direct3DDevice->Clear(0,  0, D3DCLEAR_TARGET,  D3DCOLOR_COLORVALUE(r,g,b,a),  1.0f,  0));
}

void RenderManager::ClearDepthBuffer(float32 depth)
{
    RENDER_VERIFY(direct3DDevice->Clear(0,  0, D3DCLEAR_ZBUFFER,  0,  depth,  0));
}

void RenderManager::SetHWClip(const Rect &rect)
{
	PrepareRealMatrix();
	currentClip = rect;
	if(rect.dx < 0 || rect.dy < 0)
	{
		if (scissorTestEnabled)
		{
			RENDER_VERIFY(direct3DDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false));
			scissorTestEnabled = false;
		}
		return;
	}
	int32 x = (int32)(rect.x * currentDrawScale.x + currentDrawOffset.x);
	int32 y = (int32)(rect.y * currentDrawScale.y + currentDrawOffset.y);
	int32 width = (int32)(rect.dx * currentDrawScale.x);
	int32 height = (int32)(rect.dy * currentDrawScale.y);

	switch (renderOrientation) 
	{
	case Core::SCREEN_ORIENTATION_PORTRAIT:
		{
			//			x = frameBufferWidth - x;
			//			y = frameBufferHeight/* * Core::GetVirtualToPhysicalFactor()*/ - y - height;
		}
		break;
	case Core::SCREEN_ORIENTATION_LANDSCAPE_LEFT:
		{
			int32 tmpY = y;
			y = x;
			x = tmpY;
			tmpY = height;
			height = width;
			width = tmpY;
		}
		break;
	case Core::SCREEN_ORIENTATION_LANDSCAPE_RIGHT:
		{
			int32 tmpY = height;
			height = width;
			width = tmpY;
			tmpY = y;
			y = frameBufferHeight/* * Core::GetVirtualToPhysicalFactor()*/ - x - height;
			x = frameBufferWidth/* * Core::GetVirtualToPhysicalFactor()*/ - tmpY - width;
		}
		break;
	}
	if (!scissorTestEnabled)
	{
		RENDER_VERIFY(direct3DDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, true));
		scissorTestEnabled = true;
	}	
	RECT hwRect;
	hwRect.left = x;
	hwRect.top = y;
	hwRect.right = x + width;
	hwRect.bottom = y + height;
	direct3DDevice->SetScissorRect(&hwRect);
}

void RenderManager::SetHWRenderTarget(Sprite *renderTarget)
{
	//Logger::Debug("[RenderManagerDX9] set render target: 0x%08x", renderTarget);
	if (renderTarget == 0)
	{
		RENDER_VERIFY(direct3DDevice->SetRenderTarget(0, backBufferSurface));
		RENDER_VERIFY(direct3DDevice->SetDepthStencilSurface(depthStencilSurface));

		D3DVIEWPORT9 vp;
		vp.X      = 0;
		vp.Y      = 0;
		vp.Width  = (DWORD)GetScreenWidth();
		vp.Height = (DWORD)GetScreenHeight();
		vp.MinZ   = 0.0f;
		vp.MaxZ   = 1.0f;

		direct3DDevice->SetViewport(&vp);

		
		SetRenderOrientation(Core::Instance()->GetScreenOrientation());
	}
	else
	{
		renderOrientation = Core::SCREEN_ORIENTATION_TEXTURE;
/*
#if defined(__DAVAENGINE_IPHONE__)
		glBindFramebufferOES(GL_FRAMEBUFFER_OES, renderTarget->GetTexture()->fboID);
#else
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, renderTarget->GetTexture()->fboID);
#endif

		glViewport(0, 0, renderTarget->GetTexture()->width, renderTarget->GetTexture()->height);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
#if defined(__DAVAENGINE_IPHONE__)
		glOrthof(0.0f, renderTarget->GetTexture()->width, 0.0f, renderTarget->GetTexture()->height, -1.0f, 1.0f);
#else 
		glOrtho(0.0f, renderTarget->GetTexture()->width, 0.0f, renderTarget->GetTexture()->height, -1.0f, 1.0f);
#endif
*/
		//glMatrixMode(GL_MODELVIEW);
		//glLoadIdentity();
		Texture * textureFwk = renderTarget->GetTexture(); 
		textureFwk->renderTargetModified = true;
		LPDIRECT3DTEXTURE9 texture = textureFwk->id;

		LPDIRECT3DSURFACE9 surface;
		RENDER_VERIFY(texture->GetSurfaceLevel(0, &surface));
		RENDER_VERIFY(direct3DDevice->SetRenderTarget(0, surface));
		RENDER_VERIFY(direct3DDevice->SetDepthStencilSurface(0));

		D3DVIEWPORT9 vp;
		vp.X      = 0;
		vp.Y      = 0;
		vp.Width  = (DWORD)renderTarget->GetTexture()->width;
		vp.Height = (DWORD)renderTarget->GetTexture()->height;
		vp.MinZ   = 0.0f;
		vp.MaxZ   = 1.0f;

		RENDER_VERIFY(direct3DDevice->SetViewport(&vp));
		D3DSafeRelease(surface);

		projection.BuildOrthoLH(0.0f, (float32)vp.Width, (float32)vp.Height, 0.0f, 0.0f, 1.0f);
		RENDER_VERIFY(direct3DDevice->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&projection));

		RENDER_VERIFY(direct3DDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&identity));
		RENDER_VERIFY(direct3DDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&identity));

		IdentityTotalMatrix();
		viewMappingDrawScale.x = renderTarget->GetResourceToPhysicalFactor();
		viewMappingDrawScale.y = renderTarget->GetResourceToPhysicalFactor();
		RemoveClip();
	}

	currentRenderTarget = renderTarget;
}

//void RenderManager::SetDrawOffset(const Vector2 &offset)
//{
//	Matrix4 m;
//	m.CreateTranslation(Vector3(offset.x, offset.y, 0));
//	direct3DDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&m);
//}

void RenderManager::PrepareRealMatrix()
{
	realDrawScale.x = viewMappingDrawScale.x * userDrawScale.x;
	realDrawScale.y = viewMappingDrawScale.y * userDrawScale.y;
		
	realDrawOffset.x = viewMappingDrawOffset.x + userDrawOffset.x * viewMappingDrawScale.x;
	realDrawOffset.y = viewMappingDrawOffset.y + userDrawOffset.y * viewMappingDrawScale.y;
	if (realDrawScale != currentDrawScale || realDrawOffset != currentDrawOffset) 
	{
		currentDrawScale = realDrawScale;
		currentDrawOffset = realDrawOffset;
		
		Matrix4 m1;
		m1.CreateTranslation(Vector3(currentDrawOffset.x, currentDrawOffset.y, 0));
		Matrix4 m2;
		m2.CreateScale(Vector3(currentDrawScale.x, currentDrawScale.y, 0));
		m2 *= m1;
		direct3DDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&m2);
	}
}

void RenderManager::SetMatrix(eMatrixType type, const Matrix4 & matrix)
{
	matrices[type] = matrix;
}

};

#endif // __DAVAENGINE_DIRECTX9__