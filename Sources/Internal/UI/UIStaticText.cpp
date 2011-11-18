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
#include "Utils/Utils.h"
#include "UI/UIStaticText.h"
#include "Render/RenderManager.h"
#include "Base/ObjectFactory.h"
#include "UI/UIYamlLoader.h"
#include "Utils/StringFormat.h"
#include "FileSystem/LocalizationSystem.h"

namespace DAVA 
{
	
REGISTER_CLASS(UIStaticText);

UIStaticText::UIStaticText(const Rect &rect, bool rectInAbsoluteCoordinates/* = FALSE*/) 
:	UIControl(rect, rectInAbsoluteCoordinates)
	,tempSize(0, 0)
{
	inputEnabled = false;
	textBlock = TextBlock::Create(Vector2(rect.dx, rect.dy));
	background->SetAlign(ALIGN_TOP|ALIGN_LEFT);
	background->SetPerPixelAccuracyType(UIControlBackground::PER_PIXEL_ACCURACY_ENABLED);
}

UIStaticText::~UIStaticText()
{
	SafeRelease(textBlock);
}

	
UIControl *UIStaticText::Clone()
{
	UIStaticText *t = new UIStaticText(GetRect());
	t->CopyDataFrom(this);
	return t;
}
	
void UIStaticText::CopyDataFrom(UIControl *srcControl)
{
	UIControl::CopyDataFrom(srcControl);
    UIStaticText *t = (UIStaticText *)srcControl;
    textBlock = t->textBlock->Clone();
}
	
UIStaticText *UIStaticText::CloneStaticText()
{
	return (UIStaticText *)Clone();
}
	
void UIStaticText::SetText(const WideString& _string, const Vector2 &requestedTextRectSize/* = Vector2(0,0)*/)
{
	textBlock->SetRectSize(size);
	textBlock->SetText(_string, requestedTextRectSize);
	PrepareSprite();
}
	
void UIStaticText::SetFittingOption(int32 fittingType)
{
	textBlock->SetRectSize(size);
	textBlock->SetFittingOption(fittingType);
	PrepareSprite();
}

void UIStaticText::SetFont(Font * _font)
{
	textBlock->SetRectSize(size);
	textBlock->SetFont(_font);
	PrepareSprite();
}

void UIStaticText::SetMultiline(bool _isMultilineEnabled)
{
	textBlock->SetRectSize(size);
	textBlock->SetMultiline(_isMultilineEnabled);
	PrepareSprite();
}
	
void UIStaticText::SetSpriteAlign(int32 align)
{
	SetAlign(align);
}

void UIStaticText::SetAlign(int32 _align)
{
	textBlock->SetAlign(_align);
}

	
const Vector2 &UIStaticText::GetTextSize()
{
	if (textBlock->IsSpriteReady())
	{
		return tempSize = textBlock->GetSprite()->GetSize();
	}
	else 
	{
		tempSize = Vector2(0,0);
	}

	return tempSize;
}
	



void UIStaticText::Draw(const UIGeometricData &geometricData)
{
	textBlock->SetRectSize(size);
	PrepareSprite();
	textBlock->PreDraw();
	UIControl::Draw(geometricData);	
}
	
const WideString & UIStaticText::GetText()
{
	return textBlock->GetText();
}

void UIStaticText::LoadFromYamlNode(YamlNode * node, UIYamlLoader * loader)
{
	UIControl::LoadFromYamlNode(node, loader);
	
	YamlNode * fontNode = node->Get("font");
	YamlNode * textNode = node->Get("text");
	YamlNode * multilineNode = node->Get("multiline");
    YamlNode * fittingNode = node->Get("fitting");

	if (fontNode)
	{
		const String & fontName = fontNode->AsString();
		Font * font = loader->GetFontByName(fontName);
		SetFont(font);
	}

	bool multiline = loader->GetBoolFromYamlNode(multilineNode, false);
	SetMultiline(multiline);
	
    if(fittingNode)
    {
        int32 fittingArray[] = {TextBlock::FITTING_DISABLED, TextBlock::FITTING_ENLARGE, 
                                TextBlock::FITTING_REDUCE, TextBlock::FITTING_POINTS};
		String fittingValues[] = {"Disabled", "Enlarge", "Reduce", "Points"};

		const String & fittinOption = fittingNode->AsString();
        
        int32 fittingType = 0;
        for(int32 i = 0 ; i < 4; ++i)
        {
            size_t find = fittinOption.find(fittingValues[i]);
            if(find != fittinOption.npos)
            {
                fittingType |= fittingArray[i];
            }
        }

        SetFittingOption(fittingType);
    }
    
	if (textNode)
	{
		SetText(LocalizedString(textNode->AsWString()));
	}
    
}

};