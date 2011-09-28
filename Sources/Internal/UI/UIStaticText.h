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
        * Created by Alexey 'Hottych' Prosin
=====================================================================================*/

#ifndef __DAVAENGINE_UI_STATIC_TEXT_H__
#define __DAVAENGINE_UI_STATIC_TEXT_H__

#include "Base/BaseTypes.h"
#include "UI/UIControl.h"
//#include "FTFont.h"
//#include "Texture.h"
//#include "Sprite.h"
#include "Render/2D/TextBlock.h"

namespace DAVA 
{
class UIStaticText : public UIControl 
{
public:
//	enum eFitType 
//	{
//			FITTING_DISABLED = 0
//		,	FITTING_ENLARGE = 1
//		,	FITTING_REDUCE = 2
//        ,   FITTING_POINTS = 4
//        
//	};
	
	UIStaticText(const Rect &rect = Rect(), bool rectInAbsoluteCoordinates = false); 
	virtual ~UIStaticText();

	//if requested size is 0 - text creates in the rect with size of the drawRect on draw phase
	//if requested size is >0 - text creates int the rect with the requested size
	//if requested size in <0 - rect creates for the all text size	
	void SetText(const WideString & string, const Vector2 &requestedTextRectSize = Vector2(0,0));
	void SetFont(Font * font);
	void SetMultiline(bool isMultilineEnabled);
	void SetFittingOption(int32 fittingType);//may be FITTING_DISABLED, FITTING_ENLARGE, FITTING_REDUCE, FITTING_ENLARGE | FITTING_REDUCE
	void SetAlign(int32 _align);
	virtual void SetSpriteAlign(int32 align);

	
	const Vector2 &GetTextSize();
	
	inline void PrepareSprite()
	{
		if (textBlock->IsSpriteReady())
		{
			if (background->GetSprite() != textBlock->GetSprite()) 
			{
				SetSprite(textBlock->GetSprite(), 0);
			}
		}
		else 
		{
			SetSprite(NULL, 0);
		}
	}

	
	const WideString & GetText();

	
	Font * GetFont() { return textBlock->GetFont(); }
	
	virtual UIControl *Clone();
	virtual void CopyDataFrom(UIControl *srcControl);
	UIStaticText *CloneStaticText();
	TextBlock * GetTextBlock() { return textBlock; }
	
protected:
	
//    int32 Fitting
    
	TextBlock *textBlock;
	Vector2 tempSize;
	
	virtual void Draw(const UIGeometricData &geometricData);
	
public:
	void LoadFromYamlNode(YamlNode * node, UIYamlLoader * loader);
	
};
};

#endif //__DAVAENGINE_UI_STATIC_TEXT_H__
