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
#include "Core/Core.h"
#include "Render/Shader.h"
#include "Render/RenderDataObject.h"


#include "Render/Effects/ColorOnlyEffect.h"
#include "Render/Effects/TextureMulColorEffect.h"
#include "Render/Effects/TextureMulColorAlphaTestEffect.h"

namespace DAVA
{
    
RenderEffect * RenderManager::FLAT_COLOR = 0;
RenderEffect * RenderManager::TEXTURE_MUL_FLAT_COLOR = 0;
RenderEffect * RenderManager::TEXTURE_MUL_FLAT_COLOR_ALPHA_TEST = 0;

    
RenderManager::RenderManager(Core::eRenderer _renderer)
    : currentState(_renderer)
    , hardwareState(_renderer)
{
	Logger::Debug("[RenderManager] created");
    renderer = _renderer;

//  RENDERSTATE
//	oldColor = Color::Clear();
//    newColor = Color::Clear();
//
//	oldSFactor = BLEND_NONE;
//	oldDFactor = BLEND_NONE;
//	newSFactor = BLEND_NONE;
//	newDFactor = BLEND_NONE;

//    for (uint32 idx = 0; idx < MAX_TEXTURE_LEVELS; ++idx)
//        currentTexture[idx] = 0;

// RENDERSTATE
//    newTextureEnabled = 0;
//	oldTextureEnabled = 0;
//	oldVertexArrayEnabled = 0;
//	oldTextureCoordArrayEnabled = 0;
//	oldColorArrayEnabled = 0;
//	oldBlendingEnabled = 0;
//    depthWriteEnabled = 0;
//    depthTestEnabled = 0;
    
	renderOrientation = 0;
	currentRenderTarget = 0;
	
	currentRenderEffect = 0;

	frameBufferWidth = 0;
	frameBufferHeight = 0;
	retScreenWidth = 0;
	retScreenHeight = 0;

	fps = 60;

	lockCount = 0;
	debugEnabled = false;
	fboViewRenderbuffer = 0;
	fboViewFramebuffer = 0;
	
	userDrawOffset = Vector2(0, 0);
	userDrawScale = Vector2(1, 1);

	viewMappingDrawOffset = Vector2(0, 0);
	viewMappingDrawScale = Vector2(1, 1);

	currentDrawOffset = Vector2(0, 0);
	currentDrawScale = Vector2(1, 1);
    mappingMatrixChanged = true;
	
	isInsideDraw = false;

// RENDERSTATE
//    oldAlphaTestEnabled = alphaTestEnabled = false;
//    oldAlphaTestCmpValue = alphaTestCmpValue = 0.0f;
//    oldAlphaFunc = CMP_ALWAYS;
//    
//    cullingEnabled = oldCullingEnabled = false;
//    cullFace = oldCullFace = CULL_BACK;

#if defined(__DAVAENGINE_DIRECTX9__)
	depthStencilSurface = 0;
	backBufferSurface = 0;
#endif
	cursor = 0;
    currentRenderData = 0;
    pointerArraysCurrentState = 0;
    pointerArraysRendererState = 0;
    enabledAttribCount = 0;
    
    statsFrameCountToShowDebug = 0;
    frameToShowDebugStats = -1;
    
    FLAT_COLOR = 0;
    TEXTURE_MUL_FLAT_COLOR = 0;
    TEXTURE_MUL_FLAT_COLOR_ALPHA_TEST = 0;
}
	
RenderManager::~RenderManager()
{
    SafeRelease(currentRenderData);
    SafeRelease(FLAT_COLOR);
    SafeRelease(TEXTURE_MUL_FLAT_COLOR);
    SafeRelease(TEXTURE_MUL_FLAT_COLOR_ALPHA_TEST);
	SafeRelease(cursor);
	Logger::Debug("[RenderManager] released");
}

void RenderManager::SetDebug(bool isDebugEnabled)
{
	debugEnabled = isDebugEnabled;
}
	
bool RenderManager::IsInsideDraw()
{
	return isInsideDraw;
}

void RenderManager::Init(int32 _frameBufferWidth, int32 _frameBufferHeight)
{
    if (!FLAT_COLOR)
        FLAT_COLOR = ColorOnlyEffect::Create(renderer);
    if (!TEXTURE_MUL_FLAT_COLOR) 
        TEXTURE_MUL_FLAT_COLOR= TextureMulColorEffect::Create(renderer);
    if (!TEXTURE_MUL_FLAT_COLOR_ALPHA_TEST)
        TEXTURE_MUL_FLAT_COLOR_ALPHA_TEST = TextureMulColorAlphaTestEffect::Create(renderer);
    
    currentState.Reset(true);
    
	frameBufferWidth = _frameBufferWidth;
	frameBufferHeight = _frameBufferHeight;
#if defined (__DAVAENGINE_OPENGL__)
    const char * extensions = (const char*)glGetString(GL_EXTENSIONS);
	Logger::Debug("[RenderManager::Init] orientation: %d x %d extensions: %s", frameBufferWidth, frameBufferHeight, extensions);
#else 
	Logger::Debug("[RenderManager::Init] orientation: %d x %d ", frameBufferWidth, frameBufferHeight);
#endif 
    // TODO: Rethink of initialization concepts because they changed
    pointerArraysRendererState = pointerArraysCurrentState = 0;
}

void RenderManager::Reset()
{
	ResetColor();

	currentRenderTarget = NULL;
	currentRenderEffect = NULL;
	currentClip.x = 0;
	currentClip.y = 0;
	currentClip.dx = -1;
	currentClip.dy = -1;
	
//	for (uint32 idx = 0; idx < MAX_TEXTURE_LEVELS; ++idx)
//        currentTexture[idx] = 0;

	userDrawOffset = Vector2(0, 0);
	userDrawScale = Vector2(1, 1);
	
	currentDrawOffset = Vector2(0, 0);
	currentDrawScale = Vector2(1, 1);
    mappingMatrixChanged = true;
	//currentState.Reset(false);
//	glLoadIdentity();
}

int32 RenderManager::GetRenderOrientation()
{
	return renderOrientation;
}
	
int32 RenderManager::GetScreenWidth()
{
	return retScreenWidth;	
}
int32 RenderManager::GetScreenHeight()
{
	return retScreenHeight;
}

void RenderManager::SetColor(float32 r, float32 g, float32 b, float32 a)
{
    currentState.SetColor(r, g, b, a);
}
	
void RenderManager::SetColor(const Color & _color)
{
    currentState.SetColor(_color);
}
	
float32 RenderManager::GetColorR() const
{
	return currentState.color.r;
}
	
float32 RenderManager::GetColorG() const
{
	return currentState.color.g;
}
	
float32 RenderManager::GetColorB() const
{
	return currentState.color.b;
}
	
float32 RenderManager::GetColorA() const
{
	return currentState.color.a;
}
    
const Color & RenderManager::GetColor() const
{
    return currentState.color;
}

void RenderManager::ResetColor()
{
	currentState.color.r = currentState.color.g = currentState.color.b = currentState.color.a = 1.0f;
}
	
	
void RenderManager::SetTexture(Texture *texture, uint32 textureLevel)
{
    currentState.SetTexture(texture, textureLevel);
/*  DVASSERT(textureLevel < MAX_TEXTURE_LEVELS);
	if(texture != currentTexture[textureLevel])
	{
		currentTexture[textureLevel] = texture;
		if(currentTexture[textureLevel])
		{
			if(debugEnabled)
			{
				Logger::Debug("Bind texture: id %d", texture->id);
			}
            
#if defined(__DAVAENGINE_OPENGL__)
            RENDER_VERIFY(glActiveTexture(GL_TEXTURE0 + textureLevel));
            if (textureLevel != 0)
            {
                // todo: boroda: fix that for OpenGL ES because glDisable / glEnable is deprecated
                // fix it pizdato
                if (GetRenderer() != Core::RENDERER_OPENGL_ES_2_0)
                    RENDER_VERIFY(glEnable(GL_TEXTURE_2D));
            }
            RENDER_VERIFY(glBindTexture(GL_TEXTURE_2D, texture->id));
#elif defined(__DAVAENGINE_DIRECTX9__)
            RENDER_VERIFY(GetD3DDevice()->SetTexture(textureLevel, texture->id));
#endif
		}else
        {
#if defined(__DAVAENGINE_OPENGL__)
            RENDER_VERIFY(glActiveTexture(GL_TEXTURE0 + textureLevel));
            if (textureLevel != 0)
            {
                // todo: boroda: fix that for OpenGL ES because glDisable / glEnable is deprecated
                // fix it pizdato
                if (GetRenderer() != Core::RENDERER_OPENGL_ES_2_0)
                    RENDER_VERIFY(glDisable(GL_TEXTURE_2D));
            }
#endif
        }
	}*/
}
	
Texture *RenderManager::GetTexture(uint32 textureLevel)
{
    DVASSERT(textureLevel < RenderStateBlock::MAX_TEXTURE_LEVELS);
	return currentState.currentTexture[textureLevel];	
}
    
void RenderManager::SetShader(Shader * _shader)
{
//    SafeRelease(shader);
//    shader = SafeRetain(_shader);
    currentState.SetShader(_shader);
}

Shader * RenderManager::GetShader()
{
    return currentState.shader;
}


void RenderManager::SetRenderData(RenderDataObject * object)
{
    SafeRelease(currentRenderData);
    currentRenderData = SafeRetain(object);
}
		
void RenderManager::SetClip(const Rect &rect)
{
	SetHWClip(rect);
}
	
void RenderManager::RemoveClip()
{
	SetHWClip(Rect(0,0,-1,-1));
}

void RenderManager::ClipRect(const Rect &rect)
{
	Rect r = currentClip;
	if(r.dx < 0)
	{
		r.dx = (float32)retScreenWidth;
	}
	if(r.dy < 0)
	{
		r.dy = (float32)retScreenHeight;
	}
	
	r = r.Intersection(rect);
	SetHWClip(rect);
}

void RenderManager::ClipPush()
{
	clipStack.push(currentClip);
}

void RenderManager::ClipPop()
{
	if(clipStack.empty())
	{
		Rect r(0, 0, -1, -1);
		SetClip(r);
	}
	else
	{
		Rect r = clipStack.top();
		SetClip(r);
	}
	clipStack.pop();
}
	
void RenderManager::InitFBO(GLuint _viewRenderbuffer, GLuint _viewFramebuffer)
{
	fboViewRenderbuffer = _viewRenderbuffer;
	fboViewFramebuffer = _viewFramebuffer;
}

void RenderManager::SetRenderTarget(Sprite *renderTarget)
{
//	Logger::Info("Set Render target");
	RenderTarget rt;
	rt.spr = currentRenderTarget;
	rt.orientation = renderOrientation;
	renderTargetStack.push(rt);
		
	ClipPush();
	PushDrawMatrix();
	PushMappingMatrix();
	IdentityDrawMatrix();
	SetHWRenderTarget(renderTarget);
}
	
void RenderManager::RestoreRenderTarget()
{
//	Logger::Info("Restore Render target");
	RenderTarget rt = renderTargetStack.top();
	renderTargetStack.pop();
	SetHWRenderTarget(rt.spr);

	PopDrawMatrix();
	PopMappingMatrix();
	ClipPop();
}

bool RenderManager::IsRenderTarget()
{
	return currentRenderTarget != NULL;
}
    
/*
bool RenderManager::IsDepthTestEnabled()
{
    return (hardwareState.state & RenderStateBlock::STATE_DEPTH_TEST) != 0;
}

bool RenderManager::IsDepthWriteEnabled()
{
    return (depthWriteEnabled & RenderStateBlock::STATE_DEPTH_WRITE) != 0;
}
*/

void RenderManager::SetNewRenderEffect(RenderEffect *renderEffect)
{
	SafeRelease(currentRenderEffect);
	currentRenderEffect = SafeRetain(renderEffect);
}

void RenderManager::SetRenderEffect(RenderEffect *renderEffect)
{
	//renderEffectStack.push(SafeRetain(currentRenderEffect));
	SetNewRenderEffect(renderEffect);
}

void RenderManager::DrawElements(ePrimitiveType type, int32 count, eIndexFormat indexFormat, void * indices)
{
	if (currentRenderEffect)
		currentRenderEffect->DrawElements(type, count, indexFormat, indices);
}

void RenderManager::DrawArrays(ePrimitiveType type, int32 first, int32 count)
{
	if (currentRenderEffect)
		currentRenderEffect->DrawArrays(type, first, count);
}

void RenderManager::Lock()
{
	glMutex.Lock();
}
void RenderManager::Unlock()
{
	glMutex.Unlock();
}
	
void RenderManager::LockNonMain()
{
	if(!Thread::IsMainThread())
	{
		if(!lockCount)
		{
			Lock();
		}
		lockCount++;
	}
}
	
int32 RenderManager::GetNonMainLockCount()
{
	return lockCount;
}

void RenderManager::UnlockNonMain()
{
	if(!Thread::IsMainThread())
	{
		lockCount--;
		if(!lockCount)
		{
			Unlock();
		}
	}
}

void RenderManager::SetFPS(int32 newFps)
{
	fps = newFps;	
}
int32 RenderManager::GetFPS()
{
	return fps;
}
	
	
void RenderManager::SetDrawTranslate(const Vector2 &offset)
{
    mappingMatrixChanged = true;
	userDrawOffset.x += offset.x * userDrawScale.x;
	userDrawOffset.y += offset.y * userDrawScale.y;
}

void RenderManager::SetDrawTranslate(const Vector3 &offset)
{
    mappingMatrixChanged = true;
    userDrawOffset.x += offset.x * userDrawScale.x;
    userDrawOffset.y += offset.y * userDrawScale.y;
}
    
void RenderManager::SetDrawScale(const Vector2 &scale)
{
    mappingMatrixChanged = true;
	userDrawScale.x *= scale.x;
	userDrawScale.y *= scale.y;
}
	
void RenderManager::IdentityDrawMatrix()
{
    mappingMatrixChanged = true;
	userDrawScale.x = 1.0f;
	userDrawScale.y = 1.0f;

	userDrawOffset.x = 0.0f;
	userDrawOffset.y = 0.0f;
}

void RenderManager::IdentityMappingMatrix()
{
    mappingMatrixChanged = true;
	viewMappingDrawOffset = Vector2(0, 0);
	viewMappingDrawScale = Vector2(1, 1);
}
	
void RenderManager::IdentityModelMatrix()
{
    mappingMatrixChanged = true;
    RenderManager::Instance()->SetMatrix(MATRIX_MODELVIEW, Matrix4::IDENTITY);
	currentDrawOffset = Vector2(0, 0);
    currentDrawScale = Vector2(1, 1);
}
    
	
	
void RenderManager::SetPhysicalViewScale()
{
    mappingMatrixChanged = true;
	viewMappingDrawScale.x = 1.0f;
	viewMappingDrawScale.y = 1.0f;
}

void RenderManager::SetPhysicalViewOffset()
{
    mappingMatrixChanged = true;
	viewMappingDrawOffset = Core::Instance()->GetPhysicalDrawOffset();
}

void RenderManager::SetVirtualViewScale()
{
    mappingMatrixChanged = true;
	viewMappingDrawScale.x = Core::GetVirtualToPhysicalFactor();
	viewMappingDrawScale.y = Core::GetVirtualToPhysicalFactor();
}

void RenderManager::SetVirtualViewOffset()
{
    mappingMatrixChanged = true;
	viewMappingDrawOffset.x -= Core::Instance()->GetVirtualScreenXMin() * viewMappingDrawScale.x;
	viewMappingDrawOffset.y -= Core::Instance()->GetVirtualScreenYMin() * viewMappingDrawScale.y;
}
	
void RenderManager::PushDrawMatrix()
{
	DrawMatrix dm;
	dm.userDrawOffset = userDrawOffset;
	dm.userDrawScale = userDrawScale;
	matrixStack.push(dm);
}

void RenderManager::PopDrawMatrix()
{
	IdentityDrawMatrix();
	DrawMatrix dm = matrixStack.top();
	matrixStack.pop();
	userDrawOffset = dm.userDrawOffset;
	userDrawScale = dm.userDrawScale;
}
	
void RenderManager::PushMappingMatrix()
{
	DrawMatrix dm;
	dm.userDrawOffset = viewMappingDrawOffset;
	dm.userDrawScale = viewMappingDrawScale;
	mappingMatrixStack.push(dm);
}

void RenderManager::PopMappingMatrix()
{
	IdentityMappingMatrix();
	DrawMatrix dm = mappingMatrixStack.top();
	mappingMatrixStack.pop();
	viewMappingDrawOffset = dm.userDrawOffset;
	viewMappingDrawScale = dm.userDrawScale;
}

void RenderManager::SetCursor(Cursor * _cursor)
{
#if defined(__DAVAENGINE_MACOS__) || defined(__DAVAENGINE_WIN32__)
	SafeRelease(cursor);
	cursor = SafeRetain(_cursor);
	if (cursor)cursor->HardwareSet();
#endif
}

Cursor * RenderManager::GetCursor()
{
	return cursor;
}
	
const RenderManager::Caps & RenderManager::GetCaps()
{
	return caps;
}
    
const RenderManager::Stats & RenderManager::GetStats()
{
    return stats;
}
    
void RenderManager::RectFromRenderOrientationToViewport(Rect & rect)
{
    switch(renderOrientation)
    {
        case Core::SCREEN_ORIENTATION_PORTRAIT:
            break;
        case Core::SCREEN_ORIENTATION_LANDSCAPE_LEFT:
            {
                float32 newX = (float32)frameBufferWidth - (rect.y + rect.dy);
                float32 newY = (float32)frameBufferHeight - rect.x;
                float32 newDX = rect.dy;
                float32 newDY = rect.dx;
                rect.x = newX;
                rect.y = newY;
                rect.dx = newDX;
                rect.dy = newDY;
            }
            break;
        case Core::SCREEN_ORIENTATION_LANDSCAPE_RIGHT:
            {
                float32 newX = (float32)frameBufferWidth - (rect.y + rect.dy);
                float32 newY = (float32)frameBufferHeight - rect.x;
                float32 newDX = rect.dy;
                float32 newDY = rect.dx;
                rect.x = newX;
                rect.y = newY;
                rect.dx = newDX;
                rect.dy = newDY;
            }            
            break;
            
    };
}

const Matrix4 & RenderManager::GetMatrix(eMatrixType type)
{
    return matrices[type];
}

const Matrix4 & RenderManager::GetUniformMatrix(eUniformMatrixType type)
{
    if (uniformMatrixFlags[type] == 0)
    {
        if (type == UNIFORM_MATRIX_MODELVIEWPROJECTION)
        {
            uniformMatrices[type] =  matrices[MATRIX_MODELVIEW] * matrices[MATRIX_PROJECTION];
        }
        uniformMatrixFlags[type] = 1; // matrix is ready
    }
    return uniformMatrices[type];
}
    
void RenderManager::ClearUniformMatrices()
{
    for (int32 k = 0; k < UNIFORM_MATRIX_COUNT; ++k)
        uniformMatrixFlags[k] = 0;
}
    
void RenderManager::Stats::Clear()
{
    drawArraysCalls = 0;
    drawElementsCalls = 0;
    for (int32 k = 0; k < PRIMITIVETYPE_COUNT; ++k)
        primitiveCount[k] = 0;
}

void RenderManager::EnableOutputDebugStatsEveryNFrame(int32 _frameToShowDebugStats)
{
    frameToShowDebugStats = _frameToShowDebugStats;
}

void RenderManager::ProcessStats()
{ 
    if (frameToShowDebugStats == -1)return;
    
    statsFrameCountToShowDebug++;
    if (statsFrameCountToShowDebug >= frameToShowDebugStats)
    {
        statsFrameCountToShowDebug = 0;
        Logger::Debug("== Frame stats: DrawArraysCount: %d DrawElementCount: %d ==", stats.drawArraysCalls, stats.drawElementsCalls);
        for (int32 k = 0; k < PRIMITIVETYPE_COUNT; ++k)
            Logger::Debug("== Primitive Stats: %d ==", stats.primitiveCount[k]);
    }
}
    
    /*void RenderManager::EnableAlphaTest(bool isEnabled)
{
    alphaTestEnabled = isEnabled;
}

 
void RenderManager::EnableCulling(bool isEnabled)
{
    cullingEnabled = isEnabled;
}*/

void RenderManager::AppendState(uint32 state)
{
    currentState.state |= state;
}
    
void RenderManager::RemoveState(uint32 state)
{
    currentState.state &= ~state;
}

void RenderManager::SetState(uint32 state)
{
    currentState.state = state;
}
    
uint32 RenderManager::GetState()
{
    return currentState.state;
}
    
    
void RenderManager::SetCullMode(eCull _cullFace)
{
    currentState.SetCullMode(_cullFace);
}
    
void RenderManager::SetAlphaFunc(eCmpFunc func, float32 cmpValue)
{
    currentState.SetAlphaFunc(func, cmpValue);
}

	
};