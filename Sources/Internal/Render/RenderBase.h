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
#ifndef __DAVAENGINE_RENDER_BASE_H__
#define __DAVAENGINE_RENDER_BASE_H__

#include "Base/BaseTypes.h"
#include "DAVAConfig.h"

/**
	\defgroup render Rendering abstraction
	\defgroup render_2d 2D Rendering
	\defgroup render_3d 3D Rendering
*/

/*
 Boroda: Я не знаю почему но <glew/gl.h> при включение из .mm файлов выдает ошибку как будто кто-то уже подключил 
 стандартный GL хотя в проекте нигде нет на него упоминаний. Есть подозрение что какой-то конфликт с внутренностями ObjC
 Как обойти пока непонятно - я сделал этот хедер чтобы включать его только там где это реально надо. 
 
 Раньше это лежало в BaseTypes.h
 */

#ifdef __DAVAENGINE_IPHONE__
	#define __DAVAENGINE_OPENGL__
	#import <OpenGLES/ES1/gl.h>
	#import <OpenGLES/ES1/glext.h>
    #import <OpenGLES/ES2/gl.h>
    #import <OpenGLES/ES2/glext.h>
#elif defined(__DAVAENGINE_MACOS__)
	#define __DAVAENGINE_OPENGL__
	//	#include <GL/glew.h>
	#include <OpenGL/OpenGL.h>
	//	#include <GLUT/glut.h>
#elif defined(__DAVAENGINE_WIN32__)
 	#if defined(__DAVAENGINE_RENDER_AUTOCONFIG__)
 	#define __DAVAENGINE_DIRECTX9__
//	#define __DAVAENGINE_OPENGL__
 	#endif 

	#if defined(__DAVAENGINE_OPENGL__)
		#include <GL/glew.h>
		#include <direct.h>
	#elif defined(__DAVAENGINE_DIRECTX9__)
		// TODO: right now Sprite code do not compiles without GL types. Fix that. 
		#include <GL/gl.h>
		// #include <direct.h>
		
		#define D3D_DEBUG_INFO
		#include <d3d9.h>
		#include <dxerr.h>
#endif 

#endif 


#if defined(__DAVAENGINE_OPENGL__)
#include "Render/OGLHelpers.h"
#elif defined(__DAVAENGINE_DIRECTX9__)
#include "Render/D3D9Helpers.h"
#endif 


namespace DAVA
{

enum eBlendMode
{
	BLEND_NONE = 0,				// blend mode not defined
	BLEND_ZERO,
	BLEND_ONE,
	BLEND_DST_COLOR,
	BLEND_ONE_MINUS_DST_COLOR,
	BLEND_SRC_ALPHA,
	BLEND_ONE_MINUS_SRC_ALPHA,
	BLEND_DST_ALPHA,
	BLEND_ONE_MINUS_DST_ALPHA,
	BLEND_SRC_ALPHA_SATURATE,

	BLEND_MODE_COUNT,
};


enum eVertexDataType
{
	TYPE_FLOAT = 0,
	//TYPE_FIXED,

	TYPE_COUNT
};

enum eIndexFormat
{
	EIF_16 = 0x0,
	EIF_32 = 0x1,
};


enum ePrimitiveType
{
	PRIMITIVETYPE_POINTLIST = 0,
	PRIMITIVETYPE_LINELIST,
	PRIMITIVETYPE_LINESTRIP,
	PRIMITIVETYPE_TRIANGLELIST,
	PRIMITIVETYPE_TRIANGLESTRIP,
	PRIMITIVETYPE_TRIANGLEFAN,

	PRIMITIVETYPE_COUNT
};


class RenderGuard
{
public:	
	RenderGuard();
	~RenderGuard();
	
	void LowLevelRenderCall();
	
	bool wrongCall;
};
	
};

#if defined(__DAVAENGINE_DEBUG__)
#define RENDER_GUARD RenderGuard renderGuard;
#else
#define RENDER_GUARD
#endif




#endif // __DAVAENGINE_RENDER_BASE_H__