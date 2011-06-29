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
Shader::Shader()
{
    vertexShader = 0;
    fragmentShader = 0;
    program = 0;
}
    
bool Shader::LoadFromYaml(const String & pathname)
{
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
    
    YamlNode * fragmentShaderNode = rootNode->Get("fragmentShader");
    if (!fragmentShaderNode)
    {
        SafeRelease(parser);
        return false;
    }
    
    String vertexShaderPath = vertexShaderNode->AsString();
    uint32 fileSize;
    uint8 * bytes = FileSystem::Instance()->ReadFileContents(vertexShaderPath, fileSize);
    if (!CompileShader(&vertexShader, GL_VERTEX_SHADER, 1, (GLchar*)bytes))
    {
        Logger::Error("Failed to compile vertex shader: %s", vertexShaderPath.c_str());
    }
    SafeDeleteArray(bytes);
    
    String fragmentShaderPath = fragmentShaderNode->AsString();
    uint8 * fbytes = FileSystem::Instance()->ReadFileContents(fragmentShaderPath, fileSize);
    if (!CompileShader(&fragmentShader, GL_VERTEX_SHADER, 1, (GLchar*)fbytes))
    {
        Logger::Error("Failed to compile fragment shader: %s", fragmentShaderPath.c_str());
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
    
    return true;
}
    
    
Shader::~Shader()
{
    DeleteShaders();
}


void Shader::DeleteShaders()
{
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteProgram(program);
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
GLint Shader::CompileShader(GLuint *shader, GLenum type, GLsizei count, const GLchar * sources)
{
    GLint status;
    //const GLchar *sources;
        
    *shader = glCreateShader(type);				// create shader
    glShaderSource(*shader, 1, &sources, NULL);	// set source code in the shader
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
#endif 



}



