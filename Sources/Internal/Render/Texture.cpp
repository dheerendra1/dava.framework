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
#include "Render/RenderBase.h"
#include "Render/Texture.h"
#include "Utils/Utils.h"
#include "FileSystem/Logger.h"
#include "Debug/DVAssert.h"
#include "Utils/Utils.h"
#include "Render/RenderManager.h"
#include "Utils/StringFormat.h"
#include "Platform/SystemTimer.h"
#include "FileSystem/File.h"
#include "Render/D3D9Helpers.h"
#include "Render/ImageConvert.h"
//#include "LibPngHelpers.h"

#if defined(__DAVAENGINE_IPHONE__) 
#include <CoreGraphics/CoreGraphics.h>
#include <CoreFoundation/CoreFoundation.h>
#elif defined(__DAVAENGINE_MACOS__)
#include <ApplicationServices/ApplicationServices.h>
#endif 

#include "Render/Image.h"
#include "Render/OGLHelpers.h"

namespace DAVA 
{
	
class TextureMemoryUsageInfo
{
public:
	TextureMemoryUsageInfo()
	{
		pvrTexturesMemoryUsed = 0;
		texturesMemoryUsed = 0;
		fboMemoryUsed = 0;
	}
	
	void AllocPVRTexture(int size)
	{
		pvrTexturesMemoryUsed += size;
	}
	
	void ReleasePVRTexture(int size)
	{
		pvrTexturesMemoryUsed -= size;
	}
	
	void AllocTexture(int size)
	{
		texturesMemoryUsed += size;
	}
	
	void ReleaseTexture(int size)
	{
		texturesMemoryUsed -= size;
	}
	
	void AllocFBOTexture(int size)
	{
		fboMemoryUsed += size;
	}
	
	void ReleaseFBOTexture(int size)
	{
		fboMemoryUsed -= size;
	}
	
	// STATISTICS
	int pvrTexturesMemoryUsed;
	int texturesMemoryUsed;
	int	fboMemoryUsed;
};

static TextureMemoryUsageInfo texMemoryUsageInfo;
	
Map<String, Texture*> Texture::textureMap;
static int32 textureFboCounter = 0;

// Main constructurs
int32 Texture::GetPixelFormatSize(PixelFormat format)
{
	switch(format)
	{
	case FORMAT_RGBA8888:
		return 32;
	case FORMAT_RGB565:
		return 16;
	case FORMAT_RGBA4444:
		return 16;
	case FORMAT_A8:
		return 8;
	case FORMAT_PVR4:
		return 4;
	case FORMAT_PVR2:
		return 2;
	default:
		return 0;
	};
	return 0;
}
    
const char * Texture::GetPixelFormatString(PixelFormat format)
{
    switch(format)
	{
        case FORMAT_RGBA8888:
            return "FORMAT_RGBA8888";
        case FORMAT_RGB565:
            return "FORMAT_RGB565";
        case FORMAT_RGBA4444:
            return "FORMAT_RGBA4444";
        case FORMAT_A8:
            return "FORMAT_A8";
        case FORMAT_PVR4:
            return "FORMAT_PVR4";
        case FORMAT_PVR2:
            return "FORMAT_PVR2";
        default:
            return "WRONG FORMAT";
	};
    return "WRONG FORMAT";
}

Texture * Texture::Get(const String & pathName)
{
	Texture * texture = NULL;
	Map<String, Texture *>::iterator it;
	it = textureMap.find(pathName);
	if (it != textureMap.end())
	{
		texture = it->second;
		texture->Retain();
		return texture;
	}
	return 0;
}

Texture::Texture()
:	id(0)
,	width(0)
,	height(0)
,	format(FORMAT_INVALID)
,	isRenderTarget(false)
{
#ifdef __DAVAENGINE_DIRECTX9__
	saveTexture = 0;
	renderTargetModified = false;
    renderTargetAutosave = true;
#endif 

#ifdef __DAVAENGINE_OPENGL__
	fboID = -1;
#endif
}

Texture::~Texture()
{
	RenderManager::Instance()->LockNonMain();
	if(RenderManager::Instance()->GetTexture() == this)
	{//to avoid drawing deleted textures
		RenderManager::Instance()->SetTexture(0);
	}

#if defined(__DAVAENGINE_OPENGL__)

	if(fboID != (uint32)-1)
	{
#if defined(__DAVAENGINE_IPHONE__)
		RENDER_VERIFY(glDeleteFramebuffersOES(1, &fboID));
#else
		RENDER_VERIFY(glDeleteFramebuffersEXT(1, &fboID));
#endif
	}
	
	if(id)
	{
		RENDER_VERIFY(glDeleteTextures(1, &id));
	}
#elif defined(__DAVAENGINE_DIRECTX9__)
	D3DSafeRelease(id);
	D3DSafeRelease(saveTexture);
#endif 
	RenderManager::Instance()->UnlockNonMain();
}

Texture * Texture::CreateTextFromData(PixelFormat format, uint8 * data, uint32 width, uint32 height, const char * addInfo)
{
	RenderManager::Instance()->LockNonMain();
	Texture * tx = CreateFromData(format, data, width, height);
	RenderManager::Instance()->UnlockNonMain();
	
	if (!addInfo)
		tx->relativePathname = Format("Text texture %d", textureFboCounter);
	else 
		tx->relativePathname = Format("Text texture %d info:%s", textureFboCounter, addInfo);

	textureFboCounter++;
	textureMap[tx->relativePathname] = tx;
	return tx;
}
	
void Texture::TexImage(int32 level, uint32 width, uint32 height, const void * _data)
{
#if defined(__DAVAENGINE_OPENGL__)

	int saveId;
	RENDER_VERIFY(glGetIntegerv(GL_TEXTURE_BINDING_2D, &saveId));
	RENDER_VERIFY(glBindTexture(GL_TEXTURE_2D, id));
	
	switch(format) 
	{
		case Texture::FORMAT_RGBA8888:
			RENDER_VERIFY(glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _data));
			break;
		case Texture::FORMAT_RGB565:
			RENDER_VERIFY(glTexImage2D(GL_TEXTURE_2D, level, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, _data));
			break;
		case Texture::FORMAT_A8:
			RENDER_VERIFY(glTexImage2D(GL_TEXTURE_2D, level, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, _data));
			break;
		case Texture::FORMAT_RGBA4444:
			RENDER_VERIFY(glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, _data));
			break;
		default:
			return;
	}
	
	if (saveId != 0)
		RENDER_VERIFY(glBindTexture(GL_TEXTURE_2D, saveId));

