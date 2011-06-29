/*
 *  PngImage.h
 *  texturepack
 *
 *  Created by Vitaliy Borodovsky on 10/28/08.
 *  Copyright 2008 DAVA Consulting, LLC. All rights reserved.
 *
 */
#ifndef __DAVAENGINE_PNGIMAGEEXT_H__
#define __DAVAENGINE_PNGIMAGEEXT_H__

#include "DAVAEngine.h"
using namespace DAVA;

class PngImageExt
{
public:
	PngImageExt();
	~PngImageExt();
	
	bool Create(int32 _width, int32 _height);
	
	bool Read(const char * filename);
	bool Write(const char * filename);
	
	void DrawImage(int sx, int sy, PngImageExt * image);
	void DrawImage(int sx, int sy, PngImageExt * image, const Rect2i & srcRect);

	void DrawRect(const Rect2i & rect, uint32 color);
	
	bool IsHorzLineOpaque(int y);
	bool IsVertLineOpaque(int x);
	
	void FindNonOpaqueRect(Rect2i & rect);
	
	uint8 * GetPixel(int32 x, int32 y) { return &data[(y * width + x) * 4]; };
	
	inline int32 GetWidth();
	inline int32 GetHeight();
private:	
	int32		width;
	int32		height;
	uint8  *	data;
};




int32 PngImageExt::GetWidth()
{
	return width;
}

int32 PngImageExt::GetHeight()
{
	return height;
}


#endif // __DAVAENGINE_PNGIMAGEEXT_H__
