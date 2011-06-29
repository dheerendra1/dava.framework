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

#include "UI/UIButton.h"
#include "Base/ObjectFactory.h"
#include "UI/UIYamlLoader.h"
#include "Utils/StringFormat.h"

namespace DAVA 
{
	REGISTER_CLASS(UIButton);

	UIButton::UIButton(const Rect &rect, bool rectInAbsoluteCoordinates/* = FALSE*/)
	: UIControl(rect, rectInAbsoluteCoordinates)
	{
		inputEnabled = TRUE;
		oldState = 0;
		for(int i = 0; i < DRAW_STATE_COUNT; i++)
		{
			stateBacks[i] = NULL;
			stateTexts[i] = NULL;
		}
		stateBacks[DRAW_STATE_UNPRESSED] = background;
		selectedBackground = background;
		selectedText = NULL;
		exclusiveInput = TRUE;

	}


	UIButton::~UIButton()
	{
		background = stateBacks[DRAW_STATE_UNPRESSED];
		for(int i = 0; i < DRAW_STATE_COUNT; i++)
		{
			SafeRelease(stateTexts[i]);
		}
		for(int i = 1; i < DRAW_STATE_COUNT; i++)
		{
			SafeRelease(stateBacks[i]);
		}
		SafeRelease(selectedText);
	}
	
	UIButton *UIButton::CloneButton()
	{
		return (UIButton *)Clone();
	}

	
	UIControl *UIButton::Clone()
	{
		UIButton *b = new UIButton(GetRect());
		b->CopyDataFrom(this);
		return b;
	}
	void UIButton::CopyDataFrom(UIControl *srcControl)
	{
		background = stateBacks[DRAW_STATE_UNPRESSED];
		SafeRelease(selectedText);
		for(int i = 1; i < DRAW_STATE_COUNT; i++)
		{
			SafeRelease(stateBacks[i]);
			if(stateTexts[i])
			{
				RemoveControl(stateTexts[i]);
				SafeRelease(stateTexts[i]);
			}
		}
		UIControl::CopyDataFrom(srcControl);
		UIButton *srcButton = (UIButton *)srcControl;
		for(int i = 1; i < DRAW_STATE_COUNT; i++)
		{
			if(srcButton->stateBacks[i])
			{
				stateBacks[i] = srcButton->stateBacks[i]->Clone();
			}
			if(srcButton->stateTexts[i])
			{
				stateTexts[i] = srcButton->stateTexts[i]->CloneStaticText();
			}
		}
		selectedBackground = background;
		oldState = 0;
	}


	void UIButton::SetRect(const Rect &rect, bool rectInAbsoluteCoordinates/* = FALSE*/)
	{
		UIControl::SetRect(rect, rectInAbsoluteCoordinates);
		for(int i = 1; i < DRAW_STATE_COUNT; i++)
		{
			if(stateTexts[i])
			{
				stateTexts[i]->SetRect(Rect(0, 0, rect.dx, rect.dy));
			}
		}
	}

	void UIButton::SetStateSprite(int32 state, const String &spriteName, int32 spriteFrame/* = 0*/)
	{
		for(int i = 0; i < DRAW_STATE_COUNT; i++)
		{
			if(state & 0x01)
			{
				CreateBackForState((eButtonDrawState)i)->SetSprite(spriteName, spriteFrame);
			}
			state >>= 1;
		}
	}
	
	void UIButton::SetStateSprite(int32 state, Sprite *newSprite, int32 spriteFrame/* = 0*/)
	{
		for(int i = 0; i < DRAW_STATE_COUNT; i++)
		{
			if(state & 0x01)
			{
				CreateBackForState((eButtonDrawState)i)->SetSprite(newSprite, spriteFrame);
			}
			state >>= 1;
		}
	}
	
	void UIButton::SetStateFrame(int32 state, int32 spriteFrame)
	{
		for(int i = 0; i < DRAW_STATE_COUNT; i++)
		{
			if(state & 0x01)
			{
				CreateBackForState((eButtonDrawState)i)->SetFrame(spriteFrame);
			}
			state >>= 1;
		}
	}
	
	void UIButton::SetStateDrawType(int32 state, UIControlBackground::eDrawType drawType)
	{
		for(int i = 0; i < DRAW_STATE_COUNT; i++)
		{
			if(state & 0x01)
			{
				CreateBackForState((eButtonDrawState)i)->SetDrawType(drawType);
			}
			state >>= 1;
		}
	}
	
