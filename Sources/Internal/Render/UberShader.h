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
#ifndef __DAVAENGINE_UBERSHADER_H__
#define __DAVAENGINE_UBERSHADER_H__

#include "Base/BaseTypes.h"
#include "Render/RenderBase.h"
#include "Render/RenderResource.h"

namespace DAVA
{
    
/**
    \brief This is shader cache that is compiled from one shader
 */
class Shader;
class UberShader : public BaseObject
{
public:
    UberShader();
    ~UberShader();
    
    bool LoadShader(const String & shaderFileName);
    int32 CompileShaderCombination(const String & combination);
    Shader * GetShader(const String & combination);

private:
    struct CompiledShader : public BaseObject
    {
        CompiledShader();
        
        int32 index;
        String  combination;
        Shader * shader;
    };
    
    Vector<CompiledShader*> compiledShaders;
    Map<String, CompiledShader *> compiledShadersMap;
    
    //int32   shaderLimit;
    //bool    compileEverythingAtCreateTime;
    Shader * baseShader;
};
    
}

#endif // __DAVAENGINE_UBERSHADER_H__
