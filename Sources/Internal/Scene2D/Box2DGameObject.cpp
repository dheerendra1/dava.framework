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
#include "Scene2D/Box2DGameObject.h"
#include "Scene2D/Box2DGameObjectManager.h"

namespace DAVA
{
Box2DGameObject::Box2DGameObject()
	: box2DBody(NULL)
	, listenBeginContact(false)
	, listenEndContact(false)
	, listenPreSolveContact(false)
	, listenPostSolveContact(false)
{
}
	
Box2DGameObject::~Box2DGameObject()
{
	if (box2DBody) 
	{
		box2DWorld->DestroyBody(box2DBody);
		box2DBody = NULL;
	}
}
	
void Box2DGameObject::InitBox2DBody(const b2BodyDef &bodyDef)
{
	DVASSERT_MSG(manager, "You need to set manager first");
	DVASSERT_MSG(box2DWorld, "Youor box2d world is not set");
	box2DBody = box2DWorld->CreateBody(&bodyDef);
	box2DBody->SetUserData(this);
}
	
b2Fixture* Box2DGameObject::AddFixture(const b2FixtureDef &fixtureDef)
{
	DVASSERT_MSG(box2DBody, "You need to init body first");
	return box2DBody->CreateFixture(&fixtureDef);	
}

b2Fixture* Box2DGameObject::AddFixture(const b2Shape &shape, float32 density/* = 0*/)
{
	DVASSERT_MSG(box2DBody, "You need to init body first");
	return box2DBody->CreateFixture(&shape, density);
}

void Box2DGameObject::Box2DWorldDidInit()
{
}
	
void Box2DGameObject::Box2DWorldWillDeinit()
{
}
	
void Box2DGameObject::SetManager(GameObjectManager * _manager)
{
	DVASSERT_MSG(!_manager || dynamic_cast<Box2DGameObjectManager *> (_manager), "Box2DGameObject can be added only to the Box2DGameObjectManager");
//	DVASSERT_MSG(!_manager || !manager || ((Box2DGameObjectManager *)_manager)->box2DWorld == box2DWorld, "You can't move phisycal body to the another manager");
	
	manager = _manager;
	if (manager) 
	{
		box2DWorld = ((Box2DGameObjectManager *)manager)->box2DWorld;
		Box2DWorldDidInit();
		
	}
	else
	{
		if (box2DBody)
		{
			Box2DWorldWillDeinit();
			box2DWorld->DestroyBody(box2DBody);
			box2DBody = NULL;
			box2DWorld = NULL;
		}
	}
	//TODO: Добваить активацию и деактивацию боди при добавлении и убирании из менежера.
}
	
void Box2DGameObject::BeginContact(b2Contact* contact, b2Body *ownerBody, b2Body *oppositeBody)
{
}
void Box2DGameObject::EndContact(b2Contact* contact, b2Body *ownerBody, b2Body *oppositeBody)
{
}
void Box2DGameObject::PreSolve(b2Contact* contact, const b2Manifold* oldManifold, b2Body *ownerBody, b2Body *oppositeBody)
{
}
void Box2DGameObject::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse, b2Body *ownerBody, b2Body *oppositeBody)
{
}	
};