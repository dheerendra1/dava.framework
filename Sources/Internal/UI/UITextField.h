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

#ifndef __DAVAENGINE_UI_TEXT_FIELD_H__
#define __DAVAENGINE_UI_TEXT_FIELD_H__

#include "Base/BaseTypes.h"
#include "UI/UIControl.h"
#include "UI/UIStaticText.h"

#ifdef __DAVAENGINE_IPHONE__
#include "UI/UITextFieldiPhone.h"
#endif

namespace DAVA 
{

class UITextField;
class UITextFieldDelegate
{
public:
	/*virtual void TextFieldShouldBeginEditing(UITextField * textField);
	virtual void TextFieldDidBeginEditing(UITextField * textField);
	virtual void TextFieldShouldEndEditing(UITextField * textField);
	virtual void TextFieldShouldDidEditing(UITextField * textField);*/
	/*
	 
	 */
	
	virtual void TextFieldShouldReturn(UITextField * textField) = 0;

	/*
	 \brief Asks the delegate if the specified text should be changed.
	 \param[in] textField The text field containing the text.
	 \param[in] replacementLocation starting position of range of characters to be replaced
	 \param[in] replacementLength ending position of range of characters to be replaced
	 \param[in] replacementString the replacement string.
	 \returns true if the specified text range should be replaced; otherwise, false to keep the old text.
	 */
	virtual bool TextFieldKeyPressed(UITextField * textField, int32 replacementLocation, int32 replacementLength, const WideString & replacementString) = 0;
};

class UITextField : public UIControl 
{
public:
	// TODO: fix big BOOLs(TRUE, FALSE) in code
	
	enum eReturnKeyType 
	{
		RETURN_KEY_RETURN = 0,
		RETURN_KEY_DONE
	};

    UITextField();
	
	UITextField(const Rect &rect, bool rectInAbsoluteCoordinates = false);
	virtual ~UITextField();
	
	virtual void WillAppear();
	virtual void DidAppear();
	virtual void WillDisappear();
	
	void SetDelegate(UITextFieldDelegate * delegate);
	UITextFieldDelegate * GetDelegate();

	virtual void Update(float32 timeElapsed);
	
	void OpenKeyboard();
	void CloseKeyboard();
	
	void SetFontColor(float r, float g, float b, float a);
	void SetFontSize(float size);

	virtual void SetSpriteAlign(int32 align);
    
	const WideString & GetText();
	void SetText(const WideString & text);

	void SetReturnKey(int32 returnType);

    virtual void Input(UIEvent *currentInput);

    virtual void LoadFromYamlNode(YamlNode * node, UIYamlLoader * loader);

    Font *GetFont();
    
    void SetFont(Font * font)
    {
        SafeRelease(textFont);
        textFont = SafeRetain(font);
		staticText->SetFont(textFont);
    }
protected:
    bool needRedraw;
	WideString text;
	UITextFieldDelegate * delegate;
	float32	cursorBlinkingTime;
    Font * textFont;
//    Sprite *textSprite;

//    virtual void Draw(const UIGeometricData &geometricData);

    void RenderText();
private:
//    void InitAfterYaml();

#ifdef __DAVAENGINE_IPHONE__
	UITextFieldiPhone * textFieldiPhone;
#endif

    UIStaticText * staticText;
    float32 cursorTime;
    bool showCursor;

	static UITextField * focus;
};

};

#endif // __DAVAENGINE_UI_TEXT_FIELD_H__