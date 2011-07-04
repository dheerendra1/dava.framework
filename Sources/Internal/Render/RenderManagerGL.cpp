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
#include "Render/RenderBase.h"
#include "Render/RenderManager.h"
#include "Render/Texture.h"
#include "Render/2D/Sprite.h"
#include "Utils/Utils.h"
#include "Core/Core.h"
#include "Render/OGLHelpers.h"
#include "Render/Shader.h"

#ifdef __DAVAENGINE_OPENGL__

namespace DAVA
{
	
#if defined(__DAVAENGINE_WIN32__)

static HDC hDC;
static HGLRC hRC;
static HWND hWnd;
static HINSTANCE hInstance;

bool RenderManager::Create(HINSTANCE _hInstance, HWND _hWnd)
{
	hInstance = _hInstance;
	hWnd = _hWnd;

	hDC = GetDC(hWnd);

	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory( &pfd, sizeof( pfd ) );
	pfd.nSize = sizeof( pfd );
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;
	int iFormat = ChoosePixelFormat( hDC, &pfd );
	SetPixelFormat( hDC, iFormat, &pfd );

	hRC = wglCreateContext(hDC);
	Thread::secondaryContext = wglCreateContext(hDC);
	Thread::currentDC = hDC;

	wglShareLists(Thread::secondaryContext, hRC);
	wglMakeCurrent(hDC, hRC);

	glewInit();
	const GLubyte * extensions = glGetString(GL_EXTENSIONS);
	Logger::Debug("[CoreWin32Platform] gl extensions: %s", (const char*)extensions);

	DetectRenderingCapabilities();
	return true;
}

void RenderManager::Release()
{
	wglMakeCurrent(0, 0);
	wglDeleteContext(hRC);
	ReleaseDC(hWnd, hDC);	
	
	Singleton<RenderManager>::Release();
}

bool RenderManager::ChangeDisplayMode(DisplayMode mode, bool isFullscreen)
{

	return true;
}	
	
#else

bool RenderManager::Create()
{
	DetectRenderingCapabilities();
	return true;
}
	
void RenderManager::Release()
{
	
}

#endif

void RenderManager::DetectRenderingCapabilities()
{
	caps.isHardwareCursorSupported = false;
#if defined(__DAVAENGINE_MACOS__)
	caps.isHardwareCursorSupported = true;
#endif
}

bool RenderManager::IsDeviceLost()
{
	return false;
}

void RenderManager::BeginFrame()
{
    
	RENDER_VERIFY(glViewport(0, 0, frameBufferWidth, frameBufferHeight));
	
	SetRenderOrientation(Core::Instance()->GetScreenOrientation());
	DVASSERT(!currentRenderTarget);
	DVASSERT(!currentRenderEffect);
	DVASSERT(clipStack.empty());
	DVASSERT(renderTargetStack.empty());
	DVASSERT(renderEffectStack.empty());
	Reset();
	isInsideDraw = true;
    ClearUniformMatrices();
}
	
//void RenderManager::SetDrawOffset(const Vector2 &offset)
//{
//	glMatrixMode(GL_PROJECTION);
//	glTranslatef(offset.x, offset.y, 0.0f);
//	glMatrixMode(GL_MODELVIEW);
//}

void RenderManager::PrepareRealMatrix()
{
	/*
		Boroda To Hottych: Не проще ли ставить где-то флаг что были данные изменены? И только в этом случае пересчитывать? 
		Так получается сначала считаем, потом проверки сравнения, вообщем по моему проще сделать один флаг. Что думаешь? 

		Hottych: 
		Код не оптимизировался ваааще. Задача была заставить это работать и она достигнута, 
		а ты все равно все на матрицы перепишешь.
	 */
	realDrawScale.x = viewMappingDrawScale.x * userDrawScale.x;
	realDrawScale.y = viewMappingDrawScale.y * userDrawScale.y;
	
	realDrawOffset.x = viewMappingDrawOffset.x + userDrawOffset.x * viewMappingDrawScale.x;
	realDrawOffset.y = viewMappingDrawOffset.y + userDrawOffset.y * viewMappingDrawScale.y;
	if (realDrawScale != currentDrawScale || realDrawOffset != currentDrawOffset) 
	{
//		Logger::Info("Matrix recalculated: Scale %.4f,    Offset: %.4f, %.4f", realDrawScale.x, realDrawOffset.x, realDrawOffset.y);

		currentDrawScale = realDrawScale;
		currentDrawOffset = realDrawOffset;

//		RENDER_VERIFY(
//					  glLoadIdentity();
//					  glTranslatef(currentDrawOffset.x, currentDrawOffset.y, 0.0f);
//					  glScalef(currentDrawScale.x, currentDrawScale.y, 1.0f);
//					  );
        
        
        Matrix4 glTranslate, glScale;
        glTranslate.glTranslate(currentDrawOffset.x, currentDrawOffset.y, 0.0f);
        glScale.glScale(currentDrawScale.x, currentDrawScale.y, 1.0f);
        
        // todo replace with offset calculations
        SetMatrix(MATRIX_MODELVIEW, glScale * glTranslate);
    }
}
	

void RenderManager::EndFrame()
{
	isInsideDraw = false;
#if defined(__DAVAENGINE_WIN32__)
	::SwapBuffers(hDC);
#endif
	
	RENDER_VERIFY();	// verify at the end of the frame
}
    
    
    
void RenderManager::SetViewport(const Rect & rect)
{
	int32 x = (int32)(rect.x * currentDrawScale.x + currentDrawOffset.x);
	int32 y = (int32)(rect.y * currentDrawScale.y + currentDrawOffset.y);
	int32 width, height;
    width = (int32)(rect.dx * currentDrawScale.x);
    height = (int32)(rect.dy * currentDrawScale.y);    
    
    if ((rect.dx < 0.0f) && (rect.dy < 0.0f))
    {
        glViewport(0, 0, frameBufferWidth, frameBufferHeight);
        return;
    }
    
    switch(renderOrientation)
    {
        case Core::SCREEN_ORIENTATION_PORTRAIT:
        { 
            y = frameBufferHeight - y - height;
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
    glViewport(x, y, width, height);
}


// Viewport management
void RenderManager::SetRenderOrientation(int32 orientation)
{
	renderOrientation = orientation;
	
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//#if defined(__DAVAENGINE_IPHONE__)
//	RENDER_VERIFY(glOrthof(0.0f, frameBufferWidth, frameBufferHeight, 0.0f, -1.0f, 1.0f));
//#else // for NON ES platforms
//	RENDER_VERIFY(glOrtho(0.0f, frameBufferWidth, frameBufferHeight, 0.0f, -1.0f, 1.0f));
//#endif
    
    Matrix4 orthoMatrix; 
    Matrix4 glTranslate;
    Matrix4 glRotate;

    orthoMatrix.glOrtho(0.0f, frameBufferWidth, frameBufferHeight, 0.0f, -1.0f, 1.0f);
	
    switch (orientation) 
	{
		case Core::SCREEN_ORIENTATION_PORTRAIT:
		case Core::SCREEN_ORIENTATION_TEXTURE:
			retScreenWidth = frameBufferWidth;
			retScreenHeight = frameBufferHeight;
			break;
		case Core::SCREEN_ORIENTATION_LANDSCAPE_LEFT:
            
            //mark glTranslatef(0.0f, (float32)frameBufferHeight, 0.0f);
			//mark glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
            
            glTranslate.glTranslate(0.0f, (float32)frameBufferHeight, 0.0f);
            glRotate.glRotate(-90.0f, 0.0f, 0.0f, 1.0f);
            
            orthoMatrix = glRotate * glTranslate * orthoMatrix;
            
			retScreenWidth = frameBufferHeight;
			retScreenHeight = frameBufferWidth;
            break;
		case Core::SCREEN_ORIENTATION_LANDSCAPE_RIGHT:
			//mark glTranslatef((float32)frameBufferWidth, 0.0f, 0.0f);
			//mark glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
            
            glTranslate.glTranslate((float32)frameBufferWidth, 0.0f, 0.0f);
            glRotate.glRotate(90.0f, 0.0f, 0.0f, 1.0f);
            
            orthoMatrix = glRotate * glTranslate * orthoMatrix;

			retScreenWidth = frameBufferHeight;
			retScreenHeight = frameBufferWidth;
			break;
	}
    
    SetMatrix(MATRIX_PROJECTION, orthoMatrix);

//	if (orientation != Core::SCREEN_ORIENTATION_TEXTURE) 
//	{
//		glTranslatef(Core::Instance()->GetPhysicalDrawOffset().x, Core::Instance()->GetPhysicalDrawOffset().y, 0.0f);
//	}
	
    //glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	
    RenderManager::Instance()->SetMatrix(MATRIX_MODELVIEW, Matrix4::IDENTITY);
    
	RENDER_VERIFY();

	IdentityTotalMatrix();
	SetVirtualViewScale();
	SetVirtualViewOffset();

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

static GLint BLEND_MODE_MAP[BLEND_MODE_COUNT] = 
{
		0,	// not a valid blend mode
		GL_ZERO,
		GL_ONE,
		GL_DST_COLOR,
		GL_ONE_MINUS_DST_COLOR,
		GL_SRC_ALPHA,
		GL_ONE_MINUS_SRC_ALPHA,
		GL_DST_ALPHA,
		GL_ONE_MINUS_DST_ALPHA,
		GL_SRC_ALPHA_SATURATE,
};


void RenderManager::EnableBlending(bool isEnabled)
{
	if(isEnabled != oldBlendingEnabled)
	{
		if(isEnabled)
		{
			RENDER_VERIFY(glEnable(GL_BLEND));
		}
		else
		{
			RENDER_VERIFY(glDisable(GL_BLEND));
		}
		oldBlendingEnabled = isEnabled;
	}
}
void RenderManager::EnableVertexArray(bool isEnabled)
{
	if(isEnabled != oldVertexArrayEnabled)
	{
		if(isEnabled)
		{
			RENDER_VERIFY(glEnableClientState(GL_VERTEX_ARRAY));
		}
		else
		{
			RENDER_VERIFY(glDisableClientState(GL_VERTEX_ARRAY));
		}
		oldVertexArrayEnabled = isEnabled;
	}
}
void RenderManager::EnableTextureCoordArray(bool isEnabled)
{
	if(isEnabled != oldTextureCoordArrayEnabled)
	{
		if(isEnabled)
		{
			RENDER_VERIFY(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
		}
		else
		{
			RENDER_VERIFY(glDisableClientState(GL_TEXTURE_COORD_ARRAY));
		}
		oldTextureCoordArrayEnabled = isEnabled;
	}
}

void RenderManager::EnableColorArray(bool isEnabled)
{
	if(isEnabled != oldColorArrayEnabled)
	{
		if(isEnabled)
		{
			RENDER_VERIFY(glEnableClientState(GL_COLOR_ARRAY));
		}
		else
		{
			RENDER_VERIFY(glDisableClientState(GL_COLOR_ARRAY));
		}
		oldColorArrayEnabled = isEnabled;
	}
}
	
void RenderManager::FlushState()
{
	if(newSFactor != oldSFactor || newDFactor != oldDFactor)
	{
		RENDER_VERIFY(glBlendFunc(BLEND_MODE_MAP[newSFactor], BLEND_MODE_MAP[newDFactor]));
		oldSFactor = newSFactor;
		oldDFactor = newDFactor;
	}
	if(oldR != newR || oldG != newG || oldB != newB || oldA != newA)
	{
		if(!currentRenderEffect)
		{
			RENDER_VERIFY(glColor4f(newR * newA, newG * newA, newB * newA, newA));
		}
		else
		{
			currentRenderEffect->SetColor(newR, newG, newB, newA);
		}
		oldR = newR;
		oldG = newG;
		oldB = newB;
		oldA = newA;
	}
	if(newTextureEnabled != oldTextureEnabled)
	{
		if(newTextureEnabled)
		{
			RENDER_VERIFY(glEnable(GL_TEXTURE_2D));
		}
		else
		{
			RENDER_VERIFY(glDisable(GL_TEXTURE_2D));
		}
		oldTextureEnabled = newTextureEnabled;
	}
	
	PrepareRealMatrix();
}

GLint types[TYPE_COUNT] = {GL_FLOAT};

void RenderManager::SetTexCoordPointer(int size, eVertexDataType _typeIndex, int stride, const void *pointer)
{
	GLint type = types[_typeIndex];
	if(!currentRenderEffect)
	{
		RENDER_VERIFY(glTexCoordPointer(size, type, stride, pointer));
	}
	else
	{
		currentRenderEffect->SetTexCoordPointer(size, type, stride, pointer);
	}
}

void RenderManager::SetVertexPointer(int size, eVertexDataType _typeIndex, int stride, const void *pointer)
{
	GLint type = types[_typeIndex];
	if(!currentRenderEffect)
	{
		RENDER_VERIFY(glVertexPointer(size, type, stride, pointer));
	}
	else
	{
		currentRenderEffect->SetVertexPointer(size, type, stride, pointer);
	}
}

void RenderManager::SetNormalPointer(eVertexDataType _typeIndex, int stride, const void *pointer)
{
	GLint type = types[_typeIndex];
	if(!currentRenderEffect)
	{
		RENDER_VERIFY(glNormalPointer(type, stride, pointer));
	}
	else
	{
		// TODO
		//currentRenderEffect->SetVertexPointer(size, _typeIndex, stride, pointer);
	}
}

void RenderManager::SetColorPointer(int size, eVertexDataType _typeIndex, int stride, const void *pointer)
{
	GLint type = types[_typeIndex];
	if(!currentRenderEffect)
	{
		RENDER_VERIFY(glColorPointer(size, type, stride, pointer));
	}
	else
	{
		// TODO
		//currentRenderEffect->SetVertexPointer(size, _typeIndex, stride, pointer);
	}
}

void RenderManager::DrawArrays(ePrimitiveType type, int32 first, int32 count)
{
	const int32 types[PRIMITIVETYPE_COUNT] = 
	{
		GL_POINTS,			// 		PRIMITIVETYPE_POINTLIST = 0,
		GL_LINES,			// 		PRIMITIVETYPE_LINELIST,
		GL_LINE_STRIP,		// 		PRIMITIVETYPE_LINESTRIP,
		GL_TRIANGLES,		// 		PRIMITIVETYPE_TRIANGLELIST,
		GL_TRIANGLE_STRIP,	// 		PRIMITIVETYPE_TRIANGLESTRIP,
		GL_TRIANGLE_FAN,	// 		PRIMITIVETYPE_TRIANGLEFAN,
	};

	GLuint mode = types[type];

	if(debugEnabled)
	{
		Logger::Debug("Draw arrays texture: id %d", currentTexture->id);
	}

	if(!currentRenderEffect)
	{
		RENDER_VERIFY(glDrawArrays(mode, first, count));
	}
	else
	{
		currentRenderEffect->DrawArrays(mode, first, count);
	}
}

void RenderManager::DrawElements(ePrimitiveType type, int32 count, eIndexFormat indexFormat, void * indices)
{
	const int32 types[PRIMITIVETYPE_COUNT] = 
	{
		GL_POINTS,			// 		PRIMITIVETYPE_POINTLIST = 0,
		GL_LINES,			// 		PRIMITIVETYPE_LINELIST,
		GL_LINE_STRIP,		// 		PRIMITIVETYPE_LINESTRIP,
		GL_TRIANGLES,		// 		PRIMITIVETYPE_TRIANGLELIST,
		GL_TRIANGLE_STRIP,	// 		PRIMITIVETYPE_TRIANGLESTRIP,
		GL_TRIANGLE_FAN,	// 		PRIMITIVETYPE_TRIANGLEFAN,
	};
	
	GLuint mode = types[type];
	
	if(debugEnabled)
	{
		Logger::Debug("Draw arrays texture: id %d", currentTexture->id);
	}
#if defined(__DAVAENGINE_IPHONE__)
#define GL_UNSIGNED_INT 0
#endif // __DAVAENGINE_IPHONE__

	const int32 indexTypes[2] = 
	{
		GL_UNSIGNED_SHORT, 
		GL_UNSIGNED_INT,
	};
	
	RENDER_VERIFY(glDrawElements(mode, count, indexTypes[indexFormat], indices));
}

void RenderManager::ClearWithColor(float32 r, float32 g, float32 b, float32 a)
{
	RENDER_VERIFY(glClearColor(r, g, b, a));
	RENDER_VERIFY(glClear(GL_COLOR_BUFFER_BIT));
}

void RenderManager::ClearDepthBuffer(float32 depth)
{
#if defined(__DAVAENGINE_IPHONE__)
    RENDER_VERIFY(glClearDepthf(depth));
#elif defined(__DAVAENGINE_MACOS__)
    RENDER_VERIFY(glClearDepth(depth));
#endif 
    RENDER_VERIFY(glClear(GL_DEPTH_BUFFER_BIT));
}   

void RenderManager::SetHWClip(const Rect &rect)
{
	PrepareRealMatrix();
	currentClip = rect;
	if(rect.dx < 0 || rect.dy < 0)
	{
		RENDER_VERIFY(glDisable(GL_SCISSOR_TEST));
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
			y = frameBufferHeight/* * Core::GetVirtualToPhysicalFactor()*/ - y - height;
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
	RENDER_VERIFY(glEnable(GL_SCISSOR_TEST));
	RENDER_VERIFY(glScissor(x, y, width, height));
}


void RenderManager::SetHWRenderTarget(Sprite *renderTarget)
{
	if (renderTarget == NULL)
	{
#if defined(__DAVAENGINE_IPHONE__)
		RENDER_VERIFY(glBindFramebufferOES(GL_FRAMEBUFFER_OES, fboViewFramebuffer));
#else
		RENDER_VERIFY(glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboViewFramebuffer));
#endif
		RENDER_VERIFY(glViewport(0, 0, frameBufferWidth, frameBufferHeight));

		SetRenderOrientation(Core::Instance()->GetScreenOrientation());
	}
	else
	{
		renderOrientation = Core::SCREEN_ORIENTATION_TEXTURE;
#if defined(__DAVAENGINE_IPHONE__)
		RENDER_VERIFY(glBindFramebufferOES(GL_FRAMEBUFFER_OES, renderTarget->GetTexture()->fboID));
#else
		RENDER_VERIFY(glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, renderTarget->GetTexture()->fboID));
#endif

		RENDER_VERIFY(glViewport(0, 0, renderTarget->GetTexture()->width, renderTarget->GetTexture()->height));

//		RENDER_VERIFY(glMatrixMode(GL_PROJECTION));
//		RENDER_VERIFY(glLoadIdentity());
//#if defined(__DAVAENGINE_IPHONE__)
//		RENDER_VERIFY(glOrthof(0.0f, renderTarget->GetTexture()->width, 0.0f, renderTarget->GetTexture()->height, -1.0f, 1.0f));
//#else 
//		RENDER_VERIFY(glOrtho(0.0f, renderTarget->GetTexture()->width, 0.0f, renderTarget->GetTexture()->height, -1.0f, 1.0f));
//#endif

        Matrix4 orthoMatrix; 
        orthoMatrix.glOrtho(0.0f, renderTarget->GetTexture()->width, 0.0f, renderTarget->GetTexture()->height, -1.0f, 1.0f);
        SetMatrix(MATRIX_PROJECTION, orthoMatrix);
        
		//RENDER_VERIFY(glMatrixMode(GL_MODELVIEW));
		//RENDER_VERIFY(glLoadIdentity());
        
        SetMatrix(MATRIX_MODELVIEW, Matrix4::IDENTITY);
        
		IdentityTotalMatrix();
		viewMappingDrawScale.x = renderTarget->GetResourceToPhysicalFactor();
		viewMappingDrawScale.y = renderTarget->GetResourceToPhysicalFactor();
//		Logger::Info("Sets with render target: Scale %.4f,    Offset: %.4f, %.4f", viewMappingDrawScale.x, viewMappingDrawOffset.x, viewMappingDrawOffset.y);
		RemoveClip();
	}
	
	currentRenderTarget = renderTarget;
}

    
void RenderManager::SetMatrix(eMatrixType type, const Matrix4 & matrix)
{
    GLint matrixMode[2] = {GL_MODELVIEW, GL_PROJECTION};
    glMatrixMode(matrixMode[type]);
    glLoadMatrixf(matrix.data);
    matrices[type] = matrix;
}
    
    
void RenderManager::InitGL20()
{
    colorOnly = 0;
    colorWithTexture = 0;
    
    
    colorOnly = new Shader();
    colorOnly->LoadFromYaml("~res:/Shaders/Default/fixed_func_color_only.shader");
    colorWithTexture = new Shader();
    colorWithTexture->LoadFromYaml("~res:/Shaders/Default/fixed_func_texture.shader");
        
    
}

void RenderManager::ReleaseGL20()
{
    SafeRelease(colorOnly);
    SafeRelease(colorWithTexture);
}





};

#endif // __DAVAENGINE_OPENGL__
