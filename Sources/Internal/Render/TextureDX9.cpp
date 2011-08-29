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
#include "Render/Texture.h"
#include "Render/RenderManager.h"
#include "Render/D3D9Helpers.h"
#include "Render/Image.h"
#include "Utils/StringFormat.h"

namespace DAVA
{

#if defined(__DAVAENGINE_DIRECTX9__)

//! Create render targets or shadow containers
//! Or generated textures
// This function must create texture in any case 
// because one of formats must be supported by 
// hardware.
// If they failed -- try to change your video card! 

// PRIVATE TextureDX9 Declarations
uint32 GetLog2Size(uint32 size)
{
	uint32 desiredSize = 0x01;

	while(desiredSize < size)
		desiredSize <<= 1;

	//if (DesiredSize > Size && DesiredSize > 64)
	//	DesiredSize >>= 1;

	return desiredSize;
}

void Texture::SaveToSystemMemory()
{
	if (isRenderTarget)
	{
		HRESULT hr = RenderManager::Instance()->GetD3DDevice()->TestCooperativeLevel(); 
		if (hr == D3DERR_DEVICELOST)
		{
			if (!saveTexture)
				Logger::Debug("Trying to save to system memory rendertarget that was not saved before");
			return;
		}
		D3DSafeRelease(saveTexture);

		/*
			Render manager set this flag  when you set sprite as render target.
		*/
		if (!renderTargetModified)
			return;

		//Logger::Debug("Saving render target to system memory: %s size: %d x %d", relativePathname.c_str(), width, height);
		LPDIRECT3DDEVICE9 device = RenderManager::Instance()->GetD3DDevice();
		
		D3DSURFACE_DESC desc;
		id->GetLevelDesc(0, &desc);
		/*
		HRESULT hr = device->CreateOffscreenPlainSurface(width, height, desc.Format, D3DPOOL_SYSTEMMEM, &saveSurface, NULL);
		DX_VERIFY(hr);
		*/

		hr = device->CreateTexture(width, height, 
			1/*means we create texture with 1 mipmap level*/,
			0,  desc.Format, D3DPOOL_SYSTEMMEM, &saveTexture, 0);
		RENDER_VERIFY(hr);

		LPDIRECT3DSURFACE9 renderTargetMainSurface; 
		hr = id->GetSurfaceLevel(0, &renderTargetMainSurface);
		RENDER_VERIFY(hr);

		LPDIRECT3DSURFACE9 saveTextureMainSurface; 
		hr = saveTexture->GetSurfaceLevel(0, &saveTextureMainSurface);
		RENDER_VERIFY(hr);

		hr = device->GetRenderTargetData(renderTargetMainSurface, saveTextureMainSurface);
		RENDER_VERIFY(hr);

		renderTargetModified = false;

#if 0
		//Image * image = new Image();
		Image * image = image->Create(width, height, Image::FORMAT_RGBA8888);
		D3DLOCKED_RECT rect;
		hr = saveTexture->LockRect(0, &rect, 0, 0);
		if (FAILED(hr))
		{
			Logger::Error("[TextureDX9] Could not lock DirectX9 Texture.");
			return;
		}
		int32 pixelSizeInBits = GetPixelFormatSize(format);
		if (format ==  Texture::FORMAT_RGBA8888)
		{
			//int32 pitchInBytes = 

			uint8 * destBits = (uint8*)image->GetData();
			uint8 * sourceBits = (uint8*)rect.pBits;
			for (uint32 h = 0; h < height * width; ++h)
			{
				uint32 b = sourceBits[0];
				uint32 g = sourceBits[1];
				uint32 r = sourceBits[2];
				uint32 a = sourceBits[3];

				destBits[0] = (uint8)r; //sourceBits[3];
				destBits[1] = (uint8)g; //sourceBits[0];
				destBits[2] = (uint8)b;//sourceBits[1];
				destBits[3] = (uint8)a;

				destBits += 4;
				sourceBits += 4;
			}
		}

		saveTexture->UnlockRect(0);
		image->Save(Format("FBO\\%s.png", relativePathname.c_str()));
		
		SafeRelease(image);
#endif
		


		D3DSafeRelease(renderTargetMainSurface);
		D3DSafeRelease(saveTextureMainSurface);
	}
}

void Texture::Lost()
{
	if (isRenderTarget)
	{
		//Logger::Debug("Releasing lost render target with path: %s", relativePathname.c_str());
		D3DSafeRelease(id);
	}
}

void Texture::Invalidate()
{
	if (isRenderTarget)
	{		
		//Logger::Debug("Restoring lost render target with path: %s size: %d x %d", relativePathname.c_str(), width, height);

		LPDIRECT3DDEVICE9 device = RenderManager::Instance()->GetD3DDevice();

		id = CreateTextureNative(Vector2((float32)width, (float32)height), format, isRenderTarget, 0);
		if (saveTexture)
		{
			RENDER_VERIFY(device->UpdateTexture(saveTexture, id));
			D3DSafeRelease(saveTexture);
		}


	}
}


LPDIRECT3DTEXTURE9 Texture::CreateTextureNative(Vector2 & size, PixelFormat & format, bool isRenderTarget, int32 flags)
{
	LPDIRECT3DDEVICE9 device = RenderManager::Instance()->GetD3DDevice();

	LPDIRECT3DTEXTURE9	texture;
	Rect				rect;
	PixelFormat			colorFormat;
	size.dx = (float32)GetLog2Size((uint32)size.dx);
	size.dy = (float32)GetLog2Size((uint32)size.dy);
	rect.x = 0;
	rect.y = 0;
	rect.dx = size.dx;
	rect.dy = size.dy;

	HRESULT hr;
	D3DFORMAT texFormat = D3DFMT_A8R8G8B8;
	colorFormat = FORMAT_RGBA8888;
	if (format == ECF_R5G6B5)
	{
		texFormat = D3DFMT_R5G6B5;
		colorFormat = FORMAT_RGB565;
	}

	if (format == FORMAT_RGBA8888)
	{
		texFormat = D3DFMT_A8R8G8B8;
		colorFormat = FORMAT_RGBA8888;
	}

	if (format == FORMAT_RGBA4444)
	{
		texFormat = D3DFMT_A4R4G4B4;
		colorFormat = FORMAT_RGBA4444;
	}
	
	bool enableAllMipMapLevels = Texture::isMipmapGenerationEnabled;

	DWORD textureFlags = (isRenderTarget) ? (D3DUSAGE_RENDERTARGET) : (0);

	D3DPOOL pool = D3DPOOL_MANAGED;
	if (isRenderTarget)pool = D3DPOOL_DEFAULT;

	hr = device->CreateTexture((UINT)size.dx, (UINT)size.dy,
		enableAllMipMapLevels ? 0 : 1, 
		textureFlags, texFormat, pool, &texture, 0);

	if (FAILED(hr))
	{
		Logger::Error("[TextureDX9] texture creation failed");
	}
	/*	// Brute force (try to create Texture)
		D3DFORMAT		formatList[4] = {D3DFMT_A1R5G5B5, D3DFMT_A4R4G4B4, D3DFMT_A8R8G8B8, D3DFMT_R5G6B5};
		eColorFormat	cformatList[4] = {ECF_A1R5G5B5, ECF_A4R4G4B4, ECF_A8R8G8B8, ECF_R5G6B5};
		for (uint32 i = 0; i < 4; ++i)
		{
			hr = device->CreateTexture(size.dx, size.dy,
				genMipmaps ? 0 : 1, 
				0, formatList[i], D3DPOOL_MANAGED, &texture, 0);
			if (SUCCEEDED(hr))
			{
				colorFormat = cformatList[i];
				return texture;
			}
		}
		return 0;
	}*/
	format = colorFormat;
	return texture;
}


void Texture::SetAsHardwareCursor(const Vector2 & hotSpot)
{
	if (id)
	{
		LPDIRECT3DSURFACE9 textureMainSurface; 
		HRESULT hr = id->GetSurfaceLevel(0, &textureMainSurface);
		RENDER_VERIFY(hr);
		
		// Check surface format	

		//
		D3DSURFACE_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		RENDER_VERIFY(textureMainSurface->GetDesc(&desc));

		LPDIRECT3DDEVICE9 device = RenderManager::Instance()->GetD3DDevice();
		hr = device->SetCursorProperties((UINT)hotSpot.x, (UINT)hotSpot.y, textureMainSurface);
		RENDER_VERIFY(hr);
		D3DSafeRelease(textureMainSurface);
	}
}

#endif
};



