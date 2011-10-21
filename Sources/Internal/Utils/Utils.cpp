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

#include "Utils/Utils.h"
#include "Utils/StringFormat.h"

#include "FileSystem/YamlParser.h"

namespace DAVA
{

	
WideString WcharToWString(const wchar_t *s)
{
//	Logger::Info("[WcharToWString] s = %s", s);

	WideString temp;
	if(s)
	{

		wchar_t c = 0;
		int size = 0;
		do 
		{
			c = s[size];
			++size;
//			Logger::Info("[WcharToWString] c = %d, size = %d", c, size);
			
			if(c)
				temp.append(1, c);
		} while (c);
	}

	return temp; 
}

bool IsEqual(const WideString& s1, const WideString& s2)
{
	char16 *p1 = (char16 *)s1.c_str();
	char16 *p2 = (char16 *)s2.c_str();

	while(*p1 && *p2)
	{
		if(*p1 != *p2)
			return false;

		++p1;
		++p2;
	}

	return (*p1 == *p2);
}


int32 lastBindedTexture = 0;
int32 GetSavedTextureID()
{
	int32 saveId = 0;

#if defined(__DAVAENGINE_ANDROID__)
	saveId = lastBindedTexture;
#else //#if defined(__DAVAENGINE_ANDROID__)
	RENDER_VERIFY(glGetIntegerv(GL_TEXTURE_BINDING_2D, &saveId));
#endif //#if defined(__DAVAENGINE_ANDROID__)

	return saveId;
}

void BindTexture(int32 tId)
{
	if(0 != tId)
	{
// 		RENDER_VERIFY(glBindTexture(GL_TEXTURE_2D, tId));
		glBindTexture(GL_TEXTURE_2D, tId);

		GLenum err = glGetError();
		if (err != GL_NO_ERROR)
			Logger::Debug("%s file:%s line:%d gl failed with errorcode: 0x%08x", "glBindTexture(GL_TEXTURE_2D, tId)", __FILE__, __LINE__, err);

		lastBindedTexture = tId;
	}
}

int32 lastBindedFBO = 0;
int32 GetSavedFBO()
{
	int32 saveFBO = 0;

#if defined(__DAVAENGINE_IPHONE__)
	RENDER_VERIFY(glGetIntegerv(GL_FRAMEBUFFER_BINDING_OES, &saveFBO));
#elif defined(__DAVAENGINE_ANDROID__)
	saveFBO = lastBindedFBO;
#else //Non ES platforms
	RENDER_VERIFY(glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &saveFBO));
#endif //PLATFORMS

	return saveFBO;
}
void BindFBO(int32 fbo)
{
//	if(0 != fbo)
	{
#if defined(__DAVAENGINE_IPHONE__) || defined(__DAVAENGINE_ANDROID__)
		glBindFramebufferOES(GL_FRAMEBUFFER_OES, fbo);	// Unbind the FBO for now
#else //Non ES platforms
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);	// Unbind the FBO for now
#endif //PLATFORMS

		GLenum err = glGetError();
		if (err != GL_NO_ERROR)
			Logger::Debug("%s file:%s line:%d gl failed with errorcode: 0x%08x", "glBindTexture(GL_TEXTURE_2D, tId)", __FILE__, __LINE__, err);


		lastBindedFBO = fbo;
	}
}



void Split(const String & inputString, const String & delims, Vector<String> & tokens)
{
	// Skip delims at beginning, find start of first token
	String::size_type lastPos = inputString.find_first_not_of(delims, 0);
	// Find next delimiter @ end of token
	String::size_type pos     = inputString.find_first_of(delims, lastPos);
	// output vector
	// Vector<String> tokens;
	
	while (String::npos != pos || String::npos != lastPos)
	{
		// Found a token, add it to the vector.
		tokens.push_back(inputString.substr(lastPos, pos - lastPos));
		// Skip delims.  Note the "not_of". this is beginning of token
		lastPos = inputString.find_first_not_of(delims, pos);
		// Find next delimiter at end of token.
		pos     = inputString.find_first_of(delims, lastPos);
	}	
}

	
/* Set a generic reader. */
int read_handler(void *ext, unsigned char *buffer, size_t size, size_t *length)
{
	YamlParser::YamlDataHolder * holder = (YamlParser::YamlDataHolder*)ext;
	int32 sizeToWrite = Min((uint32)size, holder->fileSize-holder->dataOffset);

//    Logger::Debug("[read_handler] sizeToWrite = %d, holder = %p, buffer = %p", sizeToWrite, holder, buffer);
//    if(holder)
//    {
//        Logger::Debug("[read_handler] holder->data = %p", holder->data);
//    }
    
	memcpy(buffer, holder->data, sizeToWrite);
	
    *length = sizeToWrite;

	holder->dataOffset += sizeToWrite;

	return 1;
}

	
}; // end of namespace DAVA
