//
//  PreviewControl.cpp
//  ParticlesEditor
//
//  Created by Igor Solovey on 11/2/11.
//  Copyright (c) 2011 DAVA Consulting. All rights reserved.
//

#include "PreviewControl.h"

REGISTER_CLASS(PreviewControl);

PreviewControl::PreviewControl()
{
    GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
    GetBackground()->SetColor(Color(0, 0, 0, 1));
}

PreviewControl::PreviewControl(ParticleEmitter *emitt)
{
    GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
    GetBackground()->SetColor(Color(0, 0, 0, 1));
    emitter = emitt;
    emitter->SetPosition(Vector2(GetRect().x + GetRect().dx/2, GetRect().y + GetRect().dy/2));
}

void PreviewControl::Input(DAVA::UIEvent *touch)
{
	if( (touch->phase == UIEvent::PHASE_DRAG || touch->phase == UIEvent::PHASE_BEGAN) && (touch->tid == UIEvent::BUTTON_1) && (touch->point.x > GetPosition().x) )
	{
		emitter->SetPosition(Vector2(touch->point.x, touch->point.y));
	}
    
	if( (touch->phase == UIEvent::PHASE_ENDED) && (touch->tid == UIEvent::BUTTON_2))
	{
		emitter->Restart(true);
	}
}

void PreviewControl::SetEmitter(ParticleEmitter *emitt)
{
    emitter = emitt;
    emitter->SetPosition(Vector2(GetRect().x + GetRect().dx/2, GetRect().y + GetRect().dy/2));
}

ParticleEmitter * PreviewControl::GetEmitter()
{
    return emitter;
}

void PreviewControl::Update(float32 timeElapsed)
{
    emitter->Update(timeElapsed);
}

void PreviewControl::Draw(const DAVA::UIGeometricData &geometricData)
{
    UIControl::Draw(geometricData);
    if(emitter)
    {
        emitter->Draw();
    }
}