/*#include "TextureDX9.h"
#include "RenderSystemDX9.h"

namespace Log
{
namespace Render
{

// Constuctor
	//! Constuctor
TextureDX9::TextureDX9(RenderSystemDX9 * _device, 
					   LPDIRECT3DTEXTURE9 _texture, 
					   const Math::Size2i & _size, 
					   eColorFormat _format,
					   int32 _flags) : 
ITexture(""), 
device(_device), 
texture(_texture), 
pitch(0), 
size(_size), 
rect(0, 0, _size.dx, _size.dy), 
flags(_flags), 
colorFormat(_format), 
image(0)
{
	deviceDX9 = device->pID3DDevice;

	D3DSURFACE_DESC desc;
	texture->GetLevelDesc(0, &desc);
	size.dx = rect.dx = desc.Width;
	size.dy = rect.dy = desc.Height;
}

//! Constuctor
TextureDX9::TextureDX9(
					   RenderSystemDX9 * _device, 
					   LPDIRECT3DTEXTURE9 _texture, 
					   Image * _image, 
					   eColorFormat _format,
					   int32 _flags) : 
ITexture(_image->GetName()), 
device(_device), 
texture(_texture), 
flags(_flags),
colorFormat(_format),
image(_image),
size(_image->GetSize())
{
	rect.x = rect.y = 0;
	
	D3DSURFACE_DESC desc;
	texture->GetLevelDesc(0, &desc);
	size.dx = rect.dx = desc.Width;
	size.dy = rect.dy = desc.Height;

	deviceDX9 = device->pID3DDevice;
	image->AddReference();
	CopyTexture();
}



// Destructor
TextureDX9::~TextureDX9()
{
	SafeRelease(image);
	SafeRelease(texture);

	Global::Log("[TextureDX9] release successfull.\n");
}

//! Lock texture 
void * TextureDX9::Lock()
{
	if (!texture)
		return 0;

	D3DLOCKED_RECT Rect;
	HRESULT hr = texture->LockRect(0, &Rect, 0, 0);
	if (FAILED(hr))
	{
		Global::Log("[TextureDX9] Could not lock DirectX8 Texture.\n", ELL_ERROR);
		return 0;
	}
	return Rect.pBits;

}
//! Unlock texture 
void TextureDX9::Unlock()
{
	texture->UnlockRect(0);
	
}
	
//! Get image file dimentions
Math::Size2i	& TextureDX9::GetSize() 
{
	return image->GetSize();
}

//! Get created texture dimentions
Math::Size2i	& TextureDX9::GetTextureSize()
{
	return size;
}
//! Get texture rect
//! Used for texture precaching & manipulation with
//! them from a large single texture 
//! For example if you have 100 small lightmaps
//! you can pack them to one large texture and 
//! prevent switching of Textures many times a frame
Math::Rect2i & TextureDX9::GetRect()
{
	return rect;
}

//! Get texture color format
eColorFormat TextureDX9::GetColorFormat()
{
	return colorFormat;
}

//! Get texture Pitch
uint32 TextureDX9::GetPitch()
{
	return pitch;
}
//! Called then texture is dead after switching between Video Modes
void TextureDX9::Restore()
{

}

//! Generate mip-maps for texture
void TextureDX9::GenerateMipMaps()
{
	//
	// Generate mipmaps
	// 
	
	int32 width = image->GetSize().dx;
	int32 height = image->GetSize().dy;

	for (uint32 i = 1; i < texture->GetLevelCount(); ++i)
	{
		D3DLOCKED_RECT prevLevelRect;
		HRESULT hr = texture->LockRect(i - 1, &prevLevelRect, 0, 0);
		if (FAILED(hr))
		{
			Global::Log("[TextureDX9] Could not lock DirectX9 Texture.\n", ELL_ERROR);
			return;
		}

		D3DLOCKED_RECT levelRect;
		hr = texture->LockRect(i, &levelRect, 0, 0);
		if (FAILED(hr))
		{
			Global::Log("[TextureDX9] Could not lock DirectX9 Texture.\n", ELL_ERROR);
			return;
		}

		ColorConverter::ConvertDataDownsize((uint8*)prevLevelRect.pBits, prevLevelRect.Pitch, width, height, colorFormat,(uint8 *) levelRect.pBits, levelRect.Pitch, colorFormat);

		hr = texture->UnlockRect(i);
		if (FAILED(hr))
		{	
			Global::Log("[TextureDX9] Could not unlock DirectX9 Texture.\n", ELL_ERROR);
		}
		hr = texture->UnlockRect(i - 1);
		if (FAILED(hr))
		{	
			Global::Log("[TextureDX9] Could not unlock DirectX9 Texture.\n", ELL_ERROR);
		}

		width >>= 1;
		height >>= 1;
	}

	//! \TODO: check why code not work
	//! texture->GenerateMipSubLevels();
}
	
//! Remove all mip-maps previously generated
void TextureDX9::RemoveMipMaps()
{


}

//! Get texture image
Image * TextureDX9::GetImage()
{
	return image;
}

// PRIVATE TextureDX9 Declarations
uint32 GetLog2Size(uint32 Size)
{
	uint32 DesiredSize = 0x01;

	while(DesiredSize < Size)
		DesiredSize <<= 1;

	//if (DesiredSize > Size && DesiredSize > 64)
	//	DesiredSize >>= 1;

	return DesiredSize;
}


void TextureDX9::CopyTexture()
{
	if (!image)return;

	uint8 * Data = (uint8 *) image->Lock();

	D3DLOCKED_RECT Rect;
	HRESULT hr = texture->LockRect(0, &Rect, 0, 0);
	if (FAILED(hr))
	{
		Global::Log("[TextureDX9] Could not lock DirectX8 Texture.\n", ELL_ERROR);
		image->Unlock();
		return;
	}

	pitch = Rect.Pitch;
	
	ColorConverter::ConvertData(Data,image->GetSize().dx * ColorFormatSize(image->GetColorFormat()),image->GetSize().dx,image->GetSize().dy, image->GetColorFormat(),(uint8 *) Rect.pBits, pitch, colorFormat);
	
	hr = texture->UnlockRect(0);
	if (FAILED(hr))
	{	
		Global::Log("[TextureDX9] Could not unlock DirectX8 Texture.\n", ELL_ERROR);
	}

	image->Unlock();


	GenerateMipMaps();
}



//! Create render targets or shadow containers
//! Or generated textures
// This function must create texture in any case 
// because one of formats must be supported by 
// hardware.
// If they failed -- try to change your video card! 

LPDIRECT3DTEXTURE9 TextureDX9::CreateTextureNative(RenderSystemDX9 * device, Math::Size2i & size, eColorFormat & format,  int32 flags)
{
	LPDIRECT3DTEXTURE9	texture;
	Math::Rect2i		rect;
	eColorFormat		colorFormat;
	size.dx = GetLog2Size(size.dx);
	size.dy = GetLog2Size(size.dy);
	rect.x = 0;
	rect.y = 0;
	rect.dx = size.dx;
	rect.dy = size.dy;

	HRESULT hr;
	D3DFORMAT texFormat = D3DFMT_A1R5G5B5;
	colorFormat = ECF_A1R5G5B5;
	if ((format == ECF_A1R5G5B5)
		||(format == ECF_R5G6B5))
	{
		texFormat = D3DFMT_A1R5G5B5;
		colorFormat = ECF_A1R5G5B5;
	}

	if ((format == ECF_R8G8B8)
		||(format == ECF_A8R8G8B8))
	{
		texFormat = D3DFMT_A8R8G8B8;
		colorFormat = ECF_A8R8G8B8;
	}

	bool genMipmaps = ((flags & ETF_GENERATEMIPMAPS) != 0);


	DWORD textureFlags = (flags & ETF_RENDERTARGET) ? (D3DUSAGE_RENDERTARGET) : (0);


	hr = device->pID3DDevice->CreateTexture(size.dx, size.dy,
		genMipmaps ? 0 : 1, 
		textureFlags, texFormat, D3DPOOL_MANAGED, &texture, 0);

	if (FAILED(hr))
	{
		// Brute force (try to create Texture)
		D3DFORMAT		formatList[4] = {D3DFMT_A1R5G5B5, D3DFMT_A4R4G4B4, D3DFMT_A8R8G8B8, D3DFMT_R5G6B5};
		eColorFormat	cformatList[4] = {ECF_A1R5G5B5, ECF_A4R4G4B4, ECF_A8R8G8B8, ECF_R5G6B5};
		for (uint32 i = 0; i < 4; ++i)
		{
			hr = device->pID3DDevice->CreateTexture(size.dx, size.dy,
				genMipmaps ? 0 : 1, 
				0, formatList[i], D3DPOOL_MANAGED, &texture, 0);
			if (SUCCEEDED(hr))
			{
				colorFormat = cformatList[i];
				return texture;
			}
		}
		return 0;
	}
	format = colorFormat;
	return texture;
}

//! Create render targets or shadow containers
//! Or generated textures
TextureDX9 * TextureDX9::CreateTexture(RenderSystemDX9 * device,const Math::Size2i & inSize, eColorFormat format, int32 flags)
{
	Math::Size2i size = inSize;
	LPDIRECT3DTEXTURE9 texture = TextureDX9::CreateTextureNative(device, size, format, flags);

	if (texture)
	{
		TextureDX9 * tex = new TextureDX9(device, texture, size, format, flags);
		return tex;
	}
	return 0;
}

//! Create standard image textures
TextureDX9 * TextureDX9::CreateTexture(RenderSystemDX9 * device, Image * image, int32 flags)
{
	//image->GetColorFormat()
	//TextureDX9 * tex = TextureDX9::CreateTexture(device, image->GetSize(), flags, image->GetColorFormat());
	Math::Size2i size = image->GetSize();
	eColorFormat format = image->GetColorFormat();
	LPDIRECT3DTEXTURE9 texture = TextureDX9::CreateTextureNative(device, size, format, flags);

	if (texture)
	{
		TextureDX9 * tex = new TextureDX9(device, texture, image, format, flags);
		return tex;
	}
	return 0;
}

void TextureDX9::Bind(uint32 stage)
{
	if (this == device->textureStage[stage])return;

	if (device->textureStage[stage])
	{
		device->textureStage[stage]->Release();
		device->textureStage[stage] = 0;
	}
	
	device->textureStage[stage] = this;
	if (device->textureStage[stage] == 0)return;

	deviceDX9->SetTexture(stage, texture);
	AddReference();
};

};	// end of namespace Render
};	// end of namespace Log

*/