#elif defined(__DAVAENGINE_DIRECTX9__)
	if (!id)
		return;

	D3DLOCKED_RECT rect;
	HRESULT hr = id->LockRect(level, &rect, 0, 0);
	if (FAILED(hr))
	{
		Logger::Error("[TextureDX9] Could not lock DirectX9 Texture.");
		return;
	}

	// \todo instead of hardcoding transformations, use ImageConvert.
	int32 pixelSizeInBits = GetPixelFormatSize(format);
	if (format ==  Texture::FORMAT_RGBA8888)
	{
		//int32 pitchInBytes = 

		uint8 * destBits = (uint8*)rect.pBits;
		uint8 * sourceBits = (uint8*)_data;
		for (uint32 h = 0; h < height * width; ++h)
		{
// 			uint32 r = sourceBits[0];
// 			uint32 g = sourceBits[1];
// 			uint32 b = sourceBits[2];
// 			uint32 a = sourceBits[3];
// 			
// 		// 		r = ((r * a) >> 8);
// 		// 		g = ((g * a) >> 8);
// 		// 		b = ((b * a) >> 8);
// 
// 			destBits[0] = (uint8)b; //sourceBits[3];
// 			destBits[1] = (uint8)g; //sourceBits[0];
// 			destBits[2] = (uint8)r;//sourceBits[1];
// 			destBits[3] = (uint8)a;

            destBits[0] = sourceBits[2];
            destBits[1] = sourceBits[1];
            destBits[2] = sourceBits[0];
            destBits[3] = sourceBits[3];

			destBits += 4;
			sourceBits += 4;
		}
	}else 
	{
		// pixel conversion from R4G4B4A4 (OpenGL format) => A4R4G4B4 (DirectX format)
		uint16 * destBits = (uint16*)rect.pBits;
		uint16 * sourceBits = (uint16*)_data;
		for (uint32 h = 0; h < height * width; ++h)
		{
			uint32 rgba = sourceBits[0];

			destBits[0] = ((rgba & 0xF) << 12) | (rgba >> 4);

			destBits ++;
			sourceBits ++;
		}	
	}

	id->UnlockRect(level);
