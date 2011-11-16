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
#include "Render/UberShader.h"
#include "Render/Shader.h"

namespace DAVA 
{
    

UberShader::CompiledShader::CompiledShader()
{
    shader = 0;
}


UberShader::UberShader()
    : baseShader(0)
{
}

UberShader::~UberShader()
{
    size_t size = compiledShaders.size();
    for (size_t k = 0; k < size; ++k)
    {
        SafeRelease(compiledShaders[k]->shader);
        SafeRelease(compiledShaders[k]);
    }
    compiledShaders.clear();
    SafeRelease(baseShader);
}
    
bool UberShader::LoadShader(const String & shaderFileName)
{
    baseShader = new Shader();
    if (!baseShader->LoadFromYaml(shaderFileName))
    {
        SafeRelease(baseShader);
        return false;
    }
    return true;
}


int32 UberShader::CompileShaderCombination(const String & combination)
{
    if (!baseShader)return -1;
 
    // If combination already compiled just return existing index
    Map<String, CompiledShader *>::iterator it = compiledShadersMap.find(combination);
    if (it != compiledShadersMap.end())
    {
        CompiledShader * c = it->second;
        return c->index;
    }
    
    // Create new shader and combile it
    Shader * newShader = baseShader->RecompileNewInstance(combination);
    
    if (newShader)
    {
        CompiledShader * c = new CompiledShader();
        c->index = compiledShaders.size();
        c->combination = combination;
        c->shader = newShader;
        compiledShaders.push_back(c);
        compiledShadersMap[c->combination] = c;
        return c->index; 
    }else
    {
        Logger::Error("Failed to compile ubershader with combination: %s", combination.c_str());
        return -1;
    }
}

Shader * UberShader::GetShader(const String & combination)
{
    Map<String, CompiledShader *>::iterator it = compiledShadersMap.find(combination);
    if (it != compiledShadersMap.end())
    {
        return it->second->shader;
    }
    return 0;
}

    
    


}



