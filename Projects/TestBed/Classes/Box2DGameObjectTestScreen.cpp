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
#include "Box2DGameObjectTestScreen.h"
#include "TouchEffectsControl.h"


//#define manager->GetPTDRatio() 64.0f
void Box2DGameObjectTestScreen::LoadResources()
{
//	DVASSERT(this == 0);
//	int32 t;
//	t = 0;
//	RenderManager::Instance()->SetFPS(30.0f);
//	
//	manager = Box2DGameObjectManager::Create();
//	manager->SetPTDRatio(64.0f);
//	
//	b2World * world = manager->world;
//	manager->EnableDebugDraw();
//	
//	// Define the ground body.
//	b2BodyDef groundBodyDef;
//	groundBodyDef.position.Set(0, 0); // bottom-left corner
//	
//	// Call the body factory which allocates memory for the ground body
//	// from a pool and creates the ground box shape (also from a pool).
//	// The body is also added to the world.
//	b2Body* groundBody = world->CreateBody(&groundBodyDef);
//	
//	// Define the ground box shape.
//	b2EdgeShape groundBox;
//	
//	
//	// bottom
//	groundBox.Set(b2Vec2(0,0), b2Vec2(480.0f/manager->GetPTDRatio(),0));
//	groundBody->CreateFixture(&groundBox, 0);
//	
//	// top
//	groundBox.Set(b2Vec2(0, 320.0f/manager->GetPTDRatio()), b2Vec2(480.0f/manager->GetPTDRatio(),320.0f/manager->GetPTDRatio()));
//	groundBody->CreateFixture(&groundBox, 0);
//	
//	// left
//	groundBox.Set(b2Vec2(0, 320.0f/manager->GetPTDRatio()), b2Vec2(0,0));
//	groundBody->CreateFixture(&groundBox, 0);
//	
//	// right
//	groundBox.Set(b2Vec2(480.0f/manager->GetPTDRatio(),320.0f/manager->GetPTDRatio()), b2Vec2(480.0f/manager->GetPTDRatio(),0));
//	groundBody->CreateFixture(&groundBox, 0);
//	
//	
//	bigBox = GameObject::Create("~res:/Gfx/GameObjects/blueboxbig");
//	bigBox->SetPivotPoint(ALIGN_HCENTER | ALIGN_VCENTER);
//	bigBox->SetPosition(100, 100);
//	manager->AddObject(bigBox.Get());
//
//	
//	b2BodyDef bodyDef;
//	bodyDef.type = b2_dynamicBody;
//	
//	//CGPoint p = physicalView.center;
//	//CGPoint boxDimensions = CGPointMake();
//	
//	bodyDef.position.Set(200/manager->GetPTDRatio(), (320.0 - 100)/manager->GetPTDRatio());
//	bodyDef.angle = DegToRad(0);
//	bodyDef.userData = bigBox.Get();
//	
//	// Tell the physics world to create the body
//	b2Body *body = world->CreateBody(&bodyDef);
//	
//	// Define another box shape for our dynamic body.
//	b2PolygonShape dynamicBox;
//	
//	dynamicBox.SetAsBox(bigBox->GetSprite()->GetWidth()/manager->GetPTDRatio()/2.0,bigBox->GetSprite()->GetHeight()/manager->GetPTDRatio()/2.0);
//	
//	// Define the dynamic body fixture.
//	b2FixtureDef fixtureDef;
//	fixtureDef.shape = &dynamicBox;
//	fixtureDef.density = 3.0f;
//	fixtureDef.friction = 0.3f;
//	fixtureDef.restitution = 0.5f; // 0 is a lead ball, 1 is a super bouncy ball
//	body->CreateFixture(&fixtureDef);
//	
//	// a dynamic body reacts to forces right away
//	body->SetType(b2_dynamicBody);
//	
//	
//	
//	
//	smallBox = GameObject::Create("~res:/Gfx/GameObjects/bluebox");
//	smallBox->SetPivotPoint(ALIGN_HCENTER | ALIGN_VCENTER);
//	manager->AddObject(smallBox.Get());
//	
//	
//	bodyDef.type = b2_dynamicBody;
//	bodyDef.allowSleep = false;
//	//CGPoint p = physicalView.center;
//	//CGPoint boxDimensions = CGPointMake();
//	
//	bodyDef.position.Set(200/manager->GetPTDRatio(), (320.0 - 50)/manager->GetPTDRatio());
//	bodyDef.angle = DegToRad(0);
//	bodyDef.userData = smallBox.Get();
//	
//	// Tell the physics world to create the body
//	b2Body *body2 = world->CreateBody(&bodyDef);
//	
//	// Define another box shape for our dynamic body.
//	//b2PolygonShape dynamicBox;
//	
//	dynamicBox.SetAsBox(smallBox->GetSprite()->GetWidth()/manager->GetPTDRatio()/2.0,smallBox->GetSprite()->GetHeight()/manager->GetPTDRatio()/2.0);
//	
//	// Define the dynamic body fixture.
//	//b2FixtureDef fixtureDef;
//	fixtureDef.shape = &dynamicBox;
//	fixtureDef.density = 3.0f;
//	fixtureDef.friction = 0.3f;
//	fixtureDef.restitution = 0.5f; // 0 is a lead ball, 1 is a super bouncy ball
//	body2->CreateFixture(&fixtureDef);
//	
//	// a dynamic body reacts to forces right away
//	body2->SetType(b2_dynamicBody);
//	
//	
//	
//	smallCircle = GameObject::Create("~res:/Gfx/GameObjects/bluecircle");
//	smallCircle->SetPivotPoint(ALIGN_HCENTER | ALIGN_VCENTER);
//	manager->AddObject(smallCircle.Get());
//	
//	
//	bodyDef.type = b2_dynamicBody;
//	
//	bodyDef.position.Set(300/manager->GetPTDRatio(), (320.0 - 100)/manager->GetPTDRatio());
//	bodyDef.angle = DegToRad(0);
//	bodyDef.userData = smallCircle.Get();
//	
//	// Tell the physics world to create the body
//	b2Body * body3 = world->CreateBody(&bodyDef);
//	
//	// Define another box shape for our dynamic body.
//	b2CircleShape circle;
//	
//	//circle.Set(smallBox->GetSprite()->GetWidth()/manager->GetPTDRatio()/2.0,smallBox->GetSprite()->GetHeight()/manager->GetPTDRatio()/2.0);
//	circle.m_radius = smallCircle->GetSprite()->GetWidth()/manager->GetPTDRatio()/2.0;
//	
//	// Define the dynamic body fixture.
//	//b2FixtureDef fixtureDef;
//	fixtureDef.shape = &circle;
//	fixtureDef.density = 3.0f;
//	fixtureDef.friction = 0.3f;
//	fixtureDef.restitution = 0.5f; // 0 is a lead ball, 1 is a super bouncy ball
//	body3->CreateFixture(&fixtureDef);
//	
//	// a dynamic body reacts to forces right away
//	body3->SetType(b2_dynamicBody);	
//	//Accelerometer::Instance()->Enable();
}

void Box2DGameObjectTestScreen::UnloadResources()
{
	manager = 0;
	
	bigBox = 0;
	smallBox = 0;
	smallCircle = 0;
}

void Box2DGameObjectTestScreen::WillAppear()
{
}

void Box2DGameObjectTestScreen::WillDisappear()
{
	
}

void Box2DGameObjectTestScreen::Input(UIEvent * touch)
{
 
}

void Box2DGameObjectTestScreen::Update(float32 timeElapsed)
{

//
//	const Vector3 & accData = Accelerometer::Instance()->GetAccelerationData();
//	b2Vec2 gravity;
//	gravity.Set( accData.y * 9.81,  -accData.x * 9.81 );
//	manager->world->SetGravity(gravity);
//	
//	for (b2Body* b = manager->world->GetBodyList(); b; b = b->GetNext())
//		b->SetAwake(true);
	
	// scaling test
	manager->Update(timeElapsed);

}

void Box2DGameObjectTestScreen::Draw(const UIGeometricData &geometricData)
{
	manager->Draw();
}
