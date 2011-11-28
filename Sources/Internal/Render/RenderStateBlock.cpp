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
#include "Render/RenderStateBlock.h"
#include "Render/RenderManager.h"
#include "Render/Shader.h"
#include "Platform/Thread.h"

namespace DAVA
{

/*void RenderStateBlock::RegisterStateUInt32(uint32 state, UpdateVarFunc func)
{
    uint32 idx = CountLeadingZeros(state);
    stateType[idx] = STATE_TYPE_UINT32;
    varUpdateFuncArray[idx] = func;
}

void RenderStateBlock::RegisterStateColor(uint32 state, UpdateVarFuncColor func)
{
    uint32 idx = CountLeadingZeros(state);
    stateType[idx] = STATE_TYPE_COLOR;
    varUpdateFuncArrayColor[idx] = func;
}
    
void RenderStateBlock::Init(Core::eRenderer renderer)
{
    isDebug = true;
    activeStateSet = 0;
    currentStateSet = 0;
    
    switch (renderer) {
        case Core::RENDERER_OPENGL:
        case Core::RENDERER_OPENGL_ES_1_0:
            RegisterStateUInt32(STATE_ENABLE_TEXTURING, &RenderStateBlock::UpdateEnableTexturingGL);

        case Core::RENDERER_OPENGL_ES_2_0:
            RegisterStateUInt32(STATE_BLEND, &RenderStateBlock::UpdateBlendGL);
            RegisterStateUInt32(STATE_BLENDFUNC, &RenderStateBlock::UpdateBlendFuncGL);
            break;
        case Core::RENDERER_DIRECTX9:
            RegisterStateUInt32(STATE_BLEND, &RenderStateBlock::UpdateBlendDX);
            RegisterStateUInt32(STATE_BLENDFUNC, &RenderStateBlock::UpdateBlendFuncDX);
            RegisterStateUInt32(STATE_ENABLE_TEXTURING, &RenderStateBlock::UpdateEnableTexturingDX);
            break;
            
        default:
            break;
    }
    
}
    
#if defined(__DAVAENGINE_OPENGL__)
void RenderStateBlock::UpdateBlendGL(uint32 newValue, uint32 oldValue)
{
    if (newValue)glEnable(GL_BLEND);
    else glDisable(GL_BLEND);
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


void RenderStateBlock::UpdateBlendFuncGL(uint32 newValue, uint32 oldValue)
{
    uint32 src = newValue & 0xff;
    uint32 dest = (newValue >> 8) & 0xff;
    glBlendFunc(BLEND_MODE_MAP[src], BLEND_MODE_MAP[dest]);
}

#endif 

#if defined(__DAVAENGINE_DIRECTX9__)
void RenderStateBlock::UpdateBlendDX(uint32 newValue, uint32 oldValue)
{
//    direct3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, newValue)
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

void RenderStateBlock::UpdateBlendFuncDX(uint32 newValue, uint32 oldValue)
{
    uint32 newSFactor = newValue & 0xff;
    uint32 newDFactor = (newValue >> 8) & 0xff;

    uint32 oldSFactor = oldValue & 0xff;
    uint32 oldDFactor = (oldValue >> 8) & 0xff;
    
//     if(newSFactor != oldSFactor )
//         RENDER_VERIFY(direct3DDevice->SetRenderState(D3DRS_SRCBLEND, BLEND_MODE_MAP[newSFactor]));
//     if (newDFactor != oldDFactor)
//         RENDER_VERIFY(direct3DDevice->SetRenderState(D3DRS_DESTBLEND, BLEND_MODE_MAP[newDFactor]));

}
#endif 

void RenderStateBlock::Flush()
{
    uint32 changeSet = activeStateSet ^ currentStateSet;
    if (changeSet != 0)
    {
        for (uint32 k = 0; k < STATE_COUNT; ++k)
        {
            if (((changeSet >> k) & 1) && (varValue[k] != currentRendererValue[k]))
            {
                if (stateType[k] == STATE_TYPE_UINT32)
                {
                    if (isDebug)Logger::Debug("Value %d changed on flush");
                    (this->*varUpdateFuncArray[k])(varValue[k], currentRendererValue[k]);
                    currentRendererValue[k] = varValue[k];
                }else if (stateType[k] == STATE_TYPE_COLOR)
                {
                    if (isDebug)Logger::Debug("ColorValue %d changed on flush");
                    (this->*varUpdateFuncArrayColor[k])(varValueColor[k], currentRendererValueColor[k]);
                    currentRendererValueColor[k] = varValueColor[k];
                }
            }
        }
    }
    activeStateSet = currentStateSet;
}
    

*/
    
RenderStateBlock::RenderStateBlock(Core::eRenderer _renderer)
    : renderer(_renderer)
{
    Reset(false);
}

RenderStateBlock::~RenderStateBlock()
{
    
}
    
    
/**
    Function to reset state to original zero state.
 */
void RenderStateBlock::Reset(bool doHardwareReset)
{
    state = 0;
    changeSet = 0;
    color.r = 1.0f;
    color.g = 1.0f;
    color.b = 1.0f;
    color.a = 1.0f;
    sourceFactor = BLEND_ONE;
    destFactor = BLEND_ZERO;
    for (int32 idx = 0; idx < MAX_TEXTURE_LEVELS; ++idx)
        currentTexture[idx] = 0;
    alphaFunc = CMP_ALWAYS;
    shader = 0;
    
    if (doHardwareReset)
    {
        SetColorInHW();
        SetEnableBlendingInHW();
        SetBlendModeInHW();
        SetDepthTestInHW();
        SetDepthWriteInHW();
        SetAlphaTestInHW();
        SetAlphaTestFuncInHW();

        for (int32 textureLevel = 0; textureLevel < MAX_TEXTURE_LEVELS; ++textureLevel)
        {
            SetTextureLevelInHW(textureLevel);
        }
    }
}
bool RenderStateBlock::IsEqual(RenderStateBlock * anotherState)
{
    if (state != anotherState->state)
        return false;
    
    // check texture first for early rejection 
    if (currentTexture[0] != anotherState->currentTexture[0])return false;

    if (state & STATE_BLEND)
    {
        if (destFactor != anotherState->destFactor)return false;
        if (sourceFactor != anotherState->sourceFactor)return false;
    }
    
    if (color != anotherState->color)return false;
    
    if (currentTexture[1] != anotherState->currentTexture[1])return false;
    if (currentTexture[2] != anotherState->currentTexture[2])return false;
    if (currentTexture[3] != anotherState->currentTexture[3])return false;
    
    
    
    
    return true;
}

void RenderStateBlock::Flush(RenderStateBlock * previousState)
{
    uint32 diffState = state ^ previousState->state;
    if (diffState != 0)
    {
        if (diffState & STATE_BLEND)
            SetEnableBlendingInHW();
        
        if (diffState & STATE_DEPTH_TEST)
            SetDepthTestInHW();
        
        if (diffState & STATE_DEPTH_WRITE)
            SetDepthWriteInHW();
        
        if (diffState & STATE_CULL)
            SetCullInHW();
        
        if (diffState & STATE_ALPHA_TEST)
            SetAlphaTestInHW();
        
        changeSet |= diffState & (STATE_TEXTURE0 | STATE_TEXTURE1 | STATE_TEXTURE2 | STATE_TEXTURE3);
        
        previousState->state = state;
    }
    
    if (changeSet != 0)
    {
        if (changeSet & STATE_CHANGED_COLOR)
            if (color != previousState->color)
            {
                SetColorInHW();
                previousState->color = color;
            }
        if (changeSet & (STATE_CHANGED_SRC_BLEND | STATE_CHANGED_DEST_BLEND))
            if (sourceFactor != previousState->sourceFactor || destFactor != previousState->destFactor)
            {
                SetBlendModeInHW();
                previousState->sourceFactor = sourceFactor;
                previousState->destFactor = destFactor;
            }
        
        if (changeSet & STATE_CHANGED_CULLMODE)
            if (cullMode != previousState->cullMode)
            {
                SetCullModeInHW();
                previousState->cullMode = cullMode;
            }
        if (changeSet & STATE_CHANGED_ALPHA_FUNC)
            if ((alphaFunc != previousState->alphaFunc) || (alphaFuncCmpValue != previousState->alphaFuncCmpValue))
            {
                SetAlphaTestFuncInHW();
                previousState->alphaFunc = alphaFunc;
                previousState->alphaFuncCmpValue = alphaFuncCmpValue;
            }
        
        if (changeSet & STATE_CHANGED_TEXTURE0)
        {
            SetTextureLevelInHW(0);
            previousState->currentTexture[0] = currentTexture[0];
        }
        if (changeSet & STATE_CHANGED_TEXTURE1)
        {
            SetTextureLevelInHW(1);
            previousState->currentTexture[1] = currentTexture[1];
        }
        if (changeSet & STATE_CHANGED_TEXTURE2)
        {
            SetTextureLevelInHW(2);
            previousState->currentTexture[2] = currentTexture[2];
        }
        if (changeSet & STATE_CHANGED_TEXTURE3)
        {
            SetTextureLevelInHW(3);
            previousState->currentTexture[3] = currentTexture[3];
        }
        
        RENDER_VERIFY(glActiveTexture(GL_TEXTURE0));
        
        if (changeSet & STATE_CHANGED_SHADER)
        {
            if (shader != previousState->shader)
            {
                if (shader)shader->Bind();
                else Shader::Unbind();
                previousState->shader = shader;
            }
        }
        changeSet = 0;
        previousState->changeSet = 0;
    }
}
    
    
#if defined (__DAVAENGINE_OPENGL__)
inline void RenderStateBlock::SetColorInHW()
{
    if (renderer != Core::RENDERER_OPENGL_ES_2_0)
        RENDER_VERIFY(glColor4f(color.r * color.a, color.g * color.a, color.b * color.a, color.a));
}

inline void RenderStateBlock::SetEnableBlendingInHW()
{
    if (state & STATE_BLEND)
    {
        RENDER_VERIFY(glEnable(GL_BLEND));
    }
    else 
    {
        RENDER_VERIFY(glDisable(GL_BLEND));
    }
}
    
inline void RenderStateBlock::SetCullInHW()
{
    if (state & STATE_CULL)
    {
        RENDER_VERIFY(glEnable(GL_CULL_FACE));
    }
    else 
    {
        RENDER_VERIFY(glDisable(GL_CULL_FACE));
    }
}

inline void RenderStateBlock::SetCullModeInHW()
{
    RENDER_VERIFY(glCullFace(CULL_FACE_MAP[cullMode]));
}


inline void RenderStateBlock::SetBlendModeInHW()
{
    RENDER_VERIFY(glBlendFunc(BLEND_MODE_MAP[sourceFactor], BLEND_MODE_MAP[destFactor]));
}

inline void RenderStateBlock::SetTextureLevelInHW(uint32 textureLevel)
{
    if(currentTexture[textureLevel])
    {
        RENDER_VERIFY(glActiveTexture(GL_TEXTURE0 + textureLevel));
        
        if (state & (STATE_TEXTURE0 << textureLevel))
            glEnable(GL_TEXTURE_2D);
        else 
            glDisable(GL_TEXTURE_2D);
        
        RENDER_VERIFY(glBindTexture(GL_TEXTURE_2D, currentTexture[textureLevel]->id));
    }else
    {
        RENDER_VERIFY(glActiveTexture(GL_TEXTURE0 + textureLevel));

        if (state & (STATE_TEXTURE0 << textureLevel))
            glEnable(GL_TEXTURE_2D);
        else 
            glDisable(GL_TEXTURE_2D);
        
        RENDER_VERIFY(glBindTexture(GL_TEXTURE_2D, 0));
    }    
}
inline void RenderStateBlock::SetDepthTestInHW()
{
    if(state & STATE_DEPTH_TEST)
    {
        RENDER_VERIFY(glEnable(GL_DEPTH_TEST));
    }
    else
    {
        RENDER_VERIFY(glDisable(GL_DEPTH_TEST));
    }    
}

inline void RenderStateBlock::SetDepthWriteInHW()
{
    if(state & STATE_DEPTH_WRITE)
    {
        RENDER_VERIFY(glDepthMask(GL_TRUE));
    }
    else
    {
        RENDER_VERIFY(glDepthMask(GL_FALSE));
    }
}
    
inline void RenderStateBlock::SetAlphaTestInHW()
{
    if(state & STATE_ALPHA_TEST)
    {
        RENDER_VERIFY(glEnable(GL_ALPHA_TEST));
    }
    else
    {
        RENDER_VERIFY(glDisable(GL_ALPHA_TEST));
    }
}

inline void RenderStateBlock::SetAlphaTestFuncInHW()
{
     RENDER_VERIFY(glAlphaFunc(ALPHA_TEST_MODE_MAP[alphaFunc], alphaFuncCmpValue) );
}
    
#elif defined(__DAVAENGINE_DIRECTX9__)
    
    
#endif 
    
    
    
    
};