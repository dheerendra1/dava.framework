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
	=====================================================================================*/

#include "TextinputTestScreen.h"
#include "AppScreens.h"

TextinputTestScreen::TextinputTestScreen(const WideString & headerText)
{
	this->headerText = headerText;
}

void TextinputTestScreen::LoadResources()
{
	SetupBackground();

	CreateFont();
	CreateHeader();
	CreateField1();
	CreateField2();
	CreateNextScreenButton();
}

void TextinputTestScreen::SetupBackground()
{
	background->SetDrawType(UIControlBackground::DRAW_FILL);
	background->color = Color(0.f, 0.f, 0.f, 1.f);
}


void TextinputTestScreen::CreateFont()
{
	font = FTFont::Create("~res:/Fonts/yikes.ttf");
	font->SetSize(20.f);
	font->SetColor(0.f, 0.f, 0.f, 1.f);
}

void TextinputTestScreen::CreateHeader()
{
	UIStaticText * header = new UIStaticText(Rect(5.f, 5.f, 300.f, 50.f));
	AddControl(header);
	header->Release();
	header->SetFont(font);
	header->GetFont()->SetColor(Color::White());
	header->SetText(headerText);
}

void TextinputTestScreen::CreateField1()
{
	field1 = new UITextField(Rect(100.f, 100.f, 200.f, 50.f));
	AddControl(field1);
	field1->Release();
	field1->GetBackground()->color = Color::White();
	field1->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
	field1->SetFont(font);
	field1->SetDelegate(this);
}

void TextinputTestScreen::CreateField2()
{
	field2 = new UITextField(Rect(100.f, 200.f, 200.f, 50.f));
	AddControl(field2);
	field2->Release();
	field2->GetBackground()->color = Color::White();
	field2->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
	field2->SetFont(font);
	field2->SetDelegate(this);
}

void TextinputTestScreen::CreateNextScreenButton()
{
	nextScreenButton = new UIButton(Rect(400.f, 100.f, 100.f, 50.f));
	AddControl(nextScreenButton);
	nextScreenButton->Release();
	nextScreenButton->GetBackground()->color = Color(.5f, .5f, .5f, 1.f);
	nextScreenButton->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
	nextScreenButton->SetStateFont(UIControl::STATE_NORMAL | UIControl::STATE_PRESSED_INSIDE, font);
	nextScreenButton->SetStateText(UIControl::STATE_NORMAL | UIControl::STATE_PRESSED_INSIDE, L"Next screen");
	nextScreenButton->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TextinputTestScreen::OnNextScreenButton));
}

void TextinputTestScreen::OnNextScreenButton(BaseObject * obj, void * userData, void * callerData)
{
	int32 currentScreenId = UIScreenManager::Instance()->GetScreenId();
	int32 nextScreenId;
	if(currentScreenId == SCREEN_TEXTINPUT_TEST_1)
		nextScreenId = SCREEN_TEXTINPUT_TEST_2;
	else
		nextScreenId = SCREEN_TEXTINPUT_TEST_1;

	UIScreenManager::Instance()->SetScreen(nextScreenId);
}

//UITextField delegate
void TextinputTestScreen::TextFieldShouldReturn(UITextField * textField)
{

}

bool TextinputTestScreen::TextFieldKeyPressed(UITextField * textField, int32 replacementLocation, int32 replacementLength, const WideString & replacementString)
{
	return true;
}

void TextinputTestScreen::UnloadResources()
{
	RemoveAllControls();
	SafeRelease(font);
}






