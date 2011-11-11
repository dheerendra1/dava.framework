//
//  ForcePreviewControl.h
//  TemplateProjectMacOS
//
//  Created by Dmitry Shpakov on 11/4/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef TemplateProjectMacOS_ForcePreviewControl_h
#define TemplateProjectMacOS_ForcePreviewControl_h

#include "DAVAEngine.h"

using namespace DAVA;

class ForcePreviewControl : public UIControl
{
public:
    ForcePreviewControl();
    
    virtual void Update(float32 timeElapsed);
	virtual void Draw(const UIGeometricData &geometricData);
	
	virtual void Input(UIEvent * touch);
    
    void SetRect(const DAVA::Rect &rect);
    
    void SetValue(Vector2 _value);
    
protected:
    Vector2 value;
    UIStaticText *powerText;
};

#endif
