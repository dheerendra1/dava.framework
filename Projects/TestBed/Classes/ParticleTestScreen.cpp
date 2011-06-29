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
#include "ParticleTestScreen.h"
#include "AppScreens.h"

void ParticleTestScreen::LoadResources()
{
	particleEmitters[0] = new ParticleEmitter();	
	particleEmitters[1] = new ParticleEmitter();	
	
	/*uint64 t1 = SystemTimer::Instance()->AbsoluteMS();
	YamlParser * parser = YamlParser::Create("~res:/Screens/SettingsMenu.yaml");
	
	YamlNode * node = parser->GetRootNode();
	//node->Print(0);
	
	Logger::Debug("%d", node->GetCount());
	for (int32 i = 0; i < node->GetCount(); ++i)
	{
		YamlNode * subNode = node->Get(i); 
		//subNode->Print(0);
	}
	
	SafeRelease(parser);
	uint64 time = SystemTimer::Instance()->AbsoluteMS() - t1;
	Logger::Debug("load %lld ms", time);*/

	/*
	ParticleLayer * layer = new ParticleLayer();
	particleEmitters[0]->AddLayer(layer);
	layer->InitWithDefaultValues();
	
	Sprite * sprite = Sprite::Create("~res:/Gfx/Sparkles/basic_blur");
	sprite->SetPivotPoint(sprite->GetWidth() / 2.0f, sprite->GetHeight() / 2.0f);
	layer->SetSprite(sprite);
	SafeRelease(sprite);
	*/
	//uint64 t1 = SystemTimer::Instance()->AbsoluteMS();
	particleEmitters[0]->LoadFromYaml("~res:/Particles/Sparkles/sparkles_linda_menu.yaml");
	particleEmittersCount = 1;
	//uint64 time = SystemTimer::Instance()->AbsoluteMS() - t1;
	
	
//	particleEmitters[0]->LoadFromYaml("~res:/Particles/Sparkles/cm_lightning1.yaml");
//	particleEmittersCount = 1;
	
//	particleEmitters[0]->LoadFromYaml("~res:/Particles/Sparkles/sparkles_linda_menu.yaml");
//	particleEmittersCount = 1;
	
//	particleEmitters[0]->LoadFromYaml("~res:/Particles/Sparkles/sparkles_clockmaster_otkat_1.yaml");
//	particleEmittersCount = 1;
	
// double emitter
//	particleEmitters[0]->LoadFromYaml("~res:/Particles/Sparkles/sparkles_linda_2.yaml");
//	particleEmitters[1]->LoadFromYaml("~res:/Particles/Sparkles/sparkles_linda_2_op.yaml");
//	particleEmittersCount = 2;

	
//	particleEmitters[0]->LoadFromYaml("~res:/Particles/Sparkles/perf_sparkles.yaml");
//	particleEmitters[0]->LoadFromYaml("~res:/Particles/Sparkles/sparkles.yaml");
//	particleEmitters[0]->LoadFromYaml("~res:/Particles/Sparkles/base_fire_fall.yaml");
//	particleEmitters[0]->LoadFromYaml("~res:/Particles/Sparkles/shoot_rotating_smoke.yaml");
//	particleEmittersCount = 1;
//	particleEmitters[0]->LoadFromYaml("~res:/Particles/Sparkles/streaks.yaml");
//	particleEmitters[0]->LoadFromYaml("~res:/Particles/Sparkles/laser_smoke.yaml");
	
	UIYamlLoader::Load(this, "~res:/Screens/particle_test_screen.yaml");
	
	particleCount = dynamic_cast<UIStaticText*>(this->FindByName("particleCountTextField"));
	fps = dynamic_cast<UIStaticText*> (this->FindByName("fpsTextField"));

	particleEmitters[0]->SetPosition(Vector2(100, 100));
	textUpdateTime = 0.0f;
	
	cursor = Sprite::Create("~res:/Gfx/Effects/flower");
	plasmaTime = 0;
}

void ParticleTestScreen::UnloadResources()
{
	SafeRelease(cursor);
	SafeRelease(particleEmitters[0]);
	SafeRelease(particleEmitters[1]);
	RemoveAllControls();
}

void ParticleTestScreen::WillAppear()
{
}

void ParticleTestScreen::WillDisappear()
{
	
}

void ParticleTestScreen::Input(UIEvent * touch)
{
	if ((touch->phase == UIEvent::PHASE_DRAG) && (touch->tid == UIEvent::BUTTON_1))
	{
		for (int32 pi = 0; pi < particleEmittersCount; ++pi)
		{
			particleEmitters[pi]->SetPosition(Vector2(touch->point.x, touch->point.y));
		}
	}

	if ((touch->phase == UIEvent::PHASE_ENDED) && (touch->tid == UIEvent::BUTTON_2))
	{
		for (int32 pi = 0; pi < particleEmittersCount; ++pi)
		{
			particleEmitters[pi]->Restart(true);
		}


		UIMoveInTransition * transition = new UIMoveInTransition();
		UIScreenManager::Instance()->SetScreen(SCREEN_RENDERTARGET_TEST, transition);
		SafeRelease(transition);
		//Core::Instance()->ToggleFullscreen();
	}
	
	
	//if (touch->phase == UIEvent::PHASE_DRAG)
	
	//if (touch->phase == UIEvent::PHASE_MOVE)
	mousePos = touch->point;
	//Logger::Debug("%f %f", touch->point.x, touch->point.y);
}

