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
#include "Image.h"

namespace DAVA
{

struct ConvertRGBA8888toRGBA4444
{
	inline void operator()(const uint32 * input, uint16 *output)
	{
		uint32 pixel = *input;
		uint32 a = ((pixel >> 24) & 0xFF) >> 4;
		uint32 r = ((pixel >> 16) & 0xFF) >> 4;
		uint32 g = ((pixel >> 8) & 0xFF) >> 4;
		uint32 b = (pixel & 0xFF) >> 4;
		*output = ((b) << 12) | (g << 8) | (r << 4) | a;
	}
};

struct UnpackRGBA8888
{
	inline void operator()(const uint32 * input, uint32 & r, uint32 & g, uint32 & b, uint32 & a)
	{
		uint32 pixel = *input;
		a = ((pixel >> 24) & 0xFF);
		r = ((pixel >> 16) & 0xFF);
		g = ((pixel >> 8) & 0xFF);
		b = (pixel & 0xFF);
	}
};

struct PackRGBA8888
{
	inline void operator()(uint32 r, uint32 g, uint32 b, uint32 a, uint32 * output)
	{
		*output = ((a) << 24) | (r << 16) | (g << 8) | b;
	}
};

struct UnpackRGBA4444
{
	inline void operator()(const uint16 * input, uint32 & r, uint32 & g, uint32 & b, uint32 & a)
	{
		uint16 pixel = *input;
		a = ((pixel >> 12) & 0xF);
		r = ((pixel >> 8) & 0xF);
		g = ((pixel >> 4) & 0xF);
		b = (pixel & 0xF);
	}
};

struct PackRGBA4444
{
	inline void operator()(uint32 r, uint32 g, uint32 b, uint32 a, uint16 * output)
	{
		*output = ((b >> 4) << 12) | ((g >> 4) << 8) | ((r >> 4) << 4) | (a >> 4);
	}
};

template<class TYPE_IN, class TYPE_OUT, typename CONVERT_FUNC>
class ConvertDirect
{
public:
    void operator()(const void * inData, uint32 inWidth, uint32 inHeight, uint32 inPitch,
                    void * outData, uint32 outWidth, uint32 outHeight, uint32 outPitch)
    {
		CONVERT_FUNC func;
        const uint8 * readPtr = reinterpret_cast<const uint8*>(inData);
        uint8 * writePtr = reinterpret_cast<uint8*>(outData);
        
        for (uint32 y = 0; y < inHeight; ++y)
        {
            const TYPE_IN * readPtrLine = reinterpret_cast<const TYPE_IN*>(readPtr);
            TYPE_OUT * writePtrLine = reinterpret_cast<TYPE_OUT*>(writePtr);
            for (uint32 x = 0; x < inWidth; ++x)
            {
                func(readPtrLine, writePtrLine);
                readPtrLine++;
                writePtrLine++;
            }
            readPtr += inPitch; 
            writePtr += outPitch;
        }
    };
};

template<class TYPE_IN, class TYPE_OUT, typename UNPACK_FUNC, typename PACK_FUNC>
class ConvertDownscaleTwiceBillinear
{
public:
	void operator()(const void * inData, uint32 inWidth, uint32 inHeight, uint32 inPitch,
		void * outData, uint32 outWidth, uint32 outHeight, uint32 outPitch)
	{
		UNPACK_FUNC unpackFunc;
		PACK_FUNC packFunc;
		const uint8 * readPtr = reinterpret_cast<const uint8*>(inData);
		uint8 * writePtr = reinterpret_cast<uint8*>(outData);

		for (uint32 y = 0; y < outHeight; ++y)
		{
			const TYPE_IN * readPtrLine = reinterpret_cast<const TYPE_IN*>(readPtr);
			TYPE_OUT * writePtrLine = reinterpret_cast<TYPE_OUT*>(writePtr);
			
			for (uint32 x = 0; x < outWidth; ++x)
			{
				uint32 r00, r01, r10, r11;
				uint32 g00, g01, g10, g11;
				uint32 b00, b01, b10, b11;
				uint32 a00, a01, a10, a11;

				unpackFunc(readPtrLine, r00, g00, b00, a00);
				unpackFunc(readPtrLine + 1, r01, g01, b01, a01);
				unpackFunc(readPtrLine + inWidth, r10, g10, b10, a10);
				unpackFunc(readPtrLine + inWidth + 1, r11, g11, b11, a11);
		
				uint32 r = (r00 + r01 + r10 + r11) >> 2;
				uint32 g = (g00 + g01 + g10 + g11) >> 2;
				uint32 b = (b00 + b01 + b10 + b11) >> 2;
				uint32 a = (a00 + a01 + a10 + a11) >> 2;
				
				packFunc(r, g, b, a, writePtrLine);

				readPtrLine += 2;
				writePtrLine++;
			}
			readPtr += inPitch * 2; 
			writePtr += outPitch;
		}
	};
};


class ImageConvert
{
public:

	static void DownscaleTwiceBillinear(	Image::PixelFormat inFormat,
												Image::PixelFormat outFormat,
												const void * inData, uint32 inWidth, uint32 inHeight, uint32 inPitch,
												void * outData, uint32 outWidth, uint32 outHeight, uint32 outPitch)
	{
		if ((inFormat == Image::FORMAT_RGBA8888) && (outFormat == Image::FORMAT_RGBA8888))
		{
			ConvertDownscaleTwiceBillinear<uint32, uint32, UnpackRGBA8888, PackRGBA8888> convert;
			convert(inData, inWidth, inHeight, inPitch, outData, outWidth, outHeight, outPitch);
		}else if ((inFormat == Image::FORMAT_RGBA8888) && (outFormat == Image::FORMAT_RGBA4444))
		{
			ConvertDownscaleTwiceBillinear<uint32, uint16, UnpackRGBA8888, PackRGBA4444> convert;
			convert(inData, inWidth, inHeight, inPitch, outData, outWidth, outHeight, outPitch);
		}else if ((inFormat == Image::FORMAT_RGBA4444) && (outFormat == Image::FORMAT_RGBA8888))
		{
			ConvertDownscaleTwiceBillinear<uint16, uint32, UnpackRGBA4444, PackRGBA8888> convert;
			convert(inData, inWidth, inHeight, inPitch, outData, outWidth, outHeight, outPitch);
		}else
		{
			DVASSERT(0 && "Convert function not implemented");
		}
	}

	Image* DownscaleTwiceBillinear(Image * source)
	{
		if (source->GetPixelFormat() == Image::FORMAT_RGBA8888)
		{
			Image * destination = Image::Create(source->GetWidth() / 2, source->GetHeight() / 2, source->GetPixelFormat());
			if (destination)
			{
				ConvertDownscaleTwiceBillinear<uint32, uint32, UnpackRGBA8888, PackRGBA8888> convertFunc;
				convertFunc(source->GetData(), source->GetWidth(), source->GetHeight(), source->GetWidth() * Image::GetFormatSize(source->GetPixelFormat()),
					destination->GetData(), destination->GetWidth(), destination->GetHeight(), destination->GetWidth() * Image::GetFormatSize(destination->GetPixelFormat()));
			}
			return destination;
		}
		return 0;
	}
};

};
