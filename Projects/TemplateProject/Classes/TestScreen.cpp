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
#include "TestScreen.h"

void TestScreen::LoadResources()
{
	manager = GameObjectManager::Create();
	bigBox = GameObject::Create("~res:/Gfx/GameObjects/blueboxbig");
	bigBox->SetPivotPoint(ALIGN_HCENTER | ALIGN_VCENTER);
	bigBox->SetPosition(200, 200);
	manager->AddObject(bigBox.Get());

	smallBox = GameObject::Create("~res:/Gfx/GameObjects/bluebox");
	smallBox->SetPivotPoint(ALIGN_HCENTER | ALIGN_VCENTER);
	bigBox->AddObject(smallBox.Get());

	smallCircle = GameObject::Create("~res:/Gfx/GameObjects/bluecircle");
	smallCircle->SetPosition(bigBox->GetSize());
	smallCircle->SetPivotPoint(ALIGN_HCENTER | ALIGN_VCENTER);
	bigBox->AddObject(smallCircle.Get());
	
	bigBox->SetAngle(DegToRad(30.0f));
	time = 0.0f;
	
	bigBoxParticles = GameObject::Create("~res:/Gfx/GameObjects/blueboxbig");
	bigBoxParticles->SetPivotPoint(ALIGN_HCENTER | ALIGN_VCENTER);
	bigBoxParticles->SetPosition(200, 100);
	manager->AddObject(bigBoxParticles.Get());
	
	bigBoxEmitter = new ParticleEmitterObject(); 
	bigBoxEmitter->LoadFromYaml("~res:/Particles/sparkles.yaml");
	bigBoxEmitter->SetPriority(10);
	
//	RefPtr<GameObject> centerTest  = GameObject::Create("~res:/Gfx/GameObjects/bluecircle");
//	centerTest->SetPosition(0, 0);
//	centerTest->SetPivotPoint(ALIGN_HCENTER | ALIGN_VCENTER);
//	bigBoxEmitter->AddObject(centerTest.Get());

	smallCircle->AddObject(bigBoxEmitter.Get());
	
	ParticleEmitterObject* bigBoxEmitterClone = new ParticleEmitterObject();
	bigBoxEmitterClone->SetEmitter(bigBoxEmitter->GetEmitter()->Clone());
	bigBoxEmitterClone->SetPriority(10);
	bigBoxEmitterClone->SetPosition(Vector2(300, 300));
	manager->AddObject(bigBoxEmitterClone);

	Texture::EnableMipmapGeneration();
	testSprite = Sprite::Create("~res:/Gfx/GameObjects/blueboxbig");
	Texture::DisableMipmapGeneration();
}

void TestScreen::UnloadResources()
{
	SafeRelease(testSprite);
	SafeRelease(manager);
}

void TestScreen::WillAppear()
{
}

void TestScreen::WillDisappear()
{
	
}

void TestScreen::Input(UIEvent * event)
{
	if (event->phase == UIEvent::PHASE_KEYCHAR)
		if (event->keyChar == '1')
		{
			Core::Instance()->ToggleFullscreen();
		}

}

void TestScreen::Update(float32 timeElapsed)
{
	// scaling test
	time += timeElapsed;
	float32 scale = 1.0f + 0.3f * sinf(time);
	bigBox->SetScale(scale, scale);
					
	// 
	bigBox->SetAngle(bigBox->GetAngle() + DegToRad(5.0f) * timeElapsed);
	smallBox->SetAngle(smallBox->GetAngle() + DegToRad(20.0f) * timeElapsed);
	smallCircle->SetAngle(smallCircle->GetAngle() - DegToRad(20.0f) * timeElapsed);
	manager->Update(timeElapsed);
}

void TestScreen::Draw(const UIGeometricData &geometricData)
{
	manager->Draw();


	RenderManager::Instance()->SetColor(1.0f, 1.0f, 1.0f, 1.0f);

	testSprite->SetPosition(0.0f, 200.0f);
	testSprite->Draw();

	testSprite->SetPosition(200.0f, 200.0f);
	testSprite->SetScale(0.5f, 0.5f);
	testSprite->Draw();

	testSprite->SetPosition(300.0f, 200.0f);
	testSprite->SetScale(0.3f, 0.3f);
	testSprite->Draw();

	testSprite->SetPosition(400.0f, 200.0f);
	testSprite->SetScale(0.1f, 0.1f);
	testSprite->Draw();
}
