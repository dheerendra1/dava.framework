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
        \todo Add possibility to change premultiplication & make possibility to change format on fly
=====================================================================================*/
#ifndef __DAVAENGINE_TEXTURE_H__
#define __DAVAENGINE_TEXTURE_H__

#include "Base/BaseTypes.h"
#include "Render/RenderBase.h"
#include "Base/BaseMath.h"
#include "Base/BaseObject.h"
#include "Render/RenderResource.h"

namespace DAVA
{
/**
	\ingroup render
	\brief Class that represents texture objects in our SDK. 
	This class support the following formats: RGBA8888, RGB565, RGBA4444, A8 on all platforms. 
	For iOS it also support compressed PVR formats. (PVR2 and PVR4)
 */
class Texture : public RenderResource
{
public:
	enum PixelFormat
	{
		FORMAT_INVALID = 0,
		FORMAT_RGBA8888 = 1,		
		FORMAT_RGB565,				 
		FORMAT_RGBA4444,			
		FORMAT_A8,
		FORMAT_PVR4,
		FORMAT_PVR2,
		
		FORMAT_CLOSEST = 256
	};
	
	enum TextureWrap
	{
		WRAP_CLAMP_TO_EDGE = 0,
		WRAP_CLAMP,
		WRAP_REPEAT,
	};
	
#ifdef __DAVAENGINE_IPHONE__
	static const int MAX_WIDTH = 1024;
	static const int MIN_WIDTH = 8;
	static const int MAX_HEIGHT = 1024;
	static const int MIN_HEIGHT = 8;
#else
	static const int MAX_WIDTH = 4096;
	static const int MIN_WIDTH = 8;
	static const int MAX_HEIGHT = 4096;
	static const int MIN_HEIGHT = 8;
#endif 

	// Main constructurs
	
	// return size of pixel format in bits 
	static int32 GetPixelFormatSize(PixelFormat format);
	
	static Texture * CreateFromData(PixelFormat format, uint8 * data, uint32 width, uint32 height);
	static Texture * CreateTextFromData(PixelFormat format, uint8 * data, uint32 width, uint32 height, const char * addInfo = 0);
	static Texture * CreateFromFile(const String & pathName);
	static Texture * CreateFBO(uint32 w, uint32 h, PixelFormat format);
	
	void LoadMipMapFromFile(int32 level, const String & pathName);

	//static void 
	static void SetDefaultRGBAFormat(PixelFormat format);
	static PixelFormat GetDefaultRGBAFormat();
	
	virtual int32 Release();

	static void	DumpTextures();

	inline int32 GetWidth() { return width; }
	inline int32 GetHeight() { return height; }
	
	static void EnableMipmapGeneration();
	static void DisableMipmapGeneration();
	void GenerateMipmaps();
	
	void TexImage(int32 level, uint32 width, uint32 height, void * _data);

	void SetWrapMode(TextureWrap wrapS, TextureWrap wrapT);
	
	void UsePvrMipmaps();

public:							// properties for fast access

#if defined(__DAVAENGINE_OPENGL__)
	uint32		id;				// OpenGL id for texture
#elif defined(__DAVAENGINE_DIRECTX9__)
	static LPDIRECT3DTEXTURE9 CreateTextureNative(Vector2 & size, PixelFormat & format, bool isRenderTarget, int32 flags);
	void SetAsHardwareCursor(const Vector2 & hotSpot);
	LPDIRECT3DTEXTURE9 id;
	LPDIRECT3DTEXTURE9 saveTexture;
	bool		 renderTargetModified;

	virtual void SaveToSystemMemory();
	virtual void Lost();
	virtual void Invalidate();
	
#endif 

	String		relativePathname;
	String		debugInfo;
	uint32		width;			// texture width 
	uint32		height;			// texture height
//	uint32		imageWidth;		// image width
//	uint32		imageHeight;	// image height
#if defined(__DAVAENGINE_OPENGL__)
	uint32		fboID;			// id of frame buffer object
#endif
	PixelFormat format;			// texture format 
	bool		isRenderTarget;

	void SetDebugInfo(const String & _debugInfo);
	
	//TODO: move to private
	static Map<String, Texture*> textureMap;	
private:
	static Texture * Get(const String & name);
	static Texture * CreateFromPNG(const String & pathName);// , PixelFormat format = SELECT_CLOSEST_FORMAT, bool premultipliedAlpha = false);
	static Texture * CreateFromPVR(const String & pathName);// , PixelFormat format = SELECT_CLOSEST_FORMAT);

	
	static Texture * UnpackPVRData(uint8 * data, uint32 dataSize);

	static PixelFormat defaultRGBAFormat;
	static bool	isMipmapGenerationEnabled;
	Texture();
	virtual ~Texture();
};



};

#endif // __DAVAENGINE_TEXTUREGLES_H__

