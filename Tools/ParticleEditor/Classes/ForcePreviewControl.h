//
//  ForcePreviewControl.h
//  ParticlesEditor
//
//  Created by Igor Solovey on 11/4/11.
//  Copyright (c) 2011 DAVA Consulting. All rights reserved.
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
    
    void SetValue(Vector3 _value);
    
protected:
    Vector3 value;
    UIStaticText *powerText;
};

#endif
