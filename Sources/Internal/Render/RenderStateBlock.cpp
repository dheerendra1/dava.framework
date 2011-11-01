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
    
}

    
    
/**
    Function to reset state to original zero state.
 */
void RenderStateBlock::Reset(bool doHardwareReset)
{
    state = 0;
    color.r = 1.0f;
    color.g = 1.0f;
    color.b = 1.0f;
    color.a = 1.0f;
    sourceFactor = BLEND_ONE;
    destFactor = BLEND_ZERO;
    for (int32 idx = 0; idx < MAX_TEXTURE_LEVELS; ++idx)
        currentTexture[idx] = 0;
    
    if (doHardwareReset)
    {
        SetColorInHW();
        SetEnableBlendingInHW();
        SetBlendModeInHW();
        SetEnableTexturingInHW();
        for (int32 textureLevel = 0; textureLevel < MAX_TEXTURE_LEVELS; ++textureLevel)
            {
                SetTextureLevelInHW(textureLevel);
            }
        
        SetDepthTestInHW();
        SetDepthWriteInHW();
    }
}

void RenderStateBlock::Flush(RenderStateBlock * previousState)
{
//#if defined(__DAVAENGINE_OPENGL__)
    if(color != previousState->color)
	{
        SetColorInHW();
	}   
 
    state |= STATE_BLEND_ENABLED;
    if ((state ^ previousState->state) & STATE_BLEND_ENABLED)
    {
        SetEnableBlendingInHW();
    }
    sourceFactor = BLEND_ONE;
    destFactor = BLEND_ONE_MINUS_SRC_ALPHA;
    if(sourceFactor != previousState->sourceFactor || destFactor != previousState->destFactor)
	{
        SetBlendModeInHW();
	}
	if((state ^ previousState->state) & STATE_TEXTURE_ENABLED)
	{
        SetEnableTexturingInHW();
	}    
    
    
    for (int32 textureLevel = 0; textureLevel < MAX_TEXTURE_LEVELS; ++textureLevel)
        //if(currentTexture[textureLevel] != previousState->currentTexture[textureLevel])
        {
            SetTextureLevelInHW(textureLevel);
        }
    
    
	if((state ^ previousState->state) & STATE_DEPTH_TEST_ENABLED)
	{
        SetDepthTestInHW();
	}
    
    if((state ^ previousState->state) & STATE_DEPTH_WRITE_ENABLED)
    {
        SetDepthWriteInHW();
    }
    
//    // Full set
//    SetColorInHW();
//    SetEnableBlendingInHW();
//    SetBlendModeInHW();
//    SetEnableTexturingInHW();
//    for (int32 textureLevel = 0; textureLevel < 1; ++textureLevel)
//    {
//        SetTextureLevelInHW(textureLevel);
//    }
//    
//    SetDepthTestInHW();
//    SetDepthWriteInHW();
    
/*#elif defined(__DAVAENGINE_DIRECTX9__)
    
    
    for (int32 textureLevel = 0; textureLevel < MAX_TEXTURE_LEVELS; ++textureLevel)
        if(currentTexture[textureLevel] != previousState->currentTexture[textureLevel])
        {
            if(currentTexture[textureLevel])
            {
                RENDER_VERIFY(GetD3DDevice()->SetTexture(textureLevel, currentTexture[textureLevel]->id));
            }else
            {
                RENDER_VERIFY(GetD3DDevice()->SetTexture(textureLevel, 0));
            }
        }

#endif // 
    */
}
    
    
#if defined (__DAVAENGINE_OPENGL__)
inline void RenderStateBlock::SetColorInHW()
{
    if (renderer != Core::RENDERER_OPENGL_ES_2_0)
        RENDER_VERIFY(glColor4f(color.r * color.a, color.g * color.a, color.b * color.a, color.a));
}

inline void RenderStateBlock::SetEnableBlendingInHW()
{
    if (state & STATE_BLEND_ENABLED)
    {
        RENDER_VERIFY(glEnable(GL_BLEND));
    }
    else 
    {
        RENDER_VERIFY(glDisable(GL_BLEND));
    }
}

inline void RenderStateBlock::SetBlendModeInHW()
{
    RENDER_VERIFY(glBlendFunc(BLEND_MODE_MAP[sourceFactor], BLEND_MODE_MAP[destFactor]));
}

inline void RenderStateBlock::SetEnableTexturingInHW()
{
    if (state & STATE_TEXTURE_ENABLED)
    {
        RENDER_VERIFY(glEnable(GL_TEXTURE_2D)); 
    }
    else 
    {
        RENDER_VERIFY(glDisable(GL_TEXTURE_2D));
    }
}

inline void RenderStateBlock::SetTextureLevelInHW(uint32 textureLevel)
{
    if(currentTexture[textureLevel])
    {
        RENDER_VERIFY(glActiveTexture(GL_TEXTURE0 + textureLevel));
        RENDER_VERIFY(glBindTexture(GL_TEXTURE_2D, currentTexture[textureLevel]->id));
    }else
    {
        RENDER_VERIFY(glActiveTexture(GL_TEXTURE0 + textureLevel));
        RENDER_VERIFY(glBindTexture(GL_TEXTURE_2D, 0));
    }    
    RENDER_VERIFY(glActiveTexture(GL_TEXTURE0));
}
inline void RenderStateBlock::SetDepthTestInHW()
{
    if(state & STATE_DEPTH_TEST_ENABLED)
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
    if(state & STATE_DEPTH_WRITE_ENABLED)
    {
        RENDER_VERIFY(glDepthMask(GL_TRUE));
    }
    else
    {
        RENDER_VERIFY(glDepthMask(GL_FALSE));
    }
}
    
#elif defined(__DAVAENGINE_DIRECTX9__)
    
    
#endif 
    
    
    
    
};