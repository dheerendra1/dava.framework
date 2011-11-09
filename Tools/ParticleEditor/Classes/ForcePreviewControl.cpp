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
}

void ForcePreviewControl::Draw(const DAVA::UIGeometricData &geometricData)
{
    UIControl::Draw(geometricData);
    RenderManager::Instance()->SetColor(1, 1, 1, 1);
    Rect controlRect = GetRect();
    Vector2 center = Vector2(controlRect.x + controlRect.dx/2, controlRect.y + controlRect.dy/2);
    RenderHelper::Instance()->DrawLine(center, center + value*controlRect.dx/2);
}
