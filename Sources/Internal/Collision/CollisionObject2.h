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
#ifndef __DAVAENGINE_COLLISION_OBJECT2_H__
#define __DAVAENGINE_COLLISION_OBJECT2_H__

#include "Base/BaseTypes.h"
#include "Base/BaseObject.h"
#include "Base/BaseMath.h"
#include "Render/2D/Sprite.h"
#include "Collision/Collisions.h"

namespace DAVA 
{
	
class CollisionObject2 : public BaseObject 
{
public:
	enum eType
	{
		TYPE_CIRCLE,
		TYPE_POLYGON,
	};
	
	CollisionObject2(eType type);
	virtual ~CollisionObject2();
	
	/*
		Collision object do not hold polygon inside his body
		So you should give him polygon that will be alive till the end of live of collision object
	 */
	void SetType(eType type);
	
	/// NOTE! Polygon2 pointer must be valid as long as CollisionObject2 lives!
	void SetPolygon(Polygon2 * p);
		
	void Update(const Sprite::DrawState & state);
	void DebugDraw();
	
	bool IsCollideWith(CollisionObject2 * collObject);
	ContactManifold2 * GetContactManifold();
	

	eType type;
	
	Vector2 position;		//	Position of the collision object pivot
	Vector2 pivot;			//	Shift of the object inside the sprite
	Vector2 scale;			//  Current scale
	float32 angle;			//  Current angle

	Circle circle;			// circle in case if it's circle collision object
	//float32 basePolygonRadius;	//	Square radius of the rotated object
	Vector2 basePolygonCenter;	// real center of the collision polygon
	//Vector2 updatedCenter;		// real center of the collision polygon
	ContactManifold2 manifold;
	
	Polygon2 * basePolygon;
	Polygon2 polygon;
	AABBox2 bbox;
	
	uint32 updateFrameIndex;
	bool collisionOnLastFrame;
	
/*	int pointsCount;
	float *points;
	
	DAVA::AABBox bbox;
	
	float collisionRadius;
	float centerX, centerY;
	
	// info from base game object	
	float *collisionVertices;
	float collX;
	float collY;
	
	float xCoord;
	float yCoord;
	float angle; */
		
	/*bool IsPointsInsidePoints(float* points1, int cnt1, float*points2, int cnt2);
	void CalcCollisionPoints();
	bool IsCollideWith(CollisionObject2 * collObject);
	void DrawCollision(); */
};


	
};

#endif 
