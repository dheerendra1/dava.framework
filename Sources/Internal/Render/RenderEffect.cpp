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

namespace DAVA 
{
Map<String, RenderEffect*> RenderEffect::effectsMap;
    
RenderEffect::RenderEffect()
{
    const String & name = GetName();
    Map<String, RenderEffect *>::iterator effectIt = effectsMap.find(name);
    if (effectIt == effectsMap.end())
    {
        effectsMap[name] = this; 
    }else
    {
        Logger::Debug("RenderEffect::RenderEffect effect with such name: %s already registered", name.c_str());
    }
}

RenderEffect::~RenderEffect()
{
    effectsMap.erase(GetName());
}
const char * RenderEffect::GetName()
{
    return "EmptyEffect";
}

void RenderEffect::DrawArrays(ePrimitiveType mode, int32 first, int32 count)
{
    DVASSERT("RenderEffect::DrawArrays not implemented but called" && 0);
}
    
void RenderEffect::DrawElements(ePrimitiveType type, int32 count, eIndexFormat indexFormat, void * indices)
{
    DVASSERT("RenderEffect::DrawElements not implemented but called" && 0);
}

};
