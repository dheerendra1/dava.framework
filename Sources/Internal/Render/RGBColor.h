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
#ifndef __DAVAENGINE_RGBCOLOR_H__
#define __DAVAENGINE_RGBCOLOR_H__

#include "Base/BaseTypes.h"
#include "Base/BaseMath.h"

namespace DAVA
{

enum eColorFormat
{
	//! 16 bit color format
	ECF_A1R5G5B5 = 1,	

	//! 16 bit 565 color format.
	ECF_R5G6B5,

	//! 24 bit color, no alpha channel
	ECF_R8G8B8,

	//! 32 bit color format. 8 bits are used for every component:
	ECF_A8R8G8B8,

	//! 16 bit color format
	ECF_A4R4G4B4,


	//! \TODO: unit test all formats above this line
	//! \TODO: write loading of these formats
	//! \TODO: write saving of these formats
	//! ----------------------------------------------

	//! DXT1 compression texture format 
	ECF_DXT1,
	
	//! DXT2 compression texture format 
	ECF_DXT2,
	
	//! DXT3 compression texture format 
	ECF_DXT3,
	
	//! DXT4 compression texture format 
	ECF_DXT4,
	
	//! DXT5 compression texture format 
	ECF_DXT5,
	
	//! 16-bit float format using 16 bits for the red channel.
	ECF_R16F,	

	//! 32-bit float format using 16 bits for the red channel and 16 bits for the green channel.
	ECF_G16R16F,
	
	//! 64-bit float format using 16 bits for the each channel (alpha, blue, green, red).
	ECF_A16B16G16R16F
}; 

inline	static	uint32	ColorFormatSize(eColorFormat Format)
{
	switch(Format)
	{
	case ECF_A1R5G5B5:
	case ECF_R5G6B5:
	case ECF_A4R4G4B4:
		return 2;

	case ECF_R8G8B8:
		return 3;
	case ECF_A8R8G8B8:
		return 4;
	default:
		return 0;
	}
	return 0;
}

struct FractionalColor
{
	union
	{
		float32	color[4];
		struct  
		{
			float32 r, g, b, a;
		};
	};

};

struct RGBColor
{
	//! no init constructor to save speed
	RGBColor() {};

	RGBColor(uint8 _r, uint8 _g, uint8 _b, uint8 _a = 255) { r = _r; g = _g; b = _b; a = _a; };
	inline RGBColor(const Vector4 & color);
	inline RGBColor(const Vector3 & color);

	// Data of RGBColor type
	union
	{
		uint8	pcolor[4];			//	Color in ARGB Format
		uint32	color;
		struct
		{
			uint8 b,g,r,a;
		};
	};

	inline	uint16			Convert16(eColorFormat Format);
	inline	uint32			Convert2(eColorFormat Format);
	inline	void			ConvertFrom(eColorFormat Format, uint32 Value);
};


//! 
//! Information structure
//! Not used yet
//!
struct RGBColorMask
{
	eColorFormat	Format;

	uint32			RMask;		// Masks to get real pixel value
	uint32			GMask;		
	uint32			BMask;
	uint32			AMask;
	
	uint32			RShift;		// shifts used to convert value to uint8
	uint32			GShift;
	uint32			BShift;
	uint32			AShift;
		
};

// implementation of RGBColor

inline uint16 RGB16(uint32 r, uint32 g, uint32 b)
{
	return (uint16)( (uint16)((r>>3) & 0x1F)<<10) | (uint16)(((g>>3) & 0x1F)<<5) | (uint16)((b>>3) & 0x1F);
}

inline	uint16 RGBColor::Convert16(eColorFormat Format)
{
	switch(Format)
	{
	case ECF_A1R5G5B5:
		{
			//uint32 tmR = ((color>>16) & 0xff)>> 3;
			//uint32 tmG = ((color>>8)& 0xff) >> 3;
			//uint32 tmB = ((color)& 0xff) >> 3;

			return RGB16(color>>16, color>>8, color);//((((uint32)R>>3) & 0x1F)<<10) | ((((uint32)G>>3) & 0x1F)<<5) | (((uint32)B) & 0x1F );
		}
	default:
		return 0;
	}
	return 0;
}

inline	uint32 RGBColor::Convert2(eColorFormat Format)
{
	switch(Format)
	{
	case ECF_A1R5G5B5:
		{
			//uint32 tmR = ((color>>16) & 0xff)>> 3;
			//uint32 tmG = ((color>>8)& 0xff) >> 3;
			//uint32 tmB = ((color)& 0xff) >> 3;

			return RGB16(color>>16, color>>8, color);//((((uint32)R>>3) & 0x1F)<<10) | ((((uint32)G>>3) & 0x1F)<<5) | (((uint32)B) & 0x1F );
		}
	case ECF_R5G6B5:
		return (((r>>3) & 0x1F)<<11) | (((g>>2) & 0x3F)<<5) | ((b>>3) & 0x1F);
	case ECF_A4R4G4B4:
		return (uint32)(((b>>4) & 0xF)<<12) | ((((uint32)g>>4) & 0xF)<<8) | ((((uint32)r>>4) & 0xF)<<4) | (((uint32)a>>4) & 0xF);
	case ECF_R8G8B8:
		return color & 0xffffff;
	case ECF_A8R8G8B8:
		return color;		
	default:
		return 0;
	}
	return 0;
}

inline	void RGBColor::ConvertFrom(eColorFormat format, uint32 value)
{
	switch(format)
	{
	case ECF_A1R5G5B5:
		a = (uint8)(value >> 11) & 0x1;
		r = (uint8)(value >> 10) & 0x1F;
		g = (uint8)(value >> 5) & 0x1F;
		b = (uint8)(value) & 0x1F;
		return;
	case ECF_R5G6B5:
		r = (uint8)(value >> 11) & 0x1F;
		g = (uint8)(value >> 5) & 0x3F;
		b = (uint8)(value) & 0x1F;
		return;
	case ECF_A4R4G4B4:
		a = (uint8)(value >> 12) & 0xF;
		r = (uint8)(value >> 8) & 0xF;
		g = (uint8)(value >> 5) & 0xF;
		b = (uint8)(value) & 0xF;
		return;
	case ECF_R8G8B8:
		a = (uint8)0xff;
		r = (uint8)(value >> 16) & 0xFF;
		g = (uint8)(value >> 8) & 0xFF;
		b = (uint8)(value) & 0xFF;
		return;
	case ECF_A8R8G8B8:
		color = value;
		return;		
	default:
		return;
	}
}

inline RGBColor::RGBColor(const Vector4 & color)
{
	r = (uint8)(color.x * 255.0f);
	g = (uint8)(color.y * 255.0f);
	b = (uint8)(color.z * 255.0f);
	a = (uint8)(color.w * 255.0f);
}
inline RGBColor::RGBColor(const Vector3 & color)
{
	r = (uint8)(color.x * 255.0f);
	g = (uint8)(color.y * 255.0f);
	b = (uint8)(color.z * 255.0f);
	a = 255;
}



};	// end of namespace DAVA

#endif // __DAVAENGINE_RGBCOLOR_H__

