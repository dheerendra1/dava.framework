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
#ifndef __DAVAENGINE_TEXTBLOCK_H__
#define __DAVAENGINE_TEXTBLOCK_H__

#include "Base/BaseObject.h"
#include "Base/BaseTypes.h"
#include "Base/BaseMath.h"
#include "Render/RenderBase.h"
#include "Render/Texture.h"
#include "Render/2D/Sprite.h"
#include "Render/2D/Font.h"

namespace DAVA
{

/**
	\ingroup render_2d
	\brief Class to render text on the screen. 
	This class support to draw singleline / multiline text to sprites using font objects that available in SDK.
	Normally you do not need it directly and you can use UIStaticText or TextGameObject. 
 */
class TextBlock : public BaseObject
{
public:
	enum eFitType 
	{
			FITTING_DISABLED = 0
		,	FITTING_ENLARGE	= 1
		,	FITTING_REDUCE = 2
        ,   FITTING_POINTS = 4
	};
	
	static void ScreenResolutionChanged();
	
	static TextBlock * Create(const Vector2 & size);
	
	virtual void SetFont(Font * font);
	virtual void SetRectSize(const Vector2 & size);
	virtual void SetAlign(int32 align);
	virtual int32 GetAlign();
	
	//[DO NOT ACTUAL ANYMORE] if requested size is 0 - text creates in the rect with size of the drawRect on draw phase
	//if requested size is >0 - text creates int the rect with the requested size
	//if requested size in <0 - rect creates for the all text size	
	virtual void SetText(const WideString & string, const Vector2 &requestedTextRectSize = Vector2(0,0));	
	virtual void SetMultiline(bool isMultilineEnabled);
	virtual void SetFittingOption(int32 fittingType);//may be FITTING_DISABLED, FITTING_ENLARGE, FITTING_REDUCE, FITTING_ENLARGE | FITTING_REDUCE, FITTING_POINTS

	virtual Font * GetFont();
	virtual const WideString & GetText();
	virtual bool GetMultiline();
	virtual int32 GetFittingOption();
	
	Sprite * GetSprite();
	bool IsSpriteReady();

	void PreDraw();

    TextBlock * Clone();
    
protected:
	TextBlock();
	~TextBlock();
	
	void Prepare();
	
	void DrawToBuffer(int16 *buf);



	void ProcessAlign();
	
	
	bool isPredrawed;
	
	
	bool cacheUseJustify;
	int32 cacheDx;
	int32 cacheDy;
	int32 cacheW;
	float32 cacheFinalW;
	
	Font * font;
	Font * constFont;
	WideString text;
    WideString pointsStr;
	bool isMultilineEnabled;
	int32 fittingType;
	Vector2 rectSize;
	int32 align;
	
	float32 originalFontSize;
	bool needRedraw;
	Vector2 requestedSize;
	
	Sprite * sprite;
	Vector<WideString> multilineStrings;
	Vector<int32> stringSizes;
};

}; //end of namespace

#endif // __DAVAENGINE_TEXTBLOCK_H__