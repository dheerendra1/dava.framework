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
#ifndef __DAVAENGINE_SHADER_H__
#define __DAVAENGINE_SHADER_H__

#include "Render/RenderBase.h"
#include "Render/RenderResource.h"
#include "Render/VertexBuffer.h"
#include "Base/BaseMath.h"

namespace DAVA
{
    
class ShaderParam
{
public:
    
    ShaderParam * next;
};

/*	
    \brief Class to use low-level shaders
 
*/	
class Shader : public RenderResource
{
public:
    enum eUniform
    {
        UNIFORM_NONE = 0, 
        UNIFORM_MODEL_VIEW_PROJECTION_MATRIX,  // gl_ModelViewProjectionMatrix
        UNIFORM_COLOR,
        UNIFORM_COUNT,
    };

    Shader();
    virtual ~Shader();
    
    // virtual void SetActiveShader(const String & string);
    virtual bool LoadFromYaml(const String & pathname);
    virtual void Bind();
    static void Unbind();
    virtual int32 FindUniformLocationByName(const String & name);
    int32 GetAttributeIndex(eVertexFormat vertexFormat);
    
    
    void SetUniformValue(int32 uniformLocation, int32 value);
    void SetUniformValue(int32 uniformLocation, const Vector3 & vector);
    void SetUniformValue(int32 uniformLocation, const Vector4 & vector);
    void SetUniformValue(int32 uniformLocation, const Matrix4 & matrix);


    /**
        This function return vertex format required by shader
     */
    //virtual uint32 GetVertexFormat();
    //virtual uint32 GetAttributeIndex(eVertexFormat fmt);
    
private:
#if defined(__DAVAENGINE_DIRECTX9__)
    
    
#elif defined(__DAVAENGINE_OPENGL__)
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint program;
    
    String * attributeNames;
    GLint activeAttributes;
    GLint activeUniforms;
    
    
    eUniform *uniformIDs;
    String * uniformNames;
    GLint * uniformLocations;
    
    int32 vertexFormatAttribIndeces[VERTEX_FORMAT_STREAM_MAX_COUNT];
    
    GLint CompileShader(GLuint *shader, GLenum type, GLint count, const GLchar * sources);    
    GLint LinkProgram(GLuint prog);
    void DeleteShaders();
    eUniform GetUniformByName(const char * name);
    int32 GetAttributeIndexByName(const char * name);
    
    static GLuint activeProgram;
#endif
};
};

#endif // __DAVAENGINE_RENDERSTATEBLOCK_H__