	void UIButton::SetStateAlign(int32 state, int32 align)
	{
		for(int i = 0; i < DRAW_STATE_COUNT; i++)
		{
			if(state & 0x01)
			{
				CreateBackForState((eButtonDrawState)i)->SetAlign(align);
			}
			state >>= 1;
		}
	}
	Sprite* UIButton::GetStateSprite(int32 state)
	{
		return GetActualBackground(state)->GetSprite();
	}
	int32 UIButton::GetStateFrame(int32 state)
	{
		return GetActualBackground(state)->GetFrame();
	}
	UIControlBackground::eDrawType UIButton::GetStateDrawType(int32 state)
	{
		return GetActualBackground(state)->GetDrawType();
	}
	int32 UIButton::GetStateAlign(int32 state)
	{
		return GetActualBackground(state)->GetAlign();
	}
	
	UIControlBackground *UIButton::GetStateBackground(int32 state)
	{
		return GetActualBackground(state);
	}
	
	
	void UIButton::SetStateBackground(int32 state, UIControlBackground *newBackground)
	{
		for(int i = 0; i < DRAW_STATE_COUNT; i++)
		{
			if(state & 0x01)
			{
				SafeRelease(stateBacks[i]);
				stateBacks[i] = newBackground->Clone();
			}
			state >>= 1;
		}
		oldState = 0;
	}

	
	void UIButton::SetStateFont(int32 state, Font *font)
	{
		for(int i = 0; i < DRAW_STATE_COUNT; i++)
		{
			if(state & 0x01)
			{
				CreateTextForState((eButtonDrawState)i)->SetFont(font);
			}
			state >>= 1;
		}
	}
	
	void UIButton::SetStateText(int32 state, const WideString &text, const Vector2 &requestedTextRectSize/* = Vector2(0,0)*/)
	{
		for(int i = 0; i < DRAW_STATE_COUNT; i++)
		{
			if(state & 0x01)
			{
				CreateTextForState((eButtonDrawState)i)->SetText(text, requestedTextRectSize);
			}
			state >>= 1;
		}
	}
	
	void UIButton::SetStateTextControl(int32 state, UIStaticText *textControl)
	{
		for(int i = 0; i < DRAW_STATE_COUNT; i++)
		{
			if(state & 0x01)
			{
				if(stateTexts[i])
				{
					RemoveControl(stateTexts[i]);
					SafeRelease(stateTexts[i]);
				}
				stateTexts[i] = textControl->CloneStaticText();
			}
			state >>= 1;
		}
		oldState = 0;
	}
	
//	FTFont *UIButton::GetStateFont(int32 state)
//	{
//		UIStaticText *tx = GetActualText(state);
//		if(tx)
//		{
//			return tx->GetFont();
//		}
//		return NULL;
//	}
//	
//	const WideString &UIButton::GetStateText(int32 state)
//	{
//		
//	}
	
	UIStaticText *UIButton::GetStateTextControl(int32 state)
	{
		return GetActualText(state);
	}
	
	
	
	void UIButton::SystemUpdate(float32 timeElapsed)
	{
		UIControl::SystemUpdate(timeElapsed);

		if(oldState != controlState)
		{
			if(selectedText)
			{
				RemoveControl(selectedText);
				SafeRelease(selectedText);
			}
			selectedText = SafeRetain(GetActualText(controlState));
			if(selectedText)
			{
				AddControl(selectedText);
			}
			
			selectedBackground = GetActualBackground(controlState);
			
			oldState = controlState;
		}
	}
	
	void UIButton::SetVisible(bool isVisible, bool hierarchic)
	{
		if(selectedText)
		{
			RemoveControl(selectedText);
		}
		UIControl::SetVisible(isVisible, hierarchic);
		for(int i = 0; i < DRAW_STATE_COUNT; i++)
		{
			if(stateTexts[i])
			{
				stateTexts[i]->SetVisible(isVisible);
			}
		}
		if(selectedText)
		{
			AddControl(selectedText);
		}
	}
	
	void UIButton::SetInputEnabled(bool isEnabled, bool hierarchic)
	{
		if(selectedText)
		{
			RemoveControl(selectedText);
		}
		UIControl::SetInputEnabled(isEnabled, hierarchic);
		if(selectedText)
		{
			AddControl(selectedText);
		}
	}
	void UIButton::SetDisabled(bool isDisabled, bool hierarchic)
	{
		if(selectedText)
		{
			RemoveControl(selectedText);
		}
		UIControl::SetDisabled(isDisabled, hierarchic);
		if(selectedText)
		{
			AddControl(selectedText);
		}
	}
	
