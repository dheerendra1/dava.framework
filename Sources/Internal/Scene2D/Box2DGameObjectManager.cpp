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
#include "Scene2D/Box2DGameObjectManager.h"
#include "FileSystem/Logger.h"
#include "Collision/CollisionObject2.h"
#include "Render/RenderManager.h"
#include "Scene2D/Box2DGameObject.h"

namespace DAVA
{
	
Box2DGameObjectManager::Box2DGameObjectManager(float32 stepsPerSecond, float32 pixelsInMeter, int32 velocityIterationsCount/* = 8*/, int32 positionIterationsCount/* = 2*/)
{
	isInUpdate = false;
	
	b2Vec2 gravity;
	gravity.Set(0.0f, -9.81f);
	bool doSleep = true;
	
	box2DWorld = new b2World(gravity, doSleep);
//	box2DWorld->SetContinuousPhysics(true);
	
	velocityIterations = velocityIterationsCount;
	positionIterations = positionIterationsCount;
	stepTimeDelta = 1 / stepsPerSecond;

	debugDraw = NULL;
	currentTimeDelta = 0;
	pixelsInMeterRatio = pixelsInMeter;
	metersInPixelRatio = 1/pixelsInMeter;
	
	tileCollider = NULL;
	tileContactListener = NULL;
	
	box2DWorld->SetContactListener(this);
}
	
Box2DGameObjectManager::~Box2DGameObjectManager()
{
	RemoveAllObjects();
	SafeDelete(debugDraw);
	SafeDelete(box2DWorld);
}
	
Box2DGameObjectManager * Box2DGameObjectManager::Create(float32 stepTime, float32 pixelsInMeter, int32 velocityIterationsCount/* = 8*/, int32 positionIterationsCount/* = 2*/)
{
	Box2DGameObjectManager * manager = new Box2DGameObjectManager(stepTime, pixelsInMeter, velocityIterationsCount, positionIterationsCount);
	return manager;
}
	
void Box2DGameObjectManager::EnableDebugDraw()
{
	debugDraw = new Box2DDebugDraw();
	debugDraw->SetFlags( b2DebugDraw::e_shapeBit | 
						b2DebugDraw::e_jointBit | 
//						b2DebugDraw::e_aabbBit | 
						b2DebugDraw::e_pairBit | 
						b2DebugDraw::e_centerOfMassBit); 
	debugDraw->SetPTDRatio(pixelsInMeterRatio);
}

//void Box2DGameObjectManager::SetPTDRatio(float32 _ptdRatio)
//{
//	ptdRatio = _ptdRatio;
//}

//void Box2DGameObjectManager::AddObject(GameObject * _object)
//{
//	b2Body* body = _object->GetUserData();
//	world->add
//	GameObjectManager::AddObject(_object);
//}

void Box2DGameObjectManager::Update(float32 timeElapsed)
{
	currentTimeDelta += timeElapsed;
	while(currentTimeDelta >= stepTimeDelta) 
	{
		// Instruct the world to perform a single step of simulation. It is
		// generally best to keep the time step and iterations fixed.
		box2DWorld->Step(stepTimeDelta, velocityIterations, positionIterations);
		//box2DWorld->ClearForces(); it's done by default after Step()
		
		for(b2Body* b = box2DWorld->GetBodyList(); b; b = b->GetNext())
		{
			if(!b->IsTiled()) 
			{
				GameObject *gameObject = (GameObject*) b->GetUserData();
				if(gameObject != NULL)
				{
					gameObject->SetPosition(VectorBox2DToGameManager(b->GetPosition()));
					gameObject->SetAngle(-b->GetAngle());					
				}
			}
		}
		
		GameObjectManager::Update(stepTimeDelta);

		currentTimeDelta -= stepTimeDelta;
	}
}
	
void Box2DGameObjectManager::Draw()
{
	GameObjectManager::Draw();

	if(debugDraw)
	{
        RenderManager::Instance()->PushDrawMatrix();
        RenderManager::Instance()->SetDrawTranslate(cameraPosition);
        RenderManager::Instance()->SetDrawScale(cameraScale);

		//debugDraw->SetCameraPos(drawState.position);
        //debugDraw->SetPTDRatio(pixelsInMeterRatio * GetCameraScale());
		box2DWorld->SetDebugDraw(debugDraw);
		box2DWorld->DrawDebugData();

        RenderManager::Instance()->PopDrawMatrix();
	}
}
	
void Box2DGameObjectManager::BeginContact(b2Contact* contact)
{
	b2Body *b2B = contact->GetFixtureA()->GetBody();
	if(!b2B->IsTiled())
	{
		Box2DGameObject *b = (Box2DGameObject *)b2B->GetUserData();
		if (b->listenBeginContact) 
		{
			b->BeginContact(contact, b2B, contact->GetFixtureB()->GetBody());
		}
	}
	else 
	{
		if (tileContactListener) 
		{
			tileContactListener->BeginContact(contact, b2B, contact->GetFixtureB()->GetBody());
		}
	}

	b2B = contact->GetFixtureB()->GetBody();
	if(!b2B->IsTiled())
	{
		Box2DGameObject *b = (Box2DGameObject *)b2B->GetUserData();
		if (b->listenBeginContact) 
		{
			b->BeginContact(contact, b2B, contact->GetFixtureA()->GetBody());
		}
	}
	else 
	{
		if (tileContactListener) 
		{
			tileContactListener->BeginContact(contact, b2B, contact->GetFixtureA()->GetBody());
		}
	}
}
void Box2DGameObjectManager::EndContact(b2Contact* contact)
{
	b2Body *b2B = contact->GetFixtureA()->GetBody();
	if(!b2B->IsTiled())
	{
		Box2DGameObject *b = (Box2DGameObject *)b2B->GetUserData();
		if (b->listenEndContact) 
		{
			b->EndContact(contact, b2B, contact->GetFixtureB()->GetBody());
		}
	}
	else 
	{
		if (tileContactListener) 
		{
			tileContactListener->EndContact(contact, b2B, contact->GetFixtureB()->GetBody());
		}
	}
	
	b2B = contact->GetFixtureB()->GetBody();
	if(!b2B->IsTiled())
	{
		Box2DGameObject *b = (Box2DGameObject *)b2B->GetUserData();
		if (b->listenEndContact) 
		{
			b->EndContact(contact, b2B, contact->GetFixtureA()->GetBody());
		}
	}
	else 
	{
		if (tileContactListener) 
		{
			tileContactListener->EndContact(contact, b2B, contact->GetFixtureA()->GetBody());
		}
	}
}
	
void Box2DGameObjectManager::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	b2Body *b2B = contact->GetFixtureA()->GetBody();
	if(!b2B->IsTiled())
	{
		Box2DGameObject *b = (Box2DGameObject *)b2B->GetUserData();
		if (b->listenPreSolveContact) 
		{
			b->PreSolve(contact, oldManifold, b2B, contact->GetFixtureB()->GetBody());
		}
	}
	else 
	{
		if (tileContactListener) 
		{
			tileContactListener->PreSolve(contact, oldManifold, b2B, contact->GetFixtureB()->GetBody());
		}
	}
	
