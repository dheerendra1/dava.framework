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
#ifndef __DAVAENGINE_COLLISIONS_H__
#define __DAVAENGINE_COLLISIONS_H__

#include "Base/StaticSingleton.h"
#include "Math/Vector.h"
#include "Math/Math2D.h"

namespace DAVA
{
#define MAX_CONTACT_POINTS 2

class ContactManifold2
{
public:
	ContactManifold2()
	{
		count = 0;
	}
	
	int32	count;
	Vector2 contactPoints[MAX_CONTACT_POINTS];
	
	int32 GetContactCount() { return count; };
	const Vector2 & GetContactPoint(int32 k) { return contactPoints[k];}
	
	void AddContactPoint(const Vector2 & cp)
	{
		if (count < MAX_CONTACT_POINTS)
		{
			contactPoints[count++] = cp;
		}
	}
	
	void MergeWithManifold(const ContactManifold2 & manifoldToMerge)
	{
		for (int mci = 0; mci < manifoldToMerge.count; ++mci)
		{
			AddContactPoint(manifoldToMerge.contactPoints[mci]);
		}
	}
	
	Vector2 AverageContactPoint()
	{
		Vector2 avC;
		for (int32 c = 0; c < count; ++c)
		{
			avC += contactPoints[c];
		}
		avC /= (float32)count;
		return avC;
	};
};

//! Class with static functions that checks intersections between different primitives
class Collisions : public StaticSingleton<Collisions>
{
public:
	Collisions();
	~Collisions();
	
	//! Check if 2D Line intersects another 2D line
	//! save result of intersection to inters variable
	//! return if true intersection occured, and false if lines parralel
	bool Is2DLineIntersects2DLine
										(const Vector2 & p1, const Vector2 & p2, 
										 const Vector2 & p3, const Vector2 & p4, 
										 Vector2 & inters);
	

	//! Check if 2D segment intersects another 2D segment
	//! save result of intersection to inters variable
	//! return if true intersection occured
	bool IsSegmentIntersectsSegment(	
											  const Vector2 & p1, const Vector2 & p2, 
											  const Vector2 & p3, const Vector2 & p4, 
											  Vector2 & inters);

	//! Check if 2D polygon intersects another 2D polygon
	//! This function can check if 2 primitives collide right now at this current update moment
	//! to check real point of 
	//! return if true intersection occured
	bool Is2DPolygonIntersects2DPolygon(const Polygon2 & poly1,	
											   const Polygon2 & poly2,
											   Vector2 & inters);
	
	
	void ProjectPolygon(const Vector2 & axis, Polygon2 & poly, float32 & min, float32 & max);
	// Calculate the distance between [minA, maxA] and [minB, maxB]
	// The distance will be negative if the intervals overlapstatic float32 IntervalDistance(float32 minA, float32 maxA, float32 minB, float32 maxB);
	float32 IntervalDistance(float32 minA, float32 maxA, float32 minB, float32 maxB);
	
	/*
		TODO: 
		Pass transforms to avoid transformating of the polygons for the objects and just use their transforms
	 
	 */
	bool IsPolygonIntersectsPolygon(Polygon2 & poly1,	
									Polygon2 & poly2);
	
	
	void FindIntersectionPolygonToPolygon(Polygon2 & poly1,	
										  Polygon2 & poly2, 
										  ContactManifold2 & manifold);
	
	void FindIntersectionLineToCircle(const Vector2 & p1, const Vector2 & p2, const Circle & circle, ContactManifold2 & manifold);
	void FindIntersectionPolygonToCircle(const Polygon2 & poly1,const Circle & circle, ContactManifold2 & manifold);
	void FindIntersectionCircleToCircle(const Circle & c1, const Circle & c2, ContactManifold2 & manifold);								 
	
	void AddSeparationAxis(const Vector2 & axis);
	Vector<Vector2> separationAxes;
};

}; // end of namespace DAVA



#endif // __DAVAENGINE_COLLISIONS_H__