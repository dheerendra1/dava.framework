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

#include "EditMatrixControl.h"
#include "Utils/StringFormat.h"

namespace DAVA 
{

EditMatrixControl::EditMatrixControl(const Rect & _rect, bool _readOnly)
    :   UIControl(_rect)
    ,   matrix(0)
    ,   readOnly(_readOnly)
{
    //UIYamlLoader::Load(this, "~res:/Screens/Panels/camera_panel.yaml");
    //UIText
    
    Font *f = FTFont::Create("~res:/Fonts/MyriadPro-Regular.otf");
    f->SetSize(12);
    f->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
    
    float32 spacing = 2.0f;
    float32 buttonWidth = (_rect.dx - spacing * 2.0f) / 4.0f;
    float32 buttonHeight = (_rect.dy - spacing * 2.0f) / 4.0f;
    for (int32 i = 0; i < 4; ++i)
        for (int32 j = 0; j < 4; ++j)
        {
            matrixButtons[i][j] = new UIButton(Rect(spacing + i * buttonWidth, spacing + j * buttonHeight, buttonWidth, buttonHeight));
            
            matrixButtons[i][j]->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
            matrixButtons[i][j]->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
            matrixButtons[i][j]->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
            matrixButtons[i][j]->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
            matrixButtons[i][j]->SetStateDrawType(UIControl::STATE_HOVER, UIControlBackground::DRAW_FILL);
            matrixButtons[i][j]->GetStateBackground(UIControl::STATE_HOVER)->SetColor(Color(0.2, 0.2, 0.2, 0.2));
            matrixButtons[i][j]->SetStateFont(UIControl::STATE_NORMAL, f);
            matrixButtons[i][j]->SetStateText(UIControl::STATE_NORMAL, L"0.000000");
            matrixButtons[i][j]->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &EditMatrixControl::OnEditButtonPressed));
            AddControl(matrixButtons[i][j]);
        }
    
    textFieldBackground = new UIControl(_rect);
    textFieldBackground->GetBackground()->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    textFieldBackground->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &EditMatrixControl::OnEditClosePressed));
    //textFieldBackground->SetInputEnabled(false);
    
    textField = new UITextField(Rect(spacing, spacing, buttonWidth * 4.0f, buttonHeight));
    textField->SetFont(f);

    SafeRelease(f);
}
        

EditMatrixControl::~EditMatrixControl()
{
    for (int32 i = 0; i < 4; ++i)
        for (int32 j = 0; j < 4; ++j)
        {
            SafeRelease(matrixButtons[i][j]);
        }
}
         
void EditMatrixControl::OnEditButtonPressed(BaseObject * obj, void *, void *)
{
    if (!readOnly)
    {
        AddControl(textFieldBackground);
        AddControl(textField);
    }
}
                                  
void EditMatrixControl::OnEditClosePressed(BaseObject * obj, void *, void *)
{
    if (textFieldBackground->GetParent())
    {
        RemoveControl(textFieldBackground);
        RemoveControl(textField);
    }
}

void EditMatrixControl::SetMatrix(Matrix4 * _matrix)
{
    matrix = _matrix;
    for (int32 i = 0; i < 4; ++i)
        for (int32 j = 0; j < 4; ++j)
        {
            matrixButtons[i][j]->SetStateText(UIControl::STATE_NORMAL, Format(L"%f", matrix->_data[i][j]));
        }
}
    
};