	b2B = contact->GetFixtureB()->GetBody();
	if(!b2B->IsTiled())
	{
		Box2DGameObject *b = (Box2DGameObject *)b2B->GetUserData();
		if (b->listenPreSolveContact) 
		{
			b->PreSolve(contact, oldManifold, b2B, contact->GetFixtureA()->GetBody());
		}
	}
	else 
	{
		if (tileContactListener) 
		{
			tileContactListener->PreSolve(contact, oldManifold, b2B, contact->GetFixtureA()->GetBody());
		}
	}
}
	
void Box2DGameObjectManager::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
	b2Body *b2B = contact->GetFixtureA()->GetBody();
	if(!b2B->IsTiled())
	{
		Box2DGameObject *b = (Box2DGameObject *)b2B->GetUserData();
		if (b->listenPostSolveContact) 
		{
			b->PostSolve(contact, impulse, b2B, contact->GetFixtureB()->GetBody());
		}
	}
	else 
	{
		if (tileContactListener) 
		{
			tileContactListener->PostSolve(contact, impulse, b2B, contact->GetFixtureB()->GetBody());
		}
	}
	
	b2B = contact->GetFixtureB()->GetBody();
	if(!b2B->IsTiled())
	{
		Box2DGameObject *b = (Box2DGameObject *)b2B->GetUserData();
		if (b->listenPostSolveContact) 
		{
			b->PostSolve(contact, impulse, b2B, contact->GetFixtureA()->GetBody());
		}
	}
	else 
	{
		if (tileContactListener) 
		{
			tileContactListener->PostSolve(contact, impulse, b2B, contact->GetFixtureA()->GetBody());
		}
	}
}

void Box2DGameObjectManager::RemoveObject(Box2DGameObject * _object)
{
    if(b2Body *body = _object->GetBox2DBody())
        body->SetActive(false);
    GameObjectManager::RemoveObject(_object);
}
};