#endif 
}

Texture * Texture::CreateFromData(PixelFormat _format, const uint8 *_data, uint32 _width, uint32 _height)
{
	Texture * texture = new Texture();
	if (!texture)return 0;
	
	texture->width = _width;
	texture->height = _height;
	texture->format = _format;

#if defined(__DAVAENGINE_OPENGL__)
	for (int i = 0; i < 10; i++) 
	{
		RENDER_VERIFY(glGenTextures(1, &texture->id));
		if(texture->id != 0)
		{
			break;
		}
		Logger::Error("TEXTURE %d GENERATE ERROR: %d", i, glGetError());
	}	

	int saveId;
	RENDER_VERIFY(glGetIntegerv(GL_TEXTURE_BINDING_2D, &saveId));
	RENDER_VERIFY(glBindTexture(GL_TEXTURE_2D, texture->id));
	RENDER_VERIFY(glPixelStorei( GL_UNPACK_ALIGNMENT, 1 ));
		
	switch(texture->format) 
	{
		case Texture::FORMAT_RGBA8888:
			RENDER_VERIFY(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _data));
			break;
		case Texture::FORMAT_RGB565:
			RENDER_VERIFY(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->width, texture->height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, _data));
			break;
		case Texture::FORMAT_A8:
			RENDER_VERIFY(glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, texture->width, texture->height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, _data));
			break;
		case Texture::FORMAT_RGBA4444:
			RENDER_VERIFY(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, _data));
			break;
		default:
			SafeRelease(texture);
			return 0;
	}

/*	const int width = 16;
	const int height = 16;
	
	uint8 data[width * height * 4];
	for (int w = 0; w < width; ++w)
		for (int h = 0; h < height; ++h)
		{
			int k = h * width + w;
			data[k * 4 + 0] = 0;
			data[k * 4 + 1] = (float)w / (float)width * 255.0f;
			data[k * 4 + 2] = (float)h / (float)height * 255.0f;
			data[k * 4 + 3] = 255;
		}
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,  data);
*/
	GLint wrapMode = 0;
#if defined(__DAVAENGINE_IPHONE__)
	wrapMode = GL_CLAMP_TO_EDGE;
#else 
	wrapMode = GL_CLAMP;
#endif 
	if (isMipmapGenerationEnabled)
	{
		RENDER_VERIFY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode));
		RENDER_VERIFY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode));
		texture->GenerateMipmaps();
	}else
	{
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		RENDER_VERIFY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode));
		RENDER_VERIFY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode));
		RENDER_VERIFY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		RENDER_VERIFY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	}
	
	if (saveId != 0)
		RENDER_VERIFY(glBindTexture(GL_TEXTURE_2D, saveId));
	
#elif defined(__DAVAENGINE_DIRECTX9__)

	texture->id = CreateTextureNative(Vector2((float32)_width, (float32)_height), texture->format, false, 0);
	texture->TexImage(0, _width, _height, _data);

	// allocate only 2 levels, and reuse buffers for generation of every mipmap level
	uint8 *mipMapData = new uint8[(_width / 2) * (_height / 2) * GetPixelFormatSize(texture->format) / 8];
	uint8 *mipMapData2 = new uint8[(_width / 4) * (_height / 4) * GetPixelFormatSize(texture->format) / 8];

	const uint8 * prevMipData = _data;
	uint8 * currentMipData = mipMapData;

	int32 mipMapWidth = _width / 2;
	int32 mipMapHeight = _height / 2;

	for (uint32 i = 1; i < texture->id->GetLevelCount(); ++i)
	{
		ImageConvert::DownscaleTwiceBillinear((Image::PixelFormat)texture->format, (Image::PixelFormat)texture->format, 
			prevMipData, mipMapWidth << 1, mipMapHeight << 1, (mipMapWidth << 1) * GetPixelFormatSize(texture->format) / 8,
			currentMipData, mipMapWidth, mipMapHeight, mipMapWidth * GetPixelFormatSize(texture->format) / 8);

		texture->TexImage(i, mipMapWidth, mipMapHeight, currentMipData);
		
		mipMapWidth  >>= 1;
		mipMapHeight >>= 1;
		
		prevMipData = currentMipData;
		currentMipData = (i & 1) ? (mipMapData2) : (mipMapData); 
	}

	SafeDeleteArray(mipMapData2);
	SafeDeleteArray(mipMapData);

