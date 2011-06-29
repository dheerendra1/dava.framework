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
#include "Render/Image.h"
#include "FileSystem/Logger.h"
#include "Platform/SystemTimer.h"
#include "Utils/Utils.h"
#include "Render/LibPngHelpers.h"
#include "FileSystem/File.h"
#include "FileSystem/FileSystem.h"

#if defined(__DAVAENGINE_IPHONE__) 
#include <CoreGraphics/CoreGraphics.h>
#include <CoreFoundation/CoreFoundation.h>
#elif defined(__DAVAENGINE_MACOS__)
#include <ApplicationServices/ApplicationServices.h>
#endif 

namespace DAVA 
{
	
Image::Image()
:	data(0)
,	width(0)
,	height(0)
,	format(FORMAT_RGB565)
,	isAlphaPremultiplied(false)
{
}

Image::~Image()
{
	SafeDeleteArray(data);
}

Image * Image::Create(int32 width, int32 height, PixelFormat format)
{
	Image * image = new Image();
	image->width = width;
	image->height = height;
	image->format = format;
	
	switch(format)
	{
		case FORMAT_RGBA8888:
			image->data = new uint8[width * height * 4];
			break;
		case FORMAT_RGB565: 
			image->data = new uint8[width * height * 2];
			break;
		case FORMAT_RGBA4444:
			image->data = new uint8[width * height * 2];
			break;
		default:
			Logger::Error("Image::Create trying to create image with wrong format");
			break;
	}
	return image;
}

#if defined(__DAVAENGINE_MACOS__) || defined(__DAVAENGINE_IPHONE__)
Vector2 Image::GetImageSize(const String & pathName)
{
	
	CGImageRef image;
    CGDataProviderRef provider;
	
	File *inFile = File::Create(pathName, File::READ|File::OPEN);
	uint32 fileSize = inFile->GetSize();
	uint8 * bytes = new uint8[fileSize];
	uint32 dataRead = inFile->Read(bytes, fileSize);
	SafeRelease(inFile);
	if(dataRead != fileSize)
	{
		return Vector2();
	}
	
	
	provider = CGDataProviderCreateWithData(NULL, bytes, fileSize, NULL);
    image = CGImageCreateWithPNGDataProvider (provider, NULL, false, kCGRenderingIntentDefault);
	if (!image)
	{
		image = CGImageCreateWithJPEGDataProvider (provider, NULL, false, kCGRenderingIntentDefault);
	}
	CGDataProviderRelease (provider);
	
	if(image == 0) 
	{
		SafeDeleteArray(bytes);
		
		return Vector2();
	}
	
	Vector2 size = Vector2((float32)CGImageGetWidth(image), (float32) CGImageGetHeight(image));
	CGImageRelease (image);
	SafeDeleteArray(bytes);
	return size;
}


Image * Image::CreateFromFile(const String & pathName)
{
	int32					width, height;
	CGContextRef			context = 0;
	uint8*					data = 0;
	CGColorSpaceRef			colorSpace;
	uint8*					tempData;
	unsigned int*			inPixel32;
	unsigned short*			outPixel16;
	bool					hasAlpha;
	CGImageAlphaInfo		info;
	CGAffineTransform		transform;
	CGSize					imageSize;
	Image::PixelFormat		pixelFormat;
//	bool					sizeToFit = false;
	
	CGImageRef image;
    CGDataProviderRef provider;
	
//	Logger::Debug("				Loading Image: %s", pathName.c_str());
	File *inFile = File::Create(pathName, File::READ|File::OPEN);
	if (!inFile)return 0;
	
	
	uint32 fileSize = inFile->GetSize();
	uint8 * bytes = new uint8[fileSize];
	uint32 dataRead = inFile->Read(bytes, fileSize);
	SafeRelease(inFile);
	if(dataRead != fileSize)
	{
		return 0;
	}
	
	
	provider = CGDataProviderCreateWithData(NULL, bytes, fileSize, NULL);
    image = CGImageCreateWithPNGDataProvider (provider, NULL, false, kCGRenderingIntentDefault);
	if (!image)
	{
		image = CGImageCreateWithJPEGDataProvider (provider, NULL, false, kCGRenderingIntentDefault);
	}
	CGDataProviderRelease (provider);
	
	if(image == 0) 
	{
		SafeDeleteArray(bytes);

		return 0;
	}
	
	info = CGImageGetAlphaInfo(image);
	hasAlpha = ((info == kCGImageAlphaPremultipliedLast) || (info == kCGImageAlphaPremultipliedFirst) || (info == kCGImageAlphaLast) || (info == kCGImageAlphaFirst) ? true : false);
	if(CGImageGetColorSpace(image)) 
	{
		if(hasAlpha)pixelFormat = Image::FORMAT_RGBA8888;
		else pixelFormat = Image::FORMAT_RGB565;
	} else 
		pixelFormat = Image::FORMAT_A8;
	
	imageSize = CGSizeMake(CGImageGetWidth(image), CGImageGetHeight(image));
	transform = CGAffineTransformIdentity;
//	Logger::Debug("				Image size: %f x %f", imageSize.width, imageSize.height);

	width = imageSize.width;
//	int i;	
//	if((width != 1) && (width & (width - 1))) 
//	{
//		i = 1;
//		while((sizeToFit ? 2 * i : i) < width)
//			i *= 2;
//		width = i;
//	}
	height = imageSize.height;
//	if((height != 1) && (height & (height - 1))) 
//	{
//		i = 1;
//		while((sizeToFit ? 2 * i : i) < height)
//			i *= 2;
//		height = i;
//	}
//	while((width > MAX_WIDTH) || (height > MAX_HEIGHT)) 
//	{
//		width /= 2;
//		height /= 2;
//		transform = CGAffineTransformScale(transform, 0.5, 0.5);
//		imageSize.width *= 0.5;
//		imageSize.height *= 0.5;
//	}
	switch(pixelFormat) 
	{		
		case Image::FORMAT_RGBA8888:
		case Image::FORMAT_RGBA4444:
			colorSpace = CGColorSpaceCreateDeviceRGB();
			data = new uint8[height * width * 4];
			context = CGBitmapContextCreate(data, width, height, 8, 4 * width, colorSpace, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big );
			CGColorSpaceRelease(colorSpace);
			break;
		case Image::FORMAT_RGB565:
			colorSpace = CGColorSpaceCreateDeviceRGB();
			data = new uint8[height * width * 4];
			context = CGBitmapContextCreate(data, width, height, 8, 4 * width, colorSpace, kCGImageAlphaNoneSkipLast | kCGBitmapByteOrder32Big);
			CGColorSpaceRelease(colorSpace);
			break;
			
		case Image::FORMAT_A8:
			data = new uint8[height * width];
			context = CGBitmapContextCreate(data, width, height, 8, width, NULL, kCGImageAlphaOnly);
			break;				
		default:
			return 0;
	}
	
	
	CGContextClearRect(context, CGRectMake(0, 0, width, height));
	CGContextTranslateCTM(context, 0, height - imageSize.height);
	
	if(!CGAffineTransformIsIdentity(transform))
		CGContextConcatCTM(context, transform);
	
	CGContextDrawImage(context, CGRectMake(0, 0, CGImageGetWidth(image), CGImageGetHeight(image)), image);
	
	//Convert "RRRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA" to "RRRRRGGGGGGBBBBB"
	if(pixelFormat == Image::FORMAT_RGB565) 
	{
		tempData = new uint8[height * width * 2];
		inPixel32 = (unsigned int*)data;
		outPixel16 = (unsigned short*)tempData;
		for(int32 i = 0; i < width * height; ++i, ++inPixel32)
			*outPixel16++ = ((((*inPixel32 >> 0) & 0xFF) >> 3) << 11) | ((((*inPixel32 >> 8) & 0xFF) >> 2) << 5) | ((((*inPixel32 >> 16) & 0xFF) >> 3) << 0);
		SafeDeleteArray(data);
		data = tempData;
	}
	
	/*if(pixelFormat == Texture::FORMAT_RGBA8888) 
	 {
	 //		unsigned char * inAlphaData = (unsigned char *)alphaData;
	 unsigned int * inOutPixel32 = (unsigned int*)data;
	 for(i = 0; i < width * height; ++i)
	 {
	 unsigned int pixel = *inOutPixel32;
	 //pixel = pixel & (0xFFFFFFFF);
	 
	 unsigned int a = (pixel >> 24) & 0xFF;
	 unsigned int r = (pixel >> 16) & 0xFF;
	 unsigned int g = (pixel >> 8) & 0xFF;
	 unsigned int b = pixel & 0xFF;
	 
	 if (a != 0)
	 {
	 r = r * 255 / a;
	 g = g * 255 / a;
	 b = b * 255 / a;
	 }
	 *inOutPixel32 = ((a) << 24) | (r << 16) | (g << 8) | b;
	 inOutPixel32++;
	 //	*inOutPixel32 = ((*inAlphaData) << 24) | pixel;
	 //	unsigned int a = *inAlphaData;
	 //	inAlphaData++;
	 }
	 }*/
	
	if(pixelFormat == Image::FORMAT_RGBA4444) 
	{
		tempData = new uint8[height * width * 2];
		inPixel32 = (unsigned int*)data;
		outPixel16 = (unsigned short*)tempData;
		
		/*for(i = 0; i < width * height; ++i, ++inPixel32)
		 *outPixel16++ = ((((*inPixel32 >> 0) & 0xFF) >> 3) << 11) | ((((*inPixel32 >> 8) & 0xFF) >> 2) << 5) | ((((*inPixel32 >> 16) & 0xFF) >> 3) << 0);
		 */
		
		for(int32 i = 0; i < width * height; ++i)
		{
			uint32 pixel = *inPixel32;
			//pixel = pixel & (0xFFFFFFFF);
			
			uint32 a = (pixel >> 24) & 0xFF;
			uint32 r = (pixel >> 16) & 0xFF;
			uint32 g = (pixel >> 8) & 0xFF;
			uint32 b = pixel & 0xFF;
			
			/* disable premultiplication
			 if (a != 0)
			 {
			 r = r * 255 / a;
			 g = g * 255 / a;
			 b = b * 255 / a;
			 }*/
			r >>= 4;
			g >>= 4;
			b >>= 4;
			a >>= 4;
			
			*outPixel16 = ((b) << 12) | (g << 8) | (r << 4) | a;
			outPixel16 ++;
			inPixel32++;
		}
		
		SafeDeleteArray(data);
		data = tempData;
	}

	CGContextRelease(context);
	CGImageRelease (image);

	SafeDeleteArray(bytes);

	Image * davaImage = new Image();
	davaImage->width = width;
	davaImage->height = height;
	davaImage->data = (uint8*)data;
	davaImage->format = pixelFormat;
	return davaImage;
}
#elif defined(__DAVAENGINE_WIN32__)


Image * Image::CreateFromFile(const String & pathName)
{
	Image * davaImage = new Image();
	davaImage->format = FORMAT_RGBA8888;
	if (1 != LibPngWrapper::ReadPngFile(FileSystem::Instance()->SystemPathForFrameworkPath(pathName).c_str(), &davaImage->width, &davaImage->height, &davaImage->data))
	{
		//Logger::Error("Failed to open png file: %s", pathName.c_str());
		SafeRelease(davaImage);
		return 0;
	}

	if(davaImage->format == Image::FORMAT_RGBA8888) 
	{
		unsigned int * inOutPixel32 = (unsigned int*)davaImage->data;
		for(int i = 0; i < davaImage->width * davaImage->height; ++i)
		{
			unsigned int pixel = *inOutPixel32;
			//pixel = pixel & (0xFFFFFFFF);

			unsigned int a = (pixel >> 24) & 0xFF;
			unsigned int r = (pixel >> 16) & 0xFF;
			unsigned int g = (pixel >> 8) & 0xFF;
			unsigned int b = pixel & 0xFF;

			//if (a != 0)
			{
				r = r * a / 255;
				g = g * a / 255;
				b = b * a / 255;
			}
			*inOutPixel32 = ((a) << 24) | (r << 16) | (g << 8) | b;
			inOutPixel32++;
			//	*inOutPixel32 = ((*inAlphaData) << 24) | pixel;
			//	unsigned int a = *inAlphaData;
			//	inAlphaData++;
		}
	}
	return davaImage;
};

#endif //	


void Image::Save(const String & filename)
{
#if defined(__DAVAENGINE_WIN32__)
	DVASSERT(format == FORMAT_RGBA8888);
	LibPngWrapper::WritePngFile(filename.c_str(), width, height, data);
#else
	
	
#endif
}


};
