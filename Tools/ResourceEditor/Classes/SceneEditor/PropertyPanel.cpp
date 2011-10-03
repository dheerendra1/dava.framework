/*
    Copyright (c) 2008, DAVA Consulting, LLC
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 
    * Created by Vitaliy Borodovsky
*/

#include "PropertyPanel.h"
#include "UI/UIStaticText.h"

namespace DAVA 
{

PropertyPanel::PropertyPanel(const Rect & rect)
    :   UIControl(rect)
    ,   lastAddedChild(0)
{
    GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
    GetBackground()->SetColor(Color(0.8f, 0.8f, 0.8f, 1.0f));
}
    
PropertyPanel::~PropertyPanel()
{
}
    
UIStaticText * PropertyPanel::AddHeader(const WideString & string, float32 fontSize)
{
    FTFont * font = FTFont::Create("~res:/Fonts/MyriadPro-Regular.otf");
    font->SetSize(fontSize);
    font->SetColor(Color(0.2f, 0.2f, 0.2f, 1.0f));

    UIStaticText * text = new UIStaticText(Rect(10, 0, GetRect().dx - 20, 20));
    text->SetAlign(ALIGN_LEFT | ALIGN_VCENTER);
    text->SetFont(font);
    text->SetText(string);
    SafeRelease(font);
    AddPropertyControl(text);
    text->Release();
    return text;
}

void PropertyPanel::AddPropertyControl(UIControl * control)
{   
    Rect rect = control->GetRect();
    rect.y = 0.0f;
    if (lastAddedChild != 0)
    {
        rect.y = lastAddedChild->GetRect().y + lastAddedChild->GetRect().dy;
    }
    control->SetRect(rect);
    AddControl(control);
    //if (lastAddedChild == 0)
    lastAddedChild = control;
}
    
};