	void UIButton::SetSelected(bool isSelected, bool hierarchic)
	{
		if(selectedText)
		{
			RemoveControl(selectedText);
		}
		UIControl::SetSelected(isSelected, hierarchic);
		if(selectedText)
		{
			AddControl(selectedText);
		}
	}
	
	void UIButton::SetExclusiveInput(bool isExclusiveInput, bool hierarchic)
	{
		if(selectedText)
		{
			RemoveControl(selectedText);
		}
		UIControl::SetExclusiveInput(isExclusiveInput, hierarchic);
		if(selectedText)
		{
			AddControl(selectedText);
		}
	}
	
	void UIButton::SetMultiInput(bool isMultiInput, bool hierarchic)
	{
		if(selectedText)
		{
			RemoveControl(selectedText);
		}
		UIControl::SetMultiInput(isMultiInput, hierarchic);
		if(selectedText)
		{
			AddControl(selectedText);
		}
	}
	
	void UIButton::SetBackground(UIControlBackground *newBg)
	{
		DVASSERT(false);
	}
	
	UIControlBackground * UIButton::GetBackground()
	{
		return selectedBackground;
	}


	
	void UIButton::SystemDraw(const UIGeometricData &geometricData)
	{
		background = selectedBackground;
		UIControl::SystemDraw(geometricData);
		background = stateBacks[DRAW_STATE_UNPRESSED];
	}
	

	
	
	
	
	
	
	
	UIControlBackground *UIButton::GetActualBackground(int32 state)
	{
		return stateBacks[BackgroundIndexForState(GetDrawStateForControlState(state))];
	}

	UIStaticText *UIButton::GetActualText(int32 state)
	{
		return stateTexts[TextIndexForState(GetDrawStateForControlState(state))];
	}

	
	UIControlBackground *UIButton::CreateBackForState(eButtonDrawState buttonState)
	{
		if(stateBacks[buttonState])
		{
			return stateBacks[buttonState];
		}
		
		stateBacks[buttonState] = new UIControlBackground();
		return stateBacks[buttonState];
	}
	
	UIStaticText *UIButton::CreateTextForState(eButtonDrawState buttonState)
	{
		if(stateTexts[buttonState])
		{
			return stateTexts[buttonState];
		}
		
		stateTexts[buttonState] = new UIStaticText(Rect(0, 0, size.x, size.y));
		if(!GetVisible())
		{
			stateTexts[buttonState]->SetVisible(false, false);
		}
		return stateTexts[buttonState];
	}


	UIButton::eButtonDrawState UIButton::GetDrawStateForControlState(int32 state)
	{
		if(state & UIControl::STATE_DISABLED)
		{
			return DRAW_STATE_DISABLED;
		}
		else if(state & UIControl::STATE_SELECTED)
		{
			return DRAW_STATE_SELECTED;
		}
		else if(state & UIControl::STATE_PRESSED_INSIDE)
		{
			return DRAW_STATE_PRESSED_INSIDE;
		}
		else if(state & UIControl::STATE_PRESSED_OUTSIDE)
		{
			return DRAW_STATE_PRESSED_OUTSIDE;
		}
		else if(state & UIControl::STATE_HOVER)
		{
			return DRAW_STATE_HOVERED;
		}
		
		return DRAW_STATE_UNPRESSED;
	}

	int32 UIButton::BackgroundIndexForState(eButtonDrawState buttonState)
	{
		if(stateBacks[buttonState])
		{//return current state if dada for state is present
			return buttonState;
		}
		switch (buttonState) 
		{//find other state if data for the requested state is absent
			case DRAW_STATE_PRESSED_INSIDE:
			{
				if(stateBacks[DRAW_STATE_PRESSED_OUTSIDE])
				{
					return DRAW_STATE_PRESSED_OUTSIDE;
				}
				return DRAW_STATE_UNPRESSED;
			}
				break;
			case DRAW_STATE_PRESSED_OUTSIDE:
			{
				return DRAW_STATE_UNPRESSED;
			}
				break;
			case DRAW_STATE_SELECTED:
			{
				if(stateBacks[DRAW_STATE_PRESSED_INSIDE])
				{
					return DRAW_STATE_PRESSED_INSIDE;
				}
				if(stateBacks[DRAW_STATE_PRESSED_OUTSIDE])
				{
					return DRAW_STATE_PRESSED_OUTSIDE;
				}
				return DRAW_STATE_UNPRESSED;
			}
				break;
			case DRAW_STATE_DISABLED:
			{
				return DRAW_STATE_UNPRESSED;
			}
				break;
			case DRAW_STATE_HOVERED:
			{
				return DRAW_STATE_UNPRESSED;
			}
				break;
		}
		return DRAW_STATE_UNPRESSED;
	}

