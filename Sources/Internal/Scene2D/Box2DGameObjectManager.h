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
#ifndef __DAVAENGINE_BOX2D_GAME_OBJECT_MANAGER_H__
#define __DAVAENGINE_BOX2D_GAME_OBJECT_MANAGER_H__

#include "Base/BaseTypes.h"
#include "Base/BaseObject.h"
#include "Animation/Animation.h"
#include "Animation/AnimatedObject.h"
#include "Scene2D/GameObjectManager.h"
#include <stack>

#if !defined(__DAVAENGINE_ANDROID__)
#include <typeinfo>
#endif //#if !defined(__DAVAENGINE_ANDROID__)

#include "Box2D/Box2D.h"
#include "Scene2D/Box2DDebugDraw.h"
#include "Scene2D/Box2DTileCollider.h"

namespace DAVA 
{
    class Box2DGameObject;
    
class Box2DTileContactListener
{
public:
	virtual void BeginContact(b2Contact* contact, b2Body *ownerBody, b2Body *oppositeBody){};/// handle begin event 
	virtual void EndContact(b2Contact* contact, b2Body *ownerBody, b2Body *oppositeBody){};/// handle end event 
	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold, b2Body *ownerBody, b2Body *oppositeBody){};/// handle pre-solve event 
	virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse, b2Body *ownerBody, b2Body *oppositeBody){};/// handle post-solve event 
};
	
class Box2DGameObjectManager : public GameObjectManager, public b2ContactListener
{
protected:
	Box2DGameObjectManager(float32 stepTime, float32 pixelsInMeter, int32 velocityIterationsCount = 8, int32 positionIterationsCount = 2);
	virtual ~Box2DGameObjectManager();
	
public:
	static Box2DGameObjectManager * Create(float32 stepsPerSecond, float32 pixelsInMeter, int32 velocityIterationsCount = 8, int32 positionIterationsCount = 2);

//	Helper functions for parameters convertation
	inline float32 GetMetersInPixelRatio();
	inline float32 GetPixelsInMeterRatio();
	inline Vector2 VectorBox2DToGameManager(const b2Vec2 &box2DVector) const;
	inline b2Vec2 VectorGameManagerToBox2D(const Vector2 &objectManagerVector) const;
	inline Vector2 SizeBox2DToGameManager(const b2Vec2 &box2DVector);
	inline b2Vec2 SizeGameManagerToBox2D(const Vector2 &objectManagerVector);
	inline float32 AngleBox2DToGameManager(float32 angle);
	inline float32 AngleGameManagerToBox2D(float32 angle);
	
	virtual void EnableDebugDraw();
	
	virtual void Update(float32 timeElapsed);
	virtual void Draw();
	
	inline void SetTileCollider(Box2DTileCollider *newTileCollider);
	inline Box2DTileCollider *GetTileCollider();

	inline void SetTileContactListener(Box2DTileContactListener *newTileCont);

	b2World * box2DWorld;
	
	virtual void BeginContact(b2Contact* contact);/// handle begin event 
	virtual void EndContact(b2Contact* contact);/// handle end event 
	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);/// handle pre-solve event 
	virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);/// handle post-solve event
    
    virtual void RemoveObject(Box2DGameObject *_object);
	
protected:
	Box2DDebugDraw * debugDraw;
	
	Box2DTileCollider *tileCollider;
	
	Box2DTileContactListener *tileContactListener;

	float32 pixelsInMeterRatio;
	float32 metersInPixelRatio;

	int32 velocityIterations;
	int32 positionIterations;
	float32 stepTimeDelta;
	
	
	float32 currentTimeDelta;
	
	friend class GameObject;
};

	
void Box2DGameObjectManager::SetTileCollider(Box2DTileCollider *newTileCollider)
{
	SafeRelease(tileCollider);
	tileCollider = SafeRetain(newTileCollider);
}

Box2DTileCollider *Box2DGameObjectManager::GetTileCollider()
{
	return tileCollider;
}

void Box2DGameObjectManager::SetTileContactListener(Box2DTileContactListener *newTileContactListener)
{
	tileContactListener = newTileContactListener;
}

float32 Box2DGameObjectManager::GetPixelsInMeterRatio()
{
	return pixelsInMeterRatio;
}
float32 Box2DGameObjectManager::GetMetersInPixelRatio()
{
	return metersInPixelRatio;
}
	
Vector2 Box2DGameObjectManager::VectorBox2DToGameManager(const b2Vec2 &box2DVector) const
{
	return 	Vector2(box2DVector.x * pixelsInMeterRatio, -box2DVector.y * pixelsInMeterRatio);
}

b2Vec2 Box2DGameObjectManager::VectorGameManagerToBox2D(const Vector2 &objectManagerVector) const
{
	return b2Vec2(objectManagerVector.x * metersInPixelRatio, -objectManagerVector.y * metersInPixelRatio);
}

Vector2 Box2DGameObjectManager::SizeBox2DToGameManager(const b2Vec2 &box2DVector)
{
	return 	Vector2(box2DVector.x * pixelsInMeterRatio, box2DVector.y * pixelsInMeterRatio);
}
	
b2Vec2 Box2DGameObjectManager::SizeGameManagerToBox2D(const Vector2 &objectManagerVector)
{
	return b2Vec2(objectManagerVector.x * metersInPixelRatio, objectManagerVector.y * metersInPixelRatio);
}
	
float32 Box2DGameObjectManager::AngleBox2DToGameManager(float32 angle)
{
	return -angle;
}
float32 Box2DGameObjectManager::AngleGameManagerToBox2D(float32 angle)
{
	return - angle;
}

	
	

};
#endif // __DAVAENGINE_BOX2D_GAME_OBJECT_MANAGER_H__