#endif 
	return texture;
}		
	
void Texture::SetWrapMode(TextureWrap wrapS, TextureWrap wrapT)
{
    RenderManager::Instance()->LockNonMain();
#if defined(__DAVAENGINE_OPENGL__)
	int saveId;
	RENDER_VERIFY(glGetIntegerv(GL_TEXTURE_BINDING_2D, &saveId));
	RENDER_VERIFY(glBindTexture(GL_TEXTURE_2D, id));
	
	GLint glWrapS = 0;
	switch(wrapS)
	{
	case WRAP_CLAMP_TO_EDGE:
		glWrapS = GL_CLAMP_TO_EDGE;
		break;
	case WRAP_CLAMP:
		glWrapS = GL_CLAMP_TO_EDGE;
		break;
	case WRAP_REPEAT:
		glWrapS = GL_REPEAT;
		break;
	};
	RENDER_VERIFY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glWrapS));
	
	GLint glWrapT = 0;
	switch(wrapT)
	{
		case WRAP_CLAMP_TO_EDGE:
			glWrapT = GL_CLAMP_TO_EDGE;
			break;
		case WRAP_CLAMP:
			glWrapT = GL_CLAMP_TO_EDGE;
			break;
		case WRAP_REPEAT:
			glWrapT = GL_REPEAT;
			break;
	};
	RENDER_VERIFY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glWrapT));

	if (saveId != 0)
		RENDER_VERIFY(glBindTexture(GL_TEXTURE_2D, saveId));
#elif defined(__DAVAENGINE_DIRECTX9____)
	


#endif 
    RenderManager::Instance()->UnlockNonMain();
}
	
bool Texture::isMipmapGenerationEnabled = false;
	
void Texture::EnableMipmapGeneration()
{
	isMipmapGenerationEnabled = true;
}

void Texture::DisableMipmapGeneration()
{
	isMipmapGenerationEnabled = false;
}

void Texture::GenerateMipmaps()
{
	RenderManager::Instance()->LockNonMain();
#if defined(__DAVAENGINE_OPENGL__)

	int saveId;
	RENDER_VERIFY(glGetIntegerv(GL_TEXTURE_BINDING_2D, &saveId));
	RENDER_VERIFY(glBindTexture(GL_TEXTURE_2D, id));
	
#if defined(__DAVAENGINE_IPHONE__)
	// definitelly works for the iPhone
	RENDER_VERIFY(glGenerateMipmapOES(GL_TEXTURE_2D));
	RENDER_VERIFY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	RENDER_VERIFY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

#else
	RENDER_VERIFY(glGenerateMipmapEXT(GL_TEXTURE_2D));

	RENDER_VERIFY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	RENDER_VERIFY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
#endif // #if defined(__DAVAENGINE_IPHONE__)

	if (saveId != 0)
		RENDER_VERIFY(glBindTexture(GL_TEXTURE_2D, saveId));
	
#elif defined(__DAVAENGINE_DIRECTX9__)

#endif // #if defined(__DAVAENGINE_OPENGL__)
	RenderManager::Instance()->UnlockNonMain();
}

void Texture::UsePvrMipmaps()
{
	RenderManager::Instance()->LockNonMain();
#if defined(__DAVAENGINE_OPENGL__)
	int saveId;
	RENDER_VERIFY(glGetIntegerv(GL_TEXTURE_BINDING_2D, &saveId));
	RENDER_VERIFY(glBindTexture(GL_TEXTURE_2D, id));

	RENDER_VERIFY(glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE));
	
	RENDER_VERIFY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	RENDER_VERIFY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	
	if (saveId != 0)
		RENDER_VERIFY(glBindTexture(GL_TEXTURE_2D, saveId));

#elif defined(__DAVAENGINE_DIRECTX9__)



#endif
    RenderManager::Instance()->UnlockNonMain();

}
	
	
void Texture::LoadMipMapFromFile(int32 level, const String & pathName)
{
	Image * image = Image::CreateFromFile(pathName);
	if ((Texture::PixelFormat)image->GetPixelFormat() != format)
	{
		Logger::Error("Texture::LoadMipMapFromFile - format of file texture different from this texture"); 
		SafeRelease(image);
		return;
	}
	TexImage(level, image->GetWidth(), image->GetHeight(), image->GetData());
	SafeRelease(image);
}
	