	int32 UIButton::TextIndexForState(eButtonDrawState buttonState)
	{
		if(stateTexts[buttonState])
		{
			return buttonState;
		}
		switch (buttonState) 
		{//find other state if data for the requested state is absent
			case DRAW_STATE_PRESSED_INSIDE:
			{
				if(stateTexts[DRAW_STATE_PRESSED_OUTSIDE])
				{
					return DRAW_STATE_PRESSED_OUTSIDE;
				}
				return DRAW_STATE_UNPRESSED;
			}
				break;
			case DRAW_STATE_PRESSED_OUTSIDE:
			{
				return DRAW_STATE_UNPRESSED;
			}
				break;
			case DRAW_STATE_SELECTED:
			{
				if(stateTexts[DRAW_STATE_PRESSED_INSIDE])
				{
					return DRAW_STATE_PRESSED_INSIDE;
				}
				if(stateTexts[DRAW_STATE_PRESSED_OUTSIDE])
				{
					return DRAW_STATE_PRESSED_OUTSIDE;
				}
				return DRAW_STATE_UNPRESSED;
			}
				break;
			case DRAW_STATE_DISABLED:
			{
				return DRAW_STATE_UNPRESSED;
			}
				break;
			case DRAW_STATE_HOVERED:
			{
				return DRAW_STATE_UNPRESSED;
			}
				break;
		}
		return DRAW_STATE_UNPRESSED;
	}
	
	void UIButton::LoadFromYamlNode(YamlNode * node, UIYamlLoader * loader)
	{
		UIControl::LoadFromYamlNode(node, loader);
		
		int32 stateArray[] = {STATE_NORMAL, STATE_PRESSED_INSIDE, STATE_PRESSED_OUTSIDE, STATE_DISABLED, STATE_SELECTED, STATE_HOVER};
		String statePostfix[] = {"Normal", "PressedInside", "PressedOutside", "Disabled", "Selected", "Hover"};
	
		for (int k = 0; k < STATE_COUNT; ++k)
		{
			YamlNode * stateSpriteNode = node->Get(Format("stateSprite%s", statePostfix[k].c_str()));
			if (stateSpriteNode)
			{
				YamlNode * spriteNode = stateSpriteNode->Get(0);
				YamlNode * frameNode = stateSpriteNode->Get(1);
				
				int32 frame = 0;
				if (frameNode)frame = frameNode->AsInt();
				if (spriteNode)
				{
					SetStateSprite(stateArray[k], spriteNode->AsString(), frame);
				}
			}
            
            YamlNode * stateDrawTypeNode = node->Get(Format("stateDrawType%s", statePostfix[k].c_str()));
			if (stateDrawTypeNode)
			{
				UIControlBackground::eDrawType type = (UIControlBackground::eDrawType)loader->GetDrawTypeFromNode(stateDrawTypeNode);
                SetStateDrawType(stateArray[k],type);
			}
            
            YamlNode * stateAlignNode = node->Get(Format("stateAlign%s", statePostfix[k].c_str()));
			if (stateAlignNode)
			{
				int32 align = loader->GetAlignFromYamlNode(stateAlignNode);
                SetStateAlign(stateArray[k],align);
			}

			YamlNode * stateFontNode = node->Get(Format("stateFont%s", statePostfix[k].c_str()));
			if (stateFontNode)
			{
				Font * font = loader->GetFontByName(stateFontNode->AsString());
				if (font)SetStateFont(stateArray[k], font);
			}
			
			YamlNode * stateTextNode = node->Get(Format("stateText%s", statePostfix[k].c_str()));
			if (stateTextNode)
			{
				SetStateText(stateArray[k], stateTextNode->AsWString());
			}
			
		}
		for (int k = 0; k < STATE_COUNT; ++k)
		{
			YamlNode * colorInheritNode = node->Get("colorInherit");
			UIControlBackground::eColorInheritType type = (UIControlBackground::eColorInheritType)loader->GetColorInheritTypeFromNode(colorInheritNode);
			if(colorInheritNode)
			{
				for(int32 i = 0; i < DRAW_STATE_COUNT; ++i)
				{
					if(stateBacks[i])
					{
						stateBacks[i]->SetColorInheritType(type);
					}
				}
			}
		}
		
		
	}
};