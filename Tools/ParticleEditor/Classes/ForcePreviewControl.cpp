//
//  ForcePreviewControl.cpp
//  TemplateProjectMacOS
//
//  Created by Dmitry Shpakov on 11/4/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "ForcePreviewControl.h"

REGISTER_CLASS(ForcePreviewControl);

ForcePreviewControl::ForcePreviewControl()
{
    GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
    GetBackground()->SetColor(Color(0.2, 0.2, 0.2, 1));
    
    Font *f = FTFont::Create("~res:/Fonts/MyriadPro-Regular.otf");
    f->SetSize(14);
    f->SetColor(Color(1,1,1,1));
    powerText = new UIStaticText();
    powerText->SetFont(f);
    AddControl(powerText);
}

void ForcePreviewControl::SetRect(const DAVA::Rect &rect)
{
    UIControl::SetRect(rect);
    powerText->SetRect(Rect(0, 0, rect.dx, rect.dy/10));
}

void ForcePreviewControl::Input(DAVA::UIEvent *touch)
{

}

void ForcePreviewControl::Update(float32 timeElapsed)
{
    
}

void ForcePreviewControl::SetValue(Vector2 _value)
{
    value = _value;
    powerText->SetText(StringToWString(Format("Force Power: %.2f", value.Length())));
}

void ForcePreviewControl::Draw(const DAVA::UIGeometricData &geometricData)
{
    UIControl::Draw(geometricData);
    RenderManager::Instance()->SetColor(1, 1, 1, 1);
    Rect controlRect = GetRect();
    Vector2 center = Vector2(controlRect.x + controlRect.dx/2, controlRect.y + 11*controlRect.dy/20);
    float32 l = value.Length();
    RenderHelper::Instance()->DrawLine(center, center + value/l*controlRect.dx*0.45f);
    
//    controlRect.x += controlRect.dx*0.05f;
//    controlRect.y += controlRect.dy*0.05f + controlRect.dy/10;
//    controlRect.dx *= 0.9f;
//    controlRect.dy *= 0.8f;
    RenderHelper::Instance()->DrawCircle(center, controlRect.dx*0.5f);
}
