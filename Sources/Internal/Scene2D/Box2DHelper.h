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
 #ifndef __DAVAENGINE_BOX2D_HELPER_H__
#define __DAVAENGINE_BOX2D_HELPER_H__

#include "Base/BaseMath.h"
#include "Scene2D/Box2DGameObjectManager.h"
#include "Box2D/Box2D.h"
#include "Base/BaseTypes.h"


namespace DAVA
{
	
	
	class Box2DGameObjectManager;
	/**
	 \ingroup scene2d
	 \brief All Box2DHelper methods gets arguments in framework metrics and convert them to the Box2D format
	 */
	class Box2DHelper
	{
	public:
		inline static b2EdgeShape EdgeShape(Box2DGameObjectManager *manager
											, const Vector2 &point1, const Vector2 &point2);
		inline static b2CircleShape CircleShape(Box2DGameObjectManager *manager
												, const float32 radius
												, const Vector2 &centerPosition = Vector2(0.0f, 0.f));

		inline static b2PolygonShape BoxShape(Box2DGameObjectManager *manager
												, const Vector2 &size, float32 angle = 0
											  , const Vector2 &centerPosition = Vector2(0.0f, 0.f));///this method gets the full box size excepts the Box2D style

		inline static b2PolygonShape PolygonShape(Box2DGameObjectManager *manager
												  , const Vector2 *verticies, int32 vertCount);
		

		inline static b2FixtureDef FixtureDef(Box2DGameObjectManager *manager
											  , const b2Shape &shape
											  , float32 density = 0.0f
											  , float32 friction = 0.2f, float32 restitution = 0.0f
											  , const b2Filter &filter = GetDefaultCollisionFilter());

		inline static b2FixtureDef SensorFixtureDef(Box2DGameObjectManager *manager
											  , const b2Shape &shape, float32 density = 0.0f
											  , float32 friction = 0.2f, float32 restitution = 0.0f
											  , const b2Filter &filter = GetDefaultCollisionFilter());
		
		
		inline static b2BodyDef StaticBodyDef(Box2DGameObjectManager *manager
											  , const Vector2 &position, float32 angle = 0.0f
											  , bool active = true, bool allowSleep = true, bool awake = true);
		inline static b2BodyDef KinematicBodyDef(Box2DGameObjectManager *manager
												, const Vector2 &position, float32 angle = 0.0f
												 , const Vector2 &linearVelocity = Vector2(0.0f, 0.0f), float32 angularVlocity = 0.0f
												 , bool active = true, bool allowSleep = true, bool awake = true);
		
		inline static b2BodyDef DynamicBodyDef(Box2DGameObjectManager *manager
												 , const Vector2 &position, float32 angle = 0.0f
												 , bool fixedRotation = false, float32 inertiaScale = 1.0f
												 , const Vector2 &linearVelocity = Vector2(0.0f, 0.0f), float32 angularVlocity = 0.0f
												 , float32 linearDamping = GetDefaultLinerDamping(), float32 angularDamping = GetDefaultAngularDamping()
												 , bool active = true, bool allowSleep = true, bool awake = true);

		inline static b2BodyDef DynamicBulletBodyDef(Box2DGameObjectManager *manager
											   , const Vector2 &position, float32 angle = 0.0f
											   , bool fixedRotation = false, float32 inertiaScale = 1.0f
											   , const Vector2 &linearVelocity = Vector2(0.0f, 0.0f), float32 angularVlocity = 0.0f
											   , float32 linearDamping = GetDefaultLinerDamping(), float32 angularDamping = GetDefaultAngularDamping()
											   , bool active = true, bool allowSleep = true, bool awake = true);
		
		inline static void SetDefaultLinerDamping(float32 newLinerDumping);
		inline static void SetDefaultAngularDamping(float32 newAngularDamping);
		inline static float32 GetDefaultLinerDamping();
		inline static float32 GetDefaultAngularDamping();
		inline static const b2Filter &GetDefaultCollisionFilter();
		static float32 defaultAngularDamping;
		static float32 defaultLinearDamping;
		static b2Filter defaultCollisionFilter;
	};
	

	
	b2EdgeShape Box2DHelper::EdgeShape(Box2DGameObjectManager *manager, const Vector2 &point1, const Vector2 &point2)
	{
		b2EdgeShape shape;
		shape.Set(manager->VectorGameManagerToBox2D(point1), manager->VectorGameManagerToBox2D(point2));
		return shape;
	}
	
