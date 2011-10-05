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
#include "Render/RenderEffect.h"
#include "Render/Texture.h"
#include "Render/RenderManager.h"
#include "Render/Effects/MultiTextureEffect.h"
#include "Render/Shader.h"

namespace DAVA 
{
    
    
RenderEffect * MultiTextureEffect::Create(Core::eRenderer renderer)
{
#if defined(__DAVAENGINE_OPENGL__)
    CONDITIONAL_CREATE(Core::RENDERER_OPENGL, MultiTextureEffectGL);
    CONDITIONAL_CREATE(Core::RENDERER_OPENGL_ES_1_0, MultiTextureEffectGL);
    CONDITIONAL_CREATE(Core::RENDERER_OPENGL_ES_2_0, MultiTextureEffectGL20);
#endif 
    
#if defined(__DAVAENGINE_DIRECTX9__)
    CONDITIONAL_CREATE(Core::RENDERER_DIRECTX9, MultiTextureEffectDX9);
#endif
    return 0;
}

#if defined(__DAVAENGINE_OPENGL__)
   
void MultiTextureEffectGL::DrawArrays(ePrimitiveType mode, int32 first, int32 count)
{
    RenderManager::Instance()->EnableTexturing(true);
    RenderManager::Instance()->SetShader(0);
    RenderManager::Instance()->FlushState();
    RenderManager::Instance()->HWDrawArrays(mode, first, count);
}
    
MultiTextureEffectGL20::MultiTextureEffectGL20()
{
    shader = new Shader();
    shader->LoadFromYaml("~res:/Shaders/Default/fixed_func_multi_texture.shader");
}

MultiTextureEffectGL20::~MultiTextureEffectGL20()
{
    SafeRelease(shader);
}

void MultiTextureEffectGL20::DrawArrays(ePrimitiveType mode, int32 first, int32 count)
{
    RenderManager::Instance()->SetShader(shader);
    RenderManager::Instance()->FlushState();
    RenderManager::Instance()->HWDrawArrays(mode, first, count);

}
#endif 
    
#if defined(__DAVAENGINE_DIRECTX9__)
    
void MultiTextureEffectDX9::DrawArrays(ePrimitiveType mode, int32 first, int32 count)
{
	RenderManager::Instance()->EnableTexturing(true);
	RenderManager::Instance()->FlushState();
	RenderManager::Instance()->HWDrawArrays(mode, first, count);
}

void MultiTextureEffectDX9::DrawElements(ePrimitiveType type, int32 count, eIndexFormat indexFormat, void * indices)
{
	RenderManager::Instance()->EnableTexturing(true);
	RenderManager::Instance()->FlushState();
	RenderManager::Instance()->HWDrawElements(type, count, indexFormat, indices);
}
#endif
    
};