Texture * Texture::CreateFromPNG(const String & pathName)
{
	Texture * texture = Texture::Get(pathName);
	if (texture)
	{
		//Logger::Debug("[Texture] Texture from cache: %s", pathName.c_str());
		return texture;
	}
	
	Image * image = Image::CreateFromFile(pathName);
	if (!image)
	{
		Logger::Debug("[Texture] Failed to load image from: %s", pathName.c_str());
		return 0;
	}
	
	RenderManager::Instance()->LockNonMain();
	texture = Texture::CreateFromData((Texture::PixelFormat)image->GetPixelFormat(), image->GetData(), image->GetWidth(), image->GetHeight());
	RenderManager::Instance()->UnlockNonMain();
	texture->relativePathname = pathName;
	
	if (texture)
		textureMap[texture->relativePathname] = texture;
	SafeRelease(image);
	
	return texture;
}		

#if defined(__DAVAENGINE_IPHONE__)
Texture * Texture::CreateFromPVR(const String & pathName)
{
	uint64_t timeCreateFromPVR = SystemTimer::Instance()->AbsoluteMS();

	Texture * texture = Texture::Get(pathName);
	if (texture)return texture;
	
	File * fp = File::Create(pathName, File::OPEN|File::READ);
	if (!fp)
	{
		Logger::Error("Failed to open PVR texture: %s", pathName.c_str());
		return 0;
	}
	uint32 fileSize = fp->GetSize();
	uint8 * bytes = new uint8[fileSize];
	uint32 dataRead = fp->Read(bytes, fileSize);

	if (dataRead != fileSize)
	{
		Logger::Error("Failed to read data from PVR texture file: %s", pathName.c_str());
		return 0;
	}
	Texture * newTexture = UnpackPVRData(bytes, fileSize);
	if (!newTexture)
	{
		Logger::Error("Failed to parse PVR texture: %s", pathName.c_str());
	}
	SafeDeleteArray(bytes);
	SafeRelease(fp);

	if (newTexture)
	{
		newTexture->relativePathname = pathName;
		textureMap[newTexture->relativePathname] = newTexture;
	}
	timeCreateFromPVR = SystemTimer::Instance()->AbsoluteMS() - timeCreateFromPVR;
	Logger::Debug("TexturePVR: t: %lld", timeCreateFromPVR);

	return newTexture;
}
	
#define PVR_TEXTURE_FLAG_TYPE_MASK	0xff
	
static char gPVRTexIdentifier[5] = "PVR!";
	
enum
{
	kPVRTextureFlagTypePVRTC_2 = 24,
	kPVRTextureFlagTypePVRTC_4
};
	

typedef struct _PVRTexHeader
{
	uint32_t headerLength;
	uint32_t height;
	uint32_t width;
	uint32_t numMipmaps;
	uint32_t flags;
	uint32_t dataLength;
	uint32_t bpp;
	uint32_t bitmaskRed;
	uint32_t bitmaskGreen;
	uint32_t bitmaskBlue;
	uint32_t bitmaskAlpha;
	uint32_t pvrTag;
	uint32_t numSurfs;
} PVRTexHeader;

	
struct TextureFrame
{
	uint8* data;
	uint32 size;
};

