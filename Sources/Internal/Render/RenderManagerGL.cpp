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
    stats.Clear();
	RENDER_VERIFY(glViewport(0, 0, frameBufferWidth, frameBufferHeight));
	
	SetRenderOrientation(Core::Instance()->GetScreenOrientation());
	DVASSERT(!currentRenderTarget);
	//DVASSERT(!currentRenderEffect);
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
    if (mappingMatrixChanged)
    {
        mappingMatrixChanged = false;
        Vector2 realDrawScale(viewMappingDrawScale.x * userDrawScale.x, viewMappingDrawScale.y * userDrawScale.y);
        Vector2 realDrawOffset(viewMappingDrawOffset.x + userDrawOffset.x * viewMappingDrawScale.x, viewMappingDrawOffset.y + userDrawOffset.y * viewMappingDrawScale.y);
	
	if (realDrawScale != currentDrawScale || realDrawOffset != currentDrawOffset) 
	{

		currentDrawScale = realDrawScale;
		currentDrawOffset = realDrawOffset;

        
        Matrix4 glTranslate, glScale;
        glTranslate.glTranslate(currentDrawOffset.x, currentDrawOffset.y, 0.0f);
        glScale.glScale(currentDrawScale.x, currentDrawScale.y, 1.0f);
        
        glTranslate = glScale * glTranslate;
        SetMatrix(MATRIX_MODELVIEW, glTranslate);
    }
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
    PrepareRealMatrix();
    
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
	

    IdentityModelMatrix();
    
	RENDER_VERIFY();

	IdentityMappingMatrix();
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
    
void RenderManager::EnableDepthTest(bool isEnabled)
{
	if(isEnabled != depthTestEnabled)
	{
		if(isEnabled)
		{
            RENDER_VERIFY(glEnable(GL_DEPTH_TEST));
		}
		else
		{
            RENDER_VERIFY(glDisable(GL_DEPTH_TEST));
		}
		depthTestEnabled = isEnabled;
	}
}

void RenderManager::EnableDepthWrite(bool isEnabled)
{
	if(isEnabled != depthWriteEnabled)
	{
		if(isEnabled)
		{
            RENDER_VERIFY(glDepthMask(GL_TRUE));
		}
		else
		{
            RENDER_VERIFY(glDepthMask(GL_TRUE));
		}
		depthWriteEnabled = isEnabled;
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
	if(oldColor != newColor)
	{
        if (renderer != Core::RENDERER_OPENGL_ES_2_0)
            RENDER_VERIFY(glColor4f(newColor.r * newColor.a, newColor.g * newColor.a, newColor.b * newColor.a, newColor.a));
		oldColor = newColor;
	}   
	if(newTextureEnabled != oldTextureEnabled)
	{
        if (GetRenderer() != Core::RENDERER_OPENGL_ES_2_0)
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
	}
    
    if (cullingEnabled != oldCullingEnabled)
    {
        if (cullingEnabled)
        {
            RENDER_VERIFY(glEnable(GL_CULL_FACE));
            
            if (cullFace != oldCullFace)
            {
                RENDER_VERIFY(glCullFace(CULL_FACE_MAP[cullFace]));
            }
            oldCullFace = cullFace;
        }else
        {
            RENDER_VERIFY(glDisable(GL_CULL_FACE));
        }
        oldCullingEnabled = cullingEnabled;
    }
    
    if ((renderer == Core::RENDERER_OPENGL) || (renderer == Core::RENDERER_OPENGL_ES_1_0))
        if (alphaTestEnabled != oldAlphaTestEnabled)
        {
            if (alphaTestEnabled)
            {
                RENDER_VERIFY(glEnable(GL_ALPHA_TEST));
            
                // if alpha test enabled set alpha func values
                if ((alphaFunc != oldAlphaFunc) || (alphaTestCmpValue != oldAlphaTestCmpValue))
                {
                    RENDER_VERIFY(glAlphaFunc(ALPHA_TEST_MODE_MAP[alphaFunc], alphaTestCmpValue) );
                    oldAlphaFunc = alphaFunc;
                    oldAlphaTestCmpValue = alphaTestCmpValue;
                }
            }else
                RENDER_VERIFY(glDisable(GL_ALPHA_TEST));
        
            oldAlphaTestEnabled = alphaTestEnabled;
        }
	
	PrepareRealMatrix();
    
    AttachRenderData(shader);
    if (shader)
    {
        shader->Bind();
    }else
    {
        Shader::Unbind();
    }
}

void RenderManager::SetTexCoordPointer(int size, eVertexDataType _typeIndex, int stride, const void *pointer)
{
	GLint type = VERTEX_DATA_TYPE_TO_GL[_typeIndex];
    RENDER_VERIFY(glTexCoordPointer(size, type, stride, pointer));
}

void RenderManager::SetVertexPointer(int size, eVertexDataType _typeIndex, int stride, const void *pointer)
{
	GLint type = VERTEX_DATA_TYPE_TO_GL[_typeIndex];
    RENDER_VERIFY(glVertexPointer(size, type, stride, pointer));
}

void RenderManager::SetNormalPointer(eVertexDataType _typeIndex, int stride, const void *pointer)
{
	GLint type = VERTEX_DATA_TYPE_TO_GL[_typeIndex];
    RENDER_VERIFY(glNormalPointer(type, stride, pointer));
}

void RenderManager::SetColorPointer(int size, eVertexDataType _typeIndex, int stride, const void *pointer)
{
	GLint type = VERTEX_DATA_TYPE_TO_GL[_typeIndex];
    RENDER_VERIFY(glColorPointer(size, type, stride, pointer));
}

void RenderManager::HWDrawArrays(ePrimitiveType type, int32 first, int32 count)
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
		Logger::Debug("Draw arrays texture: id %d", currentTexture[0]->id);
	}

    RENDER_VERIFY(glDrawArrays(mode, first, count));
    stats.drawArraysCalls++;
    switch(type)
    {
        case PRIMITIVETYPE_POINTLIST: 
            stats.primitiveCount[type] += count;
            break;
        case PRIMITIVETYPE_LINELIST:
            stats.primitiveCount[type] += count / 2;
            break;
        case PRIMITIVETYPE_LINESTRIP:
            stats.primitiveCount[type] += count - 1;
            break;
        case PRIMITIVETYPE_TRIANGLELIST:
            stats.primitiveCount[type] += count / 3;
            break;
        case PRIMITIVETYPE_TRIANGLEFAN:
        case PRIMITIVETYPE_TRIANGLESTRIP:
            stats.primitiveCount[type] += count - 2;
            break;
        default:
            break;
    };
}
    
