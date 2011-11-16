//
//  PreviewControl.h
//  ParticlesEditor
//
//  Created by Igor Solovey on 11/2/11.
//  Copyright (c) 2011 DAVA Consulting. All rights reserved.
//

#ifndef TemplateProjectMacOS_PreviewControl_h
#define TemplateProjectMacOS_PreviewControl_h

#include "DAVAEngine.h"

using namespace DAVA;

class PreviewControl : public UIControl
{
public:
    PreviewControl();
    PreviewControl(ParticleEmitter *emitt);
    
    virtual void Update(float32 timeElapsed);
	virtual void Draw(const UIGeometricData &geometricData);
	
	virtual void Input(UIEvent * touch);
    
    void SetEmitter(ParticleEmitter *emitt);
    ParticleEmitter * GetEmitter();
    
protected:
    ParticleEmitter *emitter;
};

#endif