	b2CircleShape Box2DHelper::CircleShape(Box2DGameObjectManager *manager, const float32 radius
										   , const Vector2 &centerPosition/* = Vector2(0.0f, 0.f)*/)
	{
		b2CircleShape shape;
		shape.m_p = manager->VectorGameManagerToBox2D(centerPosition);
		shape.m_radius = radius * manager->GetMetersInPixelRatio();
		return shape;
	}

	b2PolygonShape Box2DHelper::BoxShape(Box2DGameObjectManager *manager
											  , const Vector2 &size, float32 angle/* = 0*/
										 , const Vector2 &centerPosition/* = Vector2(0.0f, 0.f)*/)
	{
		b2PolygonShape shape;
		b2Vec2 sv = manager->SizeGameManagerToBox2D(size);
		sv *= 0.5f;
		shape.SetAsBox(sv.x, sv.y, manager->VectorGameManagerToBox2D(centerPosition), manager->AngleGameManagerToBox2D(angle));
		return shape;
	}

	b2PolygonShape Box2DHelper::PolygonShape(Box2DGameObjectManager *manager
											 , const Vector2 *verticies, int32 vertCount)
	{
		b2PolygonShape shape;
		b2Vec2 v[b2_maxPolygonVertices];
		for (int i = 0; i < vertCount; i++) 
		{
			v[i] = manager->VectorGameManagerToBox2D(verticies[i]);
		}
		shape.Set(v, vertCount);

		return shape;
	}
	
	
	
	
	//		shape = NULL;
	//		friction = 0.2f;
	//		restitution = 0.0f;
	//		density = 0.0f;
	//		filter.categoryBits = 0x0001;
	//		filter.maskBits = 0xFFFF;
	//		filter.groupIndex = 0;
	//		isSensor = false;
	b2FixtureDef Box2DHelper::FixtureDef(Box2DGameObjectManager *manager
										  , const b2Shape &shape
										 , float32 density/* = 0.0f*/
										  , float32 friction/* = 0.2f*/, float32 restitution/* = 0.0f*/
										  , const b2Filter &filter/* = GetDefaultCollisionFilter()*/)
	{
		b2FixtureDef fixture;
		fixture.shape = &shape;
		fixture.density = density;
		fixture.friction = friction;
		fixture.restitution = restitution;
		fixture.filter = filter;
		return fixture;
	}

	
	b2FixtureDef Box2DHelper::SensorFixtureDef(Box2DGameObjectManager *manager
												, const b2Shape &shape, float32 density/* = 0.0f*/
												, float32 friction/* = 0.2f*/, float32 restitution/* = 0.0f*/
												, const b2Filter &filter/* = GetDefaultCollisionFilter()*/)
	{
		b2FixtureDef fixture;
		fixture.shape = &shape;
		fixture.density = density;
		fixture.friction = friction;
		fixture.restitution = restitution;
		fixture.filter = filter;
		fixture.isSensor = true;
		return fixture;
	}
	

	
//	position.Set(0.0f, 0.0f);
//	angle = 0.0f;
//	linearVelocity.Set(0.0f, 0.0f);
//	angularVelocity = 0.0f;
//	fixedRotation = false;
//	bullet = false;
//	gravityScale = 1.0f;
//	active = true;
//	allowSleep = true;
//	awake = true;
//	linearDamping = 0.0f;
//	angularDamping = 0.0f;

	b2BodyDef Box2DHelper::StaticBodyDef(Box2DGameObjectManager *manager
										  , const Vector2 &position, const float32 angle/* = 0*/
										  , bool active/* = true*/, bool allowSleep/* = true*/, bool awake/* = true*/)
	{
		b2BodyDef bodyDef;
		bodyDef.position = manager->VectorGameManagerToBox2D(position);
		bodyDef.angle = manager->AngleGameManagerToBox2D(angle);
		bodyDef.active = active;
		bodyDef.allowSleep = allowSleep;
		bodyDef.awake = awake;
		
		return bodyDef;
	}
	
