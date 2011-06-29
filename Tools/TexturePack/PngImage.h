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
#ifndef __DAVA_PNG_IMAGE_H__
#define __DAVA_PNG_IMAGE_H__

#include "BaseTypes.h"
#include "BaseMath.h"


class PngImage
{
public:
	PngImage();
	~PngImage();
	
	bool Create(int32 _width, int32 _height);
	
	bool Read(const char * filename);
	bool Write(const char * filename);
	
	void DrawImage(int sx, int sy, PngImage * image);
	void DrawImage(int sx, int sy, PngImage * image, const Rect2i & srcRect);

	void DrawRect(const Log::Math::Rect2i & rect, uint32 color);
	
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




int32 PngImage::GetWidth()
{
	return width;
}

int32 PngImage::GetHeight()
{
	return height;
}


#endif // __PNG_IMAGE_H__