void ParticleTestScreen::Update(float32 timeElapsed)
{
	for (int32 pi = 0; pi < particleEmittersCount; ++pi)
		particleEmitters[pi]->Update(timeElapsed);	
	
	textUpdateTime += timeElapsed;
	if (textUpdateTime > 0.5f)
	{
		int32 cnt = 0;
		for (int32 pi = 0; pi < particleEmittersCount; ++pi)
			cnt += particleEmitters[pi]->GetParticleCount();
		
		particleCount->SetText(Format(L"pc: %d", cnt));
		fps->SetText(Format(L"f: %.0f", 1.0 / timeElapsed));
		textUpdateTime = 0.0f;
	}
	
	plasmaTime += timeElapsed;
}




void ParticleTestScreen::Draw(const UIGeometricData &geometricData)
{
//	Vector2 vertices[20 * 20]; 
//	Color colors[20 * 20];
//	uint16 indices[20 * 20 * 6];
//	int32 cnt = 0;
//	
//	uint16 * indptr = indices;
//	for (int32 x = 0; x < 20; ++x)
//		for (int32 y = 0; y < 20; ++y)
//		{
//			vertices[y * 20 + x] = Vector2(30 + x * 10, 30 + y * 10);
//			colors[y * 20 + x] = Color(x / 20.0f, y / 20.0f, 1.0f, 1.0f);
//			if ((x < 19) && (y < 19))
//			{
//				*indptr++ = y * 20 + x;
//				*indptr++ = y * 20 + (x + 1);
//				*indptr++ = (y + 1) * 20 + (x);
//				*indptr++ = y * 20 + (x + 1);
//				*indptr++ = (y + 1) * 20 + x;
//				*indptr++ = (y + 1) * 20 + (x + 1);
//				cnt += 6;
//			}
//		}
//	for (int32 x = 0; x < 20; ++x)
//		for (int32 y = 0; y < 20; ++y)
//		{	
//			float32 red = (127 + 64 * sin(1 * (x + plasmaTime)) +  64 * sin(3 * (y + plasmaTime))) / 255.0f;
//			float32 green = (127 + 64 * sin(3 * (y + plasmaTime)) + 64 * sin(8 * (y + plasmaTime))) / 255.0f;
//			float32 blue = (127 + 64 * sin(5 * (x + plasmaTime)) + 64 * sin(5 * (x + y + plasmaTime))) / 255.0f;
//			colors[y * 20 + x].r = red; //floorf(sinf(x/35)*128 + sinf(y/28) + sinf((x+y)/16)*64) / 255.0f;
//			colors[y * 20 + x].g = green; //floorf(sinf(x/35)*128 + sinf(y/28) + sinf((x+y)/16)*64) / 255.0f;
//			colors[y * 20 + x].b = blue; //floorf(sinf(x/35)*128 + sinf(y/28) + sinf((x+y)/16)*64) / 255.0f;
//		}
//	
//	RenderManager::Instance()->SetVertexPointer(2, TYPE_FLOAT, 0, vertices);
//	RenderManager::Instance()->SetColorPointer(4, TYPE_FLOAT, 0, colors);
//	//RenderManager::Instance()->SetTexture(texture);
//	RenderManager::Instance()->EnableTexturing(false);
//	RenderManager::Instance()->EnableColorArray(true);
//	RenderManager::Instance()->FlushState();	
//	RenderManager::Instance()->DrawElements(PRIMITIVETYPE_TRIANGLELIST, cnt, EIF_16, indices);
//	RenderManager::Instance()->EnableColorArray(false);
//	RenderManager::Instance()->EnableTexturing(true);
//
	
	
	for (int32 pi = 0; pi < particleEmittersCount; ++pi)
	{
		particleEmitters[pi]->Draw();	
//		Rect r(particleEmitters[pi]->GetPosition().x - 125, particleEmitters[pi]->GetPosition().y - 125, 250, 250);
//		RenderManager::Instance()->DrawRect(r);	
//
//		r = Rect(particleEmitters[pi]->GetPosition().x - 5, particleEmitters[pi]->GetPosition().y - 5, 10, 10);
//		RenderManager::Instance()->FillRect(r);	
	}
	RenderManager::Instance()->SetColor(1.0f, 1.0f, 1.0f, 1.0f);

	//mousePos = Core::Instance()->GetMouseLocation();
	cursor->SetPosition(mousePos.x, mousePos.y);
	cursor->Draw();
}