static const uint32 PVRTC2_MIN_TEXWIDTH		= 16;
static const uint32 PVRTC2_MIN_TEXHEIGHT		= 8;
static const uint32 PVRTC4_MIN_TEXWIDTH		= 8;
static const uint32 PVRTC4_MIN_TEXHEIGHT		= 8;
static const uint32 ETC_MIN_TEXWIDTH			= 4;
static const uint32 ETC_MIN_TEXHEIGHT		= 4;
	
	
Texture * Texture::UnpackPVRData(uint8 * data, uint32 fileDataSize)
{
	bool success = false;
	PVRTexHeader *header = NULL;
	uint32 flags, pvrTag;
	uint32 dataLength = 0, dataOffset = 0, dataSize = 0;
//	uint32 blockSize = 0, widthBlocks = 0, heightBlocks = 0;
	int32 origWidth = 0, origHeight = 0, width = 0, height = 0;// bpp = 4;
	uint8 *bytes = 0;
	uint32_t formatFlags;
	
	header = (PVRTexHeader *)data;
	
	pvrTag = CFSwapInt32LittleToHost(header->pvrTag);
	
	if ((uint8)(gPVRTexIdentifier[0]) != ((pvrTag >>  0) & 0xff) ||
		(uint8)gPVRTexIdentifier[1] != ((pvrTag >>  8) & 0xff) ||
		(uint8)gPVRTexIdentifier[2] != ((pvrTag >> 16) & 0xff) ||
		(uint8)gPVRTexIdentifier[3] != ((pvrTag >> 24) & 0xff))
	{
		return FALSE;
	}
	
	flags = CFSwapInt32LittleToHost(header->flags);
	formatFlags = flags & PVR_TEXTURE_FLAG_TYPE_MASK;
	
	List<TextureFrame> imageList;
	
	GLenum	internalFormat = 0;
	bool hasAlpha;
	
	if (formatFlags == kPVRTextureFlagTypePVRTC_4 || formatFlags == kPVRTextureFlagTypePVRTC_2)
	{
		if (formatFlags == kPVRTextureFlagTypePVRTC_4)internalFormat = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
		else if (formatFlags == kPVRTextureFlagTypePVRTC_2)internalFormat = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
				
		origWidth = width = CFSwapInt32LittleToHost(header->width);
		origHeight = height = CFSwapInt32LittleToHost(header->height);
				
		if (CFSwapInt32LittleToHost(header->bitmaskAlpha))
		{
			hasAlpha = true;
		}
		else
		{
			hasAlpha = false;
#if 1
			if (formatFlags == kPVRTextureFlagTypePVRTC_4)
			{
				internalFormat = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
			}
			else
			if (formatFlags == kPVRTextureFlagTypePVRTC_2)
			{
				internalFormat = GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
			}
#endif
		}
		
		
						
		dataLength = CFSwapInt32LittleToHost(header->dataLength);
						
		bytes = ((uint8 *)data) + header->headerLength;//sizeof(PVRTexHeader);
						
		// Calculate the data size for each texture level and respect the minimum number of blocks
		while (dataOffset < dataLength)
		{
#if 0
			if (formatFlags == kPVRTextureFlagTypePVRTC_4)
			{
				blockSize = 4 * 4; // Pixel by pixel block size for 4bpp
				widthBlocks = width / 4;
				heightBlocks = height / 4;
				bpp = 4;
			}
			else
			{
				blockSize = 8 * 4; // Pixel by pixel block size for 2bpp
				widthBlocks = width / 8;
				heightBlocks = height / 4;
				bpp = 2;
			}
			
			// Clamp to minimum number of blocks
			if (widthBlocks < 2)widthBlocks = 2;
			if (heightBlocks < 2)heightBlocks = 2;
					
			dataSize = widthBlocks * heightBlocks * ((blockSize  * bpp) / 8);
#else
			if (formatFlags == kPVRTextureFlagTypePVRTC_4)
			{
				dataSize = (Max(width, (int32)PVRTC4_MIN_TEXWIDTH) * Max(height, (int32)PVRTC4_MIN_TEXHEIGHT) * header->bpp) / 8;
			}
			else 
			if (formatFlags == kPVRTextureFlagTypePVRTC_2)
			{
				dataSize = (Max(width, (int32)PVRTC2_MIN_TEXWIDTH) * Max(height, (int32)PVRTC2_MIN_TEXHEIGHT) * header->bpp) / 8;
			}

			
#endif
			
			//[_imageData addObject:[NSData dataWithBytes:bytes+dataOffset length:dataSize]];
			TextureFrame frame;
			frame.data = bytes + dataOffset;
			frame.size = dataSize;
			imageList.push_back(frame);
			
			dataOffset += dataSize;
			
			width = Max(width >> 1, 1);
			height = Max(height >> 1, 1);
		}
		
		success = TRUE;
	}
	
	Texture * texture = 0;
	
	if (success)
	{
		texture = new Texture();
		if (!texture)return 0;
	
		width = origWidth;
		height = origHeight;

		texture->width = width;
		texture->height = height;
		if (formatFlags == kPVRTextureFlagTypePVRTC_4)texture->format = Texture::FORMAT_PVR4;
		else if (formatFlags == kPVRTextureFlagTypePVRTC_2)texture->format = Texture::FORMAT_PVR2;
		
		RENDER_VERIFY(glGenTextures(1, &texture->id));
		
		int saveId;
		RENDER_VERIFY(glGetIntegerv(GL_TEXTURE_BINDING_2D, &saveId));
		RENDER_VERIFY(glBindTexture(GL_TEXTURE_2D, texture->id));
		
		int32 i = 0;
		for (List<TextureFrame>::iterator it = imageList.begin(); it != imageList.end(); ++it)
		{
			TextureFrame & frame = *it;
			RENDER_VERIFY(glCompressedTexImage2D(GL_TEXTURE_2D, i++, internalFormat, width, height, 0, frame.size, frame.data));
			width = Max(width >> 1, 1);
			height = Max(height >> 1, 1);
		}
		
		RENDER_VERIFY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		RENDER_VERIFY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		
		if (saveId != 0)
			RENDER_VERIFY(glBindTexture(GL_TEXTURE_2D, saveId));
	}
	return texture;
}
#endif
	
