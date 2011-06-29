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
#ifndef __DAVAENGINE_GRAPHICSFONT_H__
#define __DAVAENGINE_GRAPHICSFONT_H__

#include "Base/BaseTypes.h"
#include "Base/BaseMath.h"
#include "Base/Singleton.h"
#include "Base/EventDispatcher.h"
#include "Render/2D/Font.h"
#include "Render/2D/Sprite.h"

namespace DAVA
{
	
class GraphicsFontDefinition;
class GraphicsFont : public Font
{
public:
	static GraphicsFont * Create(const String & fontDefName, const String & spriteName);
	
	virtual void	SetSize(float32 size);
	virtual Size2i	GetStringSize(const WideString & str, Vector<int32> *charSizes = NULL);
	virtual bool	IsCharAvaliable(char16 ch);
	virtual uint32	GetFontHeight();
	virtual int32	GetAscender();
	virtual int32	GetDescender();
	
	virtual bool	IsTextSupportsHardwareRendering();
	virtual Size2i	DrawString(float32 x, float32 y, const WideString & string, int32 justifyWidth = 0);
	
	virtual Font	* Clone();

	
protected:
	GraphicsFont();
	virtual ~GraphicsFont();
	
	float32 GetDistanceFromAtoB(int32 aIndex, int32 bIndex);

	Sprite * fontSprite;
	GraphicsFontDefinition * fdef;
	float32 fontScaleCoeff;
};
		
class GraphicsFontDefinition : public BaseObject
{
public:
	GraphicsFontDefinition();
	~GraphicsFontDefinition();
	
	float32 fontAscent;		// in points
	float32 fontDescent;	// in points
	float32 fontLeading;	// in points
	float32 fontXHeight;	// in points
	uint32	fontHeight;		// in points
	float32 charLeftRightPadding;	// in points
	float32 charTopBottomPadding;	// in points
	
	int32 tableLenght;
	char16 * characterTable;
	float32 * characterPreShift;
	float32 * characterWidthTable;
	float32 defaultShiftValue;	
	float32 * kerningBaseShift;	// advance
	
	struct KerningPair
	{
		KerningPair()
		{
			ch1Index = ch2Index = 0;
			shift = 0.0f;
			next = 0;
		}
		uint16 ch1Index;
		uint16 ch2Index;
		float32 shift;
		KerningPair * next;
	};
	
	int32 kerningPairCount;
	KerningPair ** kerningTable;
	void AddKerningPair(KerningPair * kpair);
	KerningPair * FindKerningPair(uint16 ch1Index, uint16 c2Index);
	uint16 CharacterToIndex(char16 c);
	
	static const uint16 INVALID_CHARACTER_INDEX = 0xffff;
	
	bool LoadFontDefinition(const String & fontDefName);
};
	

};

#endif // __DAVAENGINE_GRAPHICSFONT_H__
