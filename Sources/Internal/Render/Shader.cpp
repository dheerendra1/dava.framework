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
#include "Render/Shader.h"
#include "Render/RenderBase.h"
#include "Platform/Thread.h"
#include "Render/RenderManager.h"
#include "FileSystem/YamlParser.h"
#include "FileSystem/FileSystem.h"

namespace DAVA 
{
#if defined(__DAVAENGINE_OPENGL__)
GLuint Shader::activeProgram = 0;

Shader::Shader()
{
    vertexShader = 0;
    fragmentShader = 0;
    program = 0;
    
    attributeNames = 0;
    activeAttributes = 0;
    activeUniforms = 0;

    uniformIDs = 0;
    uniformNames = 0;
    uniformLocations = 0;
    
    for (int32 ki = 0; ki < VERTEX_FORMAT_STREAM_MAX_COUNT; ++ki)
         vertexFormatAttribIndeces[ki] = -1;
}

String VertexTypeStringFromEnum(GLenum type); // Fucking XCode 4 analyzer
String VertexTypeStringFromEnum(GLenum type)
{
    if (type == GL_FLOAT)return "GL_FLOAT";
    else if (type == GL_FLOAT_VEC2)return "GL_FLOAT_VEC2";
    else if (type == GL_FLOAT_VEC3)return "GL_FLOAT_VEC3";
    else if (type == GL_FLOAT_VEC4)return "GL_FLOAT_VEC4";
    else if (type == GL_FLOAT_MAT2)return "GL_FLOAT_MAT2";
    else if (type == GL_FLOAT_MAT3)return "GL_FLOAT_MAT3";
    else if (type == GL_FLOAT_MAT4)return "GL_FLOAT_MAT4";
    return "";
}
    
const char * uniformStrings[Shader::UNIFORM_COUNT] = 
    {
        "none",
        "modelViewProjectionMatrix",
        "flatColor",
    };
const char * attributeStrings[VERTEX_FORMAT_STREAM_MAX_COUNT] = 
    {
        "inPosition",
        "inNormal",
        "inColor",
        "inTexCoord",
        "inTexCoord1",
        "inTexCoord2",
        "inTexCoord3",
        "inTangent",
        "inBinormal",
        "inJointWeight"
    };
    
Shader::eUniform Shader::GetUniformByName(const char * name)
{
    for (int32 k = 0; k < UNIFORM_COUNT; ++k)
        if (strcmp(name, uniformStrings[k]) == 0)return (Shader::eUniform)k; 
    return Shader::UNIFORM_NONE;
};

int32 Shader::GetAttributeIndexByName(const char * name)
{
    for (int32 k = 0; k < VERTEX_FORMAT_STREAM_MAX_COUNT; ++k)
        if (strcmp(name, attributeStrings[k]) == 0)return k;
    return -1;
}
    
bool Shader::LoadFromYaml(const String & pathname)
{
    String pathOnly, shaderFilename;
    FileSystem::SplitPath(pathname, pathOnly, shaderFilename);
    
    
    YamlParser * parser = YamlParser::Create(pathname);
    if (!parser)
        return false;
    
    YamlNode * rootNode = parser->GetRootNode();
    if (!rootNode)
    {
        SafeRelease(rootNode);
        return false;
    }
    
    YamlNode * vertexShaderNode = rootNode->Get("vertexShader");
    if (!vertexShaderNode)
    {
        SafeRelease(parser);
        return false;
    }

    YamlNode * glslVertexNode = vertexShaderNode->Get("glsl");
    if (!glslVertexNode)
    {
        SafeRelease(parser);
        return false;
    }
    
    YamlNode * fragmentShaderNode = rootNode->Get("fragmentShader");
    if (!fragmentShaderNode)
    {
        SafeRelease(parser);
        return false;
    }
    
    YamlNode * glslFragmentNode = fragmentShaderNode->Get("glsl");
    if (!glslFragmentNode)
    {
        SafeRelease(parser);
        return false;
    }
    
    
    String vertexShaderPath = glslVertexNode->AsString();
    uint32 fileSize;
    uint8 * bytes = FileSystem::Instance()->ReadFileContents(pathOnly + vertexShaderPath, fileSize);
    if (!CompileShader(&vertexShader, GL_VERTEX_SHADER, fileSize, (GLchar*)bytes))
    {
        Logger::Error("Failed to compile vertex shader: %s", vertexShaderPath.c_str());
        SafeRelease(parser);
        return false;
    }
    SafeDeleteArray(bytes);
    
    String fragmentShaderPath = glslFragmentNode->AsString();
    uint8 * fbytes = FileSystem::Instance()->ReadFileContents(pathOnly + fragmentShaderPath, fileSize);
    if (!CompileShader(&fragmentShader, GL_FRAGMENT_SHADER, fileSize, (GLchar*)fbytes))
    {
        Logger::Error("Failed to compile fragment shader: %s", fragmentShaderPath.c_str());
        SafeRelease(parser);
        return false;
    }
    SafeDeleteArray(fbytes);
    
    
    SafeRelease(parser);

    
    program = glCreateProgram();
    
    RENDER_VERIFY(glAttachShader(program, vertexShader));
    RENDER_VERIFY(glAttachShader(program, fragmentShader));
    
    if (!LinkProgram(program))
    {
        DeleteShaders();
        return false;
    }

    
    RENDER_VERIFY(glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &activeAttributes));
                  
    Logger::Debug("shader loaded: %s attributeCount: %d", pathname.c_str(), activeAttributes);