void RenderManager::HWDrawElements(ePrimitiveType type, int32 count, eIndexFormat indexFormat, void * indices)
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
		Logger::Debug("Draw arrays texture: id %d", currentTexture[0]->id);
	}
#if defined(__DAVAENGINE_IPHONE__)
#if not defined(GL_UNSIGNED_INT)
#define GL_UNSIGNED_INT 0
#endif //not defined(GL_UNSIGNED_INT)
#endif // __DAVAENGINE_IPHONE__

	const int32 indexTypes[2] = 
	{
		GL_UNSIGNED_SHORT, 
		GL_UNSIGNED_INT,
	};
	
	RENDER_VERIFY(glDrawElements(mode, count, indexTypes[indexFormat], indices));
    stats.drawElementsCalls++;
    switch(type)
    {
        case PRIMITIVETYPE_POINTLIST: 
            stats.primitiveCount[type] += count;
            break;
        case PRIMITIVETYPE_LINELIST:
            stats.primitiveCount[type] += count / 2;
            break;
        case PRIMITIVETYPE_LINESTRIP:
            stats.primitiveCount[type] += count - 1;
            break;
        case PRIMITIVETYPE_TRIANGLELIST:
            stats.primitiveCount[type] += count / 3;
            break;
        case PRIMITIVETYPE_TRIANGLEFAN:
        case PRIMITIVETYPE_TRIANGLESTRIP:
            stats.primitiveCount[type] += count - 2;
            break;
        default:
            break;
    };
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
        
        IdentityModelMatrix();
		IdentityMappingMatrix();
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
    matrices[type] = matrix;
    uniformMatrixFlags[UNIFORM_MATRIX_MODELVIEWPROJECTION] = 0; // require update
    
    if (renderer != Core::RENDERER_OPENGL_ES_2_0)
    {
        RENDER_VERIFY(glMatrixMode(matrixMode[type]));
        RENDER_VERIFY(glLoadMatrixf(matrix.data));
    }
}
    
    
void RenderManager::AttachRenderData(Shader * shader)
{
    if (!currentRenderData)return;

    RenderManager::Instance()->LockNonMain();
    if (!shader)
    {
        // TODO: should be moved to RenderManagerGL
#if defined(__DAVAENGINE_OPENGL__)
#if defined(__DAVAENGINE_OPENGL_ARB_VBO__)
        RENDER_VERIFY(glBindBufferARB(GL_ARRAY_BUFFER_ARB, currentRenderData->vboBuffer));
#else
        RENDER_VERIFY(glBindBuffer(GL_ARRAY_BUFFER, currentRenderData->vboBuffer));
#endif
#elif defined(__DAVAENGINE_DIRECTX9__)
        DVASSERT(currentRenderData->vboBuffer == 0);
#endif
        if (enabledAttribCount != 0)
        {
            for (int32 p = 0; p < enabledAttribCount; ++p)
            {
                glDisableVertexAttribArray(p);
            }
            enabledAttribCount = 0;
            pointerArraysRendererState = 0;
        }
        
        pointerArraysCurrentState = 0;
        int32 size = (int32)currentRenderData->streamArray.size();
        for (int32 k = 0; k < size; ++k)
        {
            RenderDataStream * stream = currentRenderData->streamArray[k];
            switch(stream->formatMark)
            {
                case EVF_VERTEX:
                    SetVertexPointer(stream->size, stream->type, stream->stride, stream->pointer);
                    pointerArraysCurrentState |= EVF_VERTEX;
                    break;
                case EVF_TEXCOORD0:
                    SetTexCoordPointer(stream->size, stream->type, stream->stride, stream->pointer);
                    pointerArraysCurrentState |= EVF_TEXCOORD0;
                    break;
                default:
                    break;
            };
        };
        
        uint32 difference = pointerArraysCurrentState ^ pointerArraysRendererState;
        
        if (difference & EVF_VERTEX)
        {
            EnableVertexArray(pointerArraysCurrentState & EVF_VERTEX);
        }
        if (difference & EVF_TEXCOORD0)
        {
            EnableTextureCoordArray(pointerArraysCurrentState & EVF_TEXCOORD0);
        }
        pointerArraysRendererState = pointerArraysCurrentState;
        
    }
    else
    {
        if (GetRenderer() == Core::RENDERER_OPENGL)
        {
            if (oldVertexArrayEnabled)
        {
                EnableVertexArray(false);
                pointerArraysRendererState = 0;
            }
            if (oldTextureCoordArrayEnabled)
            {
                EnableTextureCoordArray(false);
                pointerArraysRendererState = 0;
        }
    }

        int32 currentEnabledAttribCount = 0;
        //glDisableVertexAttribArray(0);
        //glDisableVertexAttribArray(1);
        pointerArraysCurrentState = 0;
        
        //if (currentRenderData->vboBuffer != 0)
        //{
#if defined(__DAVAENGINE_OPENGL__)
#if defined(__DAVAENGINE_OPENGL_ARB_VBO__)
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, currentRenderData->vboBuffer);
#else
        glBindBuffer(GL_ARRAY_BUFFER, currentRenderData->vboBuffer);
#endif
#endif 
        //}
        
        int32 size = (int32)currentRenderData->streamArray.size();
        for (int32 k = 0; k < size; ++k)
        {
            RenderDataStream * stream = currentRenderData->streamArray[k];
            GLboolean normalized = GL_FALSE;
            
            int32 attribIndex = shader->GetAttributeIndex(stream->formatMark);
            if (attribIndex != -1)
            {
                glVertexAttribPointer(attribIndex, stream->size, VERTEX_DATA_TYPE_TO_GL[stream->type], normalized, stream->stride, stream->pointer);
                
                if (attribIndex >= enabledAttribCount)  // enable only if it was not enabled on previous step
                {
                    glEnableVertexAttribArray(attribIndex);
                }
                if (attribIndex + 1 > currentEnabledAttribCount)
                    currentEnabledAttribCount = attribIndex + 1;    // count of enabled attributes
                
                pointerArraysCurrentState |= stream->formatMark;
            }
        };
        
        for (int32 p = currentEnabledAttribCount; p < enabledAttribCount; ++p)
        {
            glDisableVertexAttribArray(p);
        }
        enabledAttribCount = currentEnabledAttribCount;
        //        uint32 difference = pointerArraysCurrentState ^ pointerArraysRendererState;
        //        
        //        if (!(difference & EVF_VERTEX))
        //        {
        //            int32 attribIndex = shader->GetAttributeIndex(EVF_VERTEX);
        //            RENDER_VERIFY(glDisableVertexAttribArray(attribIndex));
        //        }
        //        if (!(difference & EVF_TEXCOORD0))
        //        {
        //            int32 attribIndex = shader->GetAttributeIndex(EVF_TEXCOORD0);
        //            RENDER_VERIFY(glDisableVertexAttribArray(attribIndex));
        //        }
        pointerArraysRendererState = pointerArraysCurrentState;
    }
    //    for (uint32 formatFlag = EVF_LOWER_BIT; formatFlag <= EVF_HIGHER_BIT; formatFlag >>= 1)
    //    {
    //        if (formatFlag & EVF_VERTEX)
    //        {
    //            
    //        }
    //        if (formatFlag & EVF_TEXCOORD0)
    //        {
    //            
    //        }
    //    }
    //    pointerArraysRendererState = pointerArraysCurrentState;
    RenderManager::Instance()->UnlockNonMain();
}


//void RenderManager::InitGL20()
//{
//    colorOnly = 0;
//    colorWithTexture = 0;
//    
//    
//    colorOnly = new Shader();
//    colorOnly->LoadFromYaml("~res:/Shaders/Default/fixed_func_color_only.shader");
//    colorWithTexture = new Shader();
//    colorWithTexture->LoadFromYaml("~res:/Shaders/Default/fixed_func_texture.shader");
//        
//    
//}
//
//void RenderManager::ReleaseGL20()
//{
//    SafeRelease(colorOnly);
//    SafeRelease(colorWithTexture);
//}
//




};

#endif // __DAVAENGINE_OPENGL__
