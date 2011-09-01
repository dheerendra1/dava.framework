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
#ifndef __DAVAENGINE_MULTI_TEXTURE_EFFECT_H__
#define __DAVAENGINE_MULTI_TEXTURE_EFFECT_H__

#include "Base/BaseTypes.h"
#include "Base/BaseObject.h"
#include "Render/RenderBase.h"

namespace DAVA
{
class Texture;
class Shader;
class MultiTextureEffect : public RenderEffect
{
public:
    DECLARE_EFFECT(FixedFunc_MultiTextureEffect);

    static RenderEffect * Create(Core::eRenderer renderer);
};

#if defined(__DAVAENGINE_OPENGL__)
class MultiTextureEffectGL : public MultiTextureEffect
{
public:
	virtual void DrawArrays(ePrimitiveType mode, int32 first, int32 count);
	virtual void DrawElements(ePrimitiveType type, int32 count, eIndexFormat indexFormat, void * indices); 
};

class MultiTextureEffectGL20 : public MultiTextureEffect
{
public:
    MultiTextureEffectGL20();
    ~MultiTextureEffectGL20();

	virtual void DrawArrays(ePrimitiveType mode, int32 first, int32 count);
	virtual void DrawElements(ePrimitiveType type, int32 count, eIndexFormat indexFormat, void * indices); 
protected:
    Shader * shader;
};
#elif defined(__DAVAENGINE_DIRECTX9__)

class MultiTextureEffectDX9 : public MultiTextureEffect
{
public:
	virtual void DrawArrays(ePrimitiveType mode, int32 first, int32 count);
	virtual void DrawElements(ePrimitiveType type, int32 count, eIndexFormat indexFormat, void * indices); 
};

#endif 

};

#endif // __DAVAENGINE_TEXTURE_MUL_COLOR_EFFECT_H__