	b2BodyDef Box2DHelper::KinematicBodyDef(Box2DGameObjectManager *manager
											 , const Vector2 &position, float32 angle/* = 0.0f*/
											 , const Vector2 &linearVelocity/* = Vector2(0.0f, 0.0f)*/, float32 angularVlocity/* = 0.0f*/
											 , bool active/* = true*/, bool allowSleep/* = true*/, bool awake/* = true*/)
	{
		b2BodyDef bodyDef;
		bodyDef.position = manager->VectorGameManagerToBox2D(position);
		bodyDef.angle = manager->AngleGameManagerToBox2D(angle);
		bodyDef.type = b2_kinematicBody;
		bodyDef.linearVelocity = manager->VectorGameManagerToBox2D(linearVelocity);
		bodyDef.angularVelocity = manager->AngleGameManagerToBox2D(angularVlocity);
		bodyDef.active = active;
		bodyDef.allowSleep = allowSleep;
		bodyDef.awake = awake;
		
		return bodyDef;
	}
	
	b2BodyDef Box2DHelper::DynamicBodyDef(Box2DGameObjectManager *manager
										   , const Vector2 &position, float32 angle/* = 0.0f*/
										   , bool fixedRotation/* = false*/, float32 gravityScale/* = 1.0f*/
										   , const Vector2 &linearVelocity/* = Vector2(0.0f, 0.0f)*/, float32 angularVlocity/* = 0.0f*/
										   , float32 linearDamping/* = GetDefaultLinerDamping()*/, float32 angularDamping/* = GetDefaultAngularDamping()*/
										   , bool active/* = true*/, bool allowSleep/* = true*/, bool awake/* = true*/)
	{
		b2BodyDef bodyDef;
		bodyDef.position = manager->VectorGameManagerToBox2D(position);
		bodyDef.angle = manager->AngleGameManagerToBox2D(angle);
		bodyDef.fixedRotation = fixedRotation;
		bodyDef.gravityScale = gravityScale;
		bodyDef.type = b2_dynamicBody;
		bodyDef.linearVelocity = manager->VectorGameManagerToBox2D(linearVelocity);
		bodyDef.angularVelocity = manager->AngleGameManagerToBox2D(angularVlocity);
		bodyDef.linearDamping = linearDamping;
		bodyDef.angularDamping = angularDamping;
		bodyDef.active = active;
		bodyDef.allowSleep = allowSleep;
		bodyDef.awake = awake;
		
		return bodyDef;
	}

	b2BodyDef Box2DHelper::DynamicBulletBodyDef(Box2DGameObjectManager *manager
										  , const Vector2 &position, float32 angle/* = 0.0f*/
										  , bool fixedRotation/* = false*/, float32 gravityScale/* = 1.0f*/
										  , const Vector2 &linearVelocity/* = Vector2(0.0f, 0.0f)*/, float32 angularVlocity/* = 0.0f*/
										  , float32 linearDamping/* = GetDefaultLinerDamping()*/, float32 angularDamping/* = GetDefaultAngularDamping()*/
										  , bool active/* = true*/, bool allowSleep/* = true*/, bool awake/* = true*/)
	{
		b2BodyDef bodyDef;
		bodyDef.position = manager->VectorGameManagerToBox2D(position);
		bodyDef.angle = manager->AngleGameManagerToBox2D(angle);
		bodyDef.fixedRotation = fixedRotation;
		bodyDef.gravityScale = gravityScale;
		bodyDef.type = b2_dynamicBody;
		bodyDef.linearVelocity = manager->VectorGameManagerToBox2D(linearVelocity);
		bodyDef.angularVelocity = manager->AngleGameManagerToBox2D(angularVlocity);
		bodyDef.linearDamping = linearDamping;
		bodyDef.angularDamping = angularDamping;
		bodyDef.active = active;
		bodyDef.allowSleep = allowSleep;
		bodyDef.awake = awake;
		bodyDef.bullet = true;
		
		return bodyDef;
	}
	
	
	const b2Filter &Box2DHelper::GetDefaultCollisionFilter()
	{
		return defaultCollisionFilter;
	}

	
	
	void Box2DHelper::SetDefaultLinerDamping(float32 newLinerDumping)
	{
		defaultLinearDamping = newLinerDumping;
	}
	void Box2DHelper::SetDefaultAngularDamping(float32 newAngularDamping)
	{
		defaultAngularDamping = newAngularDamping;
	}
	float32 Box2DHelper::GetDefaultLinerDamping()
	{
		return defaultLinearDamping;
	}
	float32 Box2DHelper::GetDefaultAngularDamping()
	{
		return defaultAngularDamping;
	}
	
};
#endif