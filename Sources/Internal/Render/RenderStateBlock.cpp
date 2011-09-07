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

void RenderStateBlock::RegisterStateUInt32(uint32 state, UpdateVarFunc func)
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
    

    
    
    
};