    char attributeName[512];
    attributeNames = new String[activeAttributes];
    for (int32 k = 0; k < activeAttributes; ++k)
    {
        GLint size;
        GLenum type;
        RENDER_VERIFY(glGetActiveAttrib(program, k, 512, 0, &size, &type, attributeName));
        attributeNames[k] = attributeName;
        
        int32 flagIndex = GetAttributeIndexByName(attributeName);
        vertexFormatAttribIndeces[flagIndex] = k;
        Logger::Debug("shader attr: %s size: %d type: %s", attributeName, size, VertexTypeStringFromEnum(type).c_str());
        if (vertexFormatAttribIndeces[k] != -1)
            Logger::Debug("shader attr matched: 0x%08x", (1 << flagIndex));
    }
 
    RENDER_VERIFY(glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &activeUniforms));
    
    uniformLocations = new GLint[activeUniforms];
    uniformIDs = new eUniform[activeUniforms];
    uniformNames = new String[activeUniforms];
    for (int32 k = 0; k < activeUniforms; ++k)
    {
        GLint size;
        GLenum type;
        RENDER_VERIFY(glGetActiveUniform(program, k, 512, 0, &size, &type, attributeName));
        eUniform uniform = GetUniformByName(attributeName);
        //Logger::Debug("shader uniform: %s size: %d type: %s", attributeName, size, VertexTypeStringFromEnum(type).c_str());
        uniformNames[k] = attributeName;
        uniformLocations[k] = glGetUniformLocation(program, uniformNames[k].c_str());
        uniformIDs[k] = uniform;
        Logger::Debug("shader known uniform: %s size: %d type: %s", uniformNames[k].c_str(), size, VertexTypeStringFromEnum(type).c_str());
    }
    
    return true;
}
    
    
Shader::~Shader()
{
    SafeDeleteArray(uniformNames);
    SafeDeleteArray(uniformIDs);
    SafeDeleteArray(uniformLocations);
    DeleteShaders();
}
    
void Shader::SetUniformValue(int32 uniformLocation, int32 value)
{
    glUniform1i(uniformLocation, value);
}

void Shader::SetUniformValue(int32 uniformLocation, const Vector3 & vector)
{
    glUniform3fv(uniformLocation, 1, &vector.x);
}
void Shader::SetUniformValue(int32 uniformLocation, const Vector4 & vector)
{
    glUniform4fv(uniformLocation, 1, &vector.x);
}

void Shader::SetUniformValue(int32 uniformLocation, const Matrix4 & matrix)
{
    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, matrix.data);
}
    
int32 Shader::GetAttributeIndex(eVertexFormat vertexFormat)
{
    return vertexFormatAttribIndeces[CountLeadingZeros(vertexFormat)];
}

void Shader::DeleteShaders()
{
    RENDER_VERIFY(glDetachShader(program, vertexShader));
    RENDER_VERIFY(glDetachShader(program, fragmentShader));
    RENDER_VERIFY(glDeleteShader(vertexShader));
    RENDER_VERIFY(glDeleteShader(fragmentShader));
    RENDER_VERIFY(glDeleteProgram(program));
}

/* Link a program with all currently attached shaders */
GLint Shader::LinkProgram(GLuint prog)
{
    GLint status;
    
    glLinkProgram(prog);
    
    GLint logLength;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        Logger::Debug("Program link log:\n%s", log);
        free(log);
    }
    
    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
        Logger::Debug("Failed to link program %d", prog);
    
    return status;
}
    
/* Create and compile a shader from the provided source(s) */
GLint Shader::CompileShader(GLuint *shader, GLenum type, GLint count, const GLchar * sources)
{
    GLint status;
    //const GLchar *sources;
        
    *shader = glCreateShader(type);				// create shader
    glShaderSource(*shader, 1, &sources, &count);	// set source code in the shader
    glCompileShader(*shader);					// compile shader
    
//#if defined(DEBUG)
    GLint logLength;
    glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetShaderInfoLog(*shader, logLength, &logLength, log);
        Logger::Debug("Shader compile log:\n%s", log);
        free(log);
    }
//#endif
    
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        Logger::Debug("Failed to compile shader:\n");
    }
    
    return status;
}
    
void Shader::Unbind()
{
    if (activeProgram != 0)
    {
        glUseProgram(0);
        activeProgram = 0;
    }
}
    
void Shader::Bind()
{
    if (activeProgram != program)
    {
        glUseProgram(program);
        activeProgram = program;
    }
    
    for (int32 k = 0; k < activeUniforms; ++k)
    {
        switch (uniformIDs[k])
        {
        case UNIFORM_MODEL_VIEW_PROJECTION_MATRIX:
            {    
                const Matrix4 & modelViewProj = RenderManager::Instance()->GetUniformMatrix(RenderManager::UNIFORM_MATRIX_MODELVIEWPROJECTION);
                glUniformMatrix4fv(uniformLocations[k], 1, GL_FALSE, modelViewProj.data);
                break;
            }
        case UNIFORM_COLOR:
            {
                const Color & c = RenderManager::Instance()->GetColor();
                glUniform4fv(uniformLocations[k], 1, &c.r);
                break;
            }  
        default:
            
            break;
        }
    }
    
}
    
int32 Shader::FindUniformLocationByName(const String & name)
{
    for (int32 k = 0; k < activeUniforms; ++k)
    {
        if (uniformNames[k] == name)
        {
            return uniformLocations[k];
        }
    }
    return -1;
}
    

#endif 



}



