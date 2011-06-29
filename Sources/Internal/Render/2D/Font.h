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
#ifndef __DAVAENGINE_FONT_H__
#define __DAVAENGINE_FONT_H__

#include "Base/BaseTypes.h"
#include "Base/BaseMath.h"
#include "Base/Singleton.h"
#include "Base/EventDispatcher.h"

namespace DAVA
{
/**
	\defgroup fonts Fonts in engine
 */

class Texture;

/** 
	\ingroup fonts
	\brief This class is base class for all fonts.
 
	This class implements all base functions to handle properties of the font.
	All fonts in system should be inherited from this class.
 */
class Font : public BaseObject
{
public:
	enum eFontType 
	{
			FONT_TYPE_FT = 0
		,	FONT_TYPE_GRAPHICAL
	};
	
	Font();
	~Font();

	static void SetDPI(int32 dpi);
	static int32 GetDPI();
	
	virtual void SetColor(float32 r, float32 g, float32 b, float32 a);
	virtual void SetShadowColor(float32 r, float32 g, float32 b, float32 a);
	virtual void SetColor(const Color & color);
	virtual const Color & GetColor() const;
	virtual void SetShadowColor(const Color & color);
	virtual const Color & GetShadowColor() const;
	
	virtual void SetShadowOffset(const Vector2 & offset);
	virtual void SetShadowOffset(float32 x, float32 y);
	virtual const Vector2 & GetShadowOffset() const;
	
	virtual void SetSize(float32 size);
	virtual float32	GetSize();
	
	virtual void SetVerticalSpacing(int32 verticalSpacing);
	virtual int32 GetVerticalSpacing();
	
	// text splitting functions
	bool SplitTextToStrings(const WideString & text, const Vector2 & targetRectSize, Vector<WideString> & resultVector);

	// pure virtual functions that have to be overloaded in implementations 
	virtual Size2i GetStringSize(const WideString & str, Vector<int32> *charSizes = 0) = 0;
	virtual bool IsCharAvaliable(char16 ch) = 0;
	virtual uint32 GetFontHeight() = 0;
	virtual int32  GetAscender() = 0;
	virtual int32  GetDescender() = 0;
	virtual Font * Clone() = 0;
	virtual bool IsEqual(Font *font);

	// functions to overload rendering
	virtual bool IsTextSupportsSoftwareRendering() { return false; };
	virtual Size2i DrawStringToBuffer(void * buffer, int32 bufWidth, int32 bufHeight, int32 offsetX, int32 offsetY, int32 justifyWidth, const WideString & str, bool contentScaleIncluded = false) { return  Size2i(0, 0); };
	
	virtual bool IsTextSupportsHardwareRendering() { return false; };
	virtual Size2i DrawString(float32 offsetX, float32 offsetY, const WideString & str, int32 justifyWidth = 0) { return Size2i(0, 0);};
	
	
	
protected:
	static int32 globalFontDPI;
	
	float32	size;
	Color color;
	Color shadowColor;
	Vector2	shadowOffset;
	int32 verticalSpacing;
	
	eFontType fontType;
};
		
};

#endif // __DAVAENGINE_FONT_H__
