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
        * Created by Vitaliy Borodovsky 
=====================================================================================*/
#include "Particles/ParticleEmitterObject.h"
#include "Scene2D/GameObjectManager.h"

namespace DAVA 
{
	
ParticleEmitterObject::ParticleEmitterObject()
	: emitter(0)
	, removeAfterNRepeats(-1)
	, isRemoveWhenEmpty(false)
{
}

ParticleEmitterObject::~ParticleEmitterObject()
{
	SafeRelease(emitter);
}

void ParticleEmitterObject::SetEmitter(ParticleEmitter * _emitter)
{
	SafeRelease(emitter);
	emitter = SafeRetain(_emitter);
}
	
void ParticleEmitterObject::LoadFromYaml(const String & pathName)
{
	SafeRelease(emitter);
	emitter = new ParticleEmitter();
	emitter->LoadFromYaml(pathName);
}

void ParticleEmitterObject::RecalcHierarchy(Sprite::DrawState & parentDrawState)
{
	GameObject::RecalcHierarchy(parentDrawState);
	
	emitter->SetPosition(globalDrawState.position);
	emitter->SetAngle(globalDrawState.angle);
}
	
void ParticleEmitterObject::RemoveAfterNRepeats(int32 numberOfRepeats)
{
	removeAfterNRepeats = numberOfRepeats;
}

void ParticleEmitterObject::RemoveWhenEmpty()
{
	isRemoveWhenEmpty = true;
}

ParticleEmitter * ParticleEmitterObject::GetEmitter()
{
	return emitter;
}

void ParticleEmitterObject::Update(float32 timeElapsed)
{
	emitter->Update(timeElapsed);
	if (removeAfterNRepeats > 0)
	{
		if (emitter->GetRepeatCount() >= removeAfterNRepeats)
		{
			GetManager()->RemoveObject(this);
		}	
	}
	if (isRemoveWhenEmpty)
	{
		if (emitter->GetParticleCount() == 0)
			GetManager()->RemoveObject(this);
	}
}

void ParticleEmitterObject::Draw()
{
	//if (!visible)return;
	emitter->Draw();
}

}
