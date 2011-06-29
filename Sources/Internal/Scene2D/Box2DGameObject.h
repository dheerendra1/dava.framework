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
#ifndef __DAVAENGINE_BOX2D_GAME_OBJECT_H__
#define __DAVAENGINE_BOX2D_GAME_OBJECT_H__ 

#include "Base/BaseMath.h"
#include "Scene2D/GameObject.h"
#include "Render/2D/Sprite.h"
//#include "Animation/AnimatedObject.h"
//#include "Animation/Interpolation.h"
#include "Box2D/Box2D.h"

//#define DRAW_COLLISIONS


namespace DAVA
{
	
	
	class Box2DGameObjectManager;
	/**
	 \ingroup scene2d
	 \brief base class in 2d scene hierarchy with phisycs
	 */
	class Box2DGameObject : public GameObject
	{
	public:
		Box2DGameObject();

		virtual void Box2DWorldDidInit();///it's a good place to init your object body and fixtures
		virtual void Box2DWorldWillDeinit();///after this method end box2DBody will be destroyed

		virtual void SetManager(GameObjectManager * _manager);

		///all bodyDef parameters (position, size, etc.) must income in box2D metrices
		void InitBox2DBody(const b2BodyDef &bodyDef);
		b2Fixture* AddFixture(const b2FixtureDef &fixtureDef);
		b2Fixture* AddFixture(const b2Shape &shape, float32 density = 0);//density is 0 for static bodies
		
		inline b2Body *GetBox2DBody()
		{
			return box2DBody;
		}
		
		inline Box2DGameObjectManager *B2Manager()///just a helper function for fast pointer cast
		{
			return (Box2DGameObjectManager *)manager;
		}
		
		virtual void BeginContact(b2Contact* contact, b2Body *ownerBody, b2Body *oppositeBody);/// handle begin event 
		virtual void EndContact(b2Contact* contact, b2Body *ownerBody, b2Body *oppositeBody);/// handle end event 
		virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold, b2Body *ownerBody, b2Body *oppositeBody);/// handle pre-solve event 
		virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse, b2Body *ownerBody, b2Body *oppositeBody);/// handle post-solve event 

		bool listenBeginContact;
		bool listenEndContact;
		bool listenPreSolveContact;
		bool listenPostSolveContact;

		b2Body *box2DBody;
	protected:
		virtual ~Box2DGameObject();
		b2World *box2DWorld;
	};
};
#endif