/*- (bool)createGLTexture
{
	int width = _width;
	int height = _height;
	NSData *data;
	GLenum err;
	
	if ([_imageData count] > 0)
	{
		if (_name != 0)
			glDeleteTextures(1, &_name);
			
		glGenTextures(1, &_name);
		glBindTexture(GL_TEXTURE_2D, _name);
		}
	
	for (int i=0; i < [_imageData count]; i++)
	{
		data = [_imageData objectAtIndex:i];
		glCompressedTexImage2D(GL_TEXTURE_2D, i, _internalFormat, width, height, 0, [data length], [data bytes]);
		
		err = glGetError();
		if (err != GL_NO_ERROR)
		{
			NSLog(@"Error uploading compressed texture level: %d. glError: 0x%04X", i, err);
			return FALSE;
		}
		
		int allocSize = 0;
		if (_internalFormat == GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG)
			allocSize = width * height / 2;
			if (_internalFormat == GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG)
				allocSize = width * height / 4;
				_memoryUsage += allocSize;
				
				width = MAX(width >> 1, 1);
				height = MAX(height >> 1, 1);
				
				
				}
	
	texMemoryUsageInfo.AllocPVRTexture(_memoryUsage);
	
	[_imageData removeAllObjects];
	
	return TRUE;
}*/
	


Texture * Texture::CreateFromFile(const String & pathName)
{
	// TODO: add check that pathName 
	String extension = pathName.substr(pathName.length() - 4, 4); 

	if (extension == String(".png"))
		return CreateFromPNG(pathName);
#if defined(__DAVAENGINE_IPHONE__)
	else if (extension == String(".pvr"))
		return CreateFromPVR(pathName);
#endif
	return 0;
}
	
int32 Texture::Release()
{
	if(GetRetainCount() == 1)
	{
		textureMap.erase(relativePathname);
	}
	return BaseObject::Release();
}
	
Texture * Texture::CreateFBO(uint32 w, uint32 h, PixelFormat format)
{
	int i;
	int dx = w;
	if(dx < 16)
	{
		dx = 16;
	}
	else if((dx != 1) && (dx & (dx - 1))) 
	{
		i = 1;
		while(i < dx)
			i *= 2;
		dx = i;
	}
	int dy = h;
	if(dy < 16)
	{
		dy = 16;
	}
	else if((dy != 1) && (dy & (dy - 1))) 
	{
		i = 1;
		while(i < dy)
			i *= 2;
		dy = i;
	}
	
	RenderManager::Instance()->LockNonMain();
	
#if defined(__DAVAENGINE_OPENGL__)

	GLint saveTexture;
	GLint saveFBO;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &saveTexture);
#if defined(__DAVAENGINE_IPHONE__)
	glGetIntegerv(GL_FRAMEBUFFER_BINDING_OES, &saveFBO);
#else
	glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &saveFBO);
#endif	
	Texture *tx = Texture::CreateFromData(format, NULL, dx, dy);
	DVASSERT(tx);
	 
	// Now setup a texture to render to
	RENDER_VERIFY(glBindTexture(GL_TEXTURE_2D, tx->id));
	RENDER_VERIFY(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	RENDER_VERIFY(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	RENDER_VERIFY(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	RENDER_VERIFY(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	
		// Setup our FBO
#if defined(__DAVAENGINE_IPHONE__)
	RENDER_VERIFY(glGenFramebuffersOES(1, &tx->fboID));
	RENDER_VERIFY(glBindFramebufferOES(GL_FRAMEBUFFER_OES, tx->fboID));
		
	// And attach it to the FBO so we can render to it
	RENDER_VERIFY(glFramebufferTexture2DOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_TEXTURE_2D, tx->id, 0));
		
	GLenum status = glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES);
	if(status != GL_FRAMEBUFFER_COMPLETE_OES)
	{
		Logger::Error("glCheckFramebufferStatusOES: %d", status);
	}
#else
#if defined(__DAVAENGINE_WIN32__)    
	if(GLEW_EXT_framebuffer_object)
	{
#endif        
		RENDER_VERIFY(glGenFramebuffersEXT(1, &tx->fboID));
		RENDER_VERIFY(glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, tx->fboID));
	
		// And attach it to the FBO so we can render to it
		RENDER_VERIFY(glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, tx->id, 0));
	
		GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		if(status != GL_FRAMEBUFFER_COMPLETE_EXT)
		{
			Logger::Error("[Texture::CreateFBO] glCheckFramebufferStatusEXT: %d", status);
		}
#if defined(__DAVAENGINE_WIN32__)         
	}
	else
	{
		Logger::Error("[Texture::CreateFBO] GL_EXT_framebuffer_object not supported");
	}
#endif    
#endif
				
#if defined(__DAVAENGINE_IPHONE__) 
	RENDER_VERIFY(glBindFramebufferOES(GL_FRAMEBUFFER_OES, saveFBO));	// Unbind the FBO for now
#else 
	RENDER_VERIFY(glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, saveFBO));	// Unbind the FBO for now
#endif
	RENDER_VERIFY(glBindTexture(GL_TEXTURE_2D, saveTexture));
#elif defined(__DAVAENGINE_DIRECTX9__)
	
	// TODO: Create FBO
	Texture * tx = new Texture();
	if (!tx)return 0;

	tx->width = dx;
	tx->height = dy;
	tx->format = format;

	tx->id = CreateTextureNative(Vector2((float32)tx->width, (float32)tx->height), tx->format, true, 0);

#endif 

	RenderManager::Instance()->UnlockNonMain();


	tx->isRenderTarget = true;
	tx->relativePathname = Format("FBO texture %d", textureFboCounter);
	textureMap[tx->relativePathname] = tx;
	
	textureFboCounter++;
	
	return tx;
}

	
void Texture::DumpTextures()
{
	int32 allocSize = 0;
	int32 cnt = 0;
	Logger::Info("============================================================");
	Logger::Info("--------------- Currently allocated textures ---------------");
	for(Map<String, Texture *>::iterator it = textureMap.begin(); it != textureMap.end(); ++it)
	{
		Texture *t = it->second;
		Logger::Info("%s with id %d (%dx%d) retainCount: %d debug: %s format: %s", t->relativePathname.c_str(), t->id, t->width, t->height, t->GetRetainCount(), t->debugInfo.c_str(), GetPixelFormatString(t->format));
		cnt++;
		switch (t->format) 
		{
			case FORMAT_RGBA8888:
				allocSize += t->width * t->height * 4;
				break;
			case FORMAT_RGBA4444:
				allocSize += t->width * t->height * 2;
				break;
			case FORMAT_RGB565:
				allocSize += t->width * t->height * 2;
				break;
			case FORMAT_A8:
				allocSize += t->width * t->height;
				break;
			default:
				break;
		}
	}
	Logger::Info("      Total allocated textures %d    memory size %d", cnt, allocSize);
	Logger::Info("============================================================");
}
	
	
Texture::PixelFormat Texture::defaultRGBAFormat = Texture::FORMAT_RGBA8888;

void Texture::SetDefaultRGBAFormat(PixelFormat format)
{
	defaultRGBAFormat = format;
}

Texture::PixelFormat Texture::GetDefaultRGBAFormat()
{
	return defaultRGBAFormat;
}

void Texture::SetDebugInfo(const String & _debugInfo)
{
#if defined(__DAVAENGINE_DEBUG__)
	debugInfo = _debugInfo;	
#endif
}

	
};





