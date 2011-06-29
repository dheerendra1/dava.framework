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
#include "Base/BaseTypes.h"
#include "Base/BaseMath.h"
#include "Collision/Collisions.h"
#include "Render/RenderHelper.h"
#include "Render/RenderManager.h"

namespace DAVA
{
	

void Collisions::ProjectPolygon(const Vector2 & axis, Polygon2 & poly, float32 & min, float32 & max)
{
	Vector2 * points = poly.GetPoints();
	float32 proj = DotProduct(points[0], axis);
	min = proj;
	max = proj;
	for (int32 pi = 1; pi < poly.GetPointCount(); ++pi)
	{
		proj = DotProduct(points[pi], axis);
		if (proj < min)
			min = proj;
		if (proj > max)
			max = proj;
	}
}


float32 Collisions::IntervalDistance(float32 minA, float32 maxA, float32 minB, float32 maxB) 
{
    if (minA < minB) {
        return minB - maxA;
    } else {
        return minA - maxB;
    }
}
	
void Collisions::AddSeparationAxis(const Vector2 & axis)
{
	size_t size = separationAxes.size();
	for (size_t index = 0; index < 	size; ++index)
	{
		float32 cross = Abs(CrossProduct(axis, separationAxes[index]));
		if (cross < EPSILON)
			return;
	}
	// Logger::Debug("new axis: %f %f", axis.x, axis.y);
	separationAxes.push_back(axis);
}
	
/*
 
 */	
class Transform2
{
	Vector2 translate;
	Matrix2 rotate;
	Vector2 scale;
};
	

bool Collisions::IsPolygonIntersectsPolygon(Polygon2 & poly1, Polygon2 & poly2)
{
//#define DEBUG_DRAW_INTERSECTIONS

	Vector2 * points1 = poly1.GetPoints();
	Vector2 * points2 = poly2.GetPoints();

	separationAxes.clear();
	
	for (int32 index1 = 0; index1 < poly1.pointCount; ++index1)
	{
		int32 index2 = (index1 + 1 != poly1.pointCount) ? (index1 + 1) : (0);
		
		Vector2 line = points1[index2] - points1[index1];
		Vector2 normal = Vector2(line.y, -line.x);
		normal.Normalize();

		AddSeparationAxis(normal);

#if defined(DEBUG_DRAW_INTERSECTIONS)
		RenderManager::Instance()->SetColor(0.0f, 0.0f, 1.0f, 1.0f);
		RenderHelper::DrawLine(points1[index1] + (line / 2), points1[index1] + (line / 2) + normal * 10);
#endif 
	}		
	
	for (int32 index1 = 0; index1 < poly2.pointCount; ++index1)
	{
		int32 index2 = (index1 + 1 != poly2.pointCount) ? (index1 + 1) : (0);
		
		Vector2 line = points2[index2] - points2[index1];
		Vector2 normal = Vector2(line.y, -line.x);
		normal.Normalize();
		AddSeparationAxis(normal);
		
#if defined(DEBUG_DRAW_INTERSECTIONS)
		RenderManager::Instance()->SetColor(0.0f, 1.0f, 0.0f, 1.0f);
		RenderHelper::DrawLine(points2[index1] + (line / 3), points2[index1] + (line / 3) + normal * 10);
#endif 
	}
	
	size_t size = separationAxes.size();
#if defined(DEBUG_DRAW_INTERSECTIONS)
	for (size_t index = 0; index < size; ++index)
	{
		Vector2 axis = separationAxes[index];
		RenderManager::Instance()->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
		RenderHelper::DrawLine(Vector2(50.0f, 50.0f), Vector2(50.0f, 50.0f) + axis * 1000);
	}
#endif 

	for (size_t index = 0; index < size; ++index)
	{
		Vector2 axis = separationAxes[index];
		
		
		float32 p1Min, p1Max;
		ProjectPolygon(axis, poly1, p1Min, p1Max);
		
		float32 p2Min, p2Max;
		ProjectPolygon(axis, poly2, p2Min, p2Max);

#if defined(DEBUG_DRAW_INTERSECTIONS)
		RenderManager::Instance()->SetColor(0.0f, 1.0f, 1.0f, 1.0f);
		Vector2 norm = Vector2(axis.y, -axis.x);
		RenderHelper::DrawLine(Vector2(50.0f, 50.0f) + axis * p1Min + norm * 2.0f, Vector2(50.0f, 50.0f) + axis * p1Max + norm * 2.0f);

		RenderManager::Instance()->SetColor(1.0f, 1.0f, 0.0f, 1.0f);
		RenderHelper::DrawLine(Vector2(50.0f, 50.0f) + axis * p2Min - norm * 2.0f, Vector2(50.0f, 50.0f) + axis * p2Max - norm * 2.0f);
#endif
		
		
		if (IntervalDistance(p1Min, p1Max, p2Min, p2Max) > 0)
			return false;
	}


	return true;
}
	
	
void Collisions::FindIntersectionPolygonToPolygon(Polygon2 & poly1,	
									  Polygon2 & poly2, 
									  ContactManifold2 & manifold)
{
	/*
		Line equation
		Line(x, y) = N * P (x, y) + D; 
	 */
	
	manifold.count = 0;
	for (int32 p1Index1 = 0; p1Index1 < poly1.pointCount; ++p1Index1)
	{
		int32 p1Index2 = (p1Index1 + 1 != poly1.pointCount) ? (p1Index1 + 1) : (0);
		
		for (int32 p2Index1 = 0; p2Index1 < poly2.pointCount; ++p2Index1)
		{
			int32 p2Index2 = (p2Index1 + 1 != poly2.pointCount) ? (p2Index1 + 1) : (0);

			Vector2 intersection;
			
			if (Collisions::Instance()->IsSegmentIntersectsSegment(poly1.points[p1Index1], poly1.points[p1Index2], 
										   poly2.points[p2Index1], poly2.points[p2Index2],
										   intersection))
			{
				manifold.contactPoints[manifold.count] = intersection;
				manifold.count++;
				if (manifold.count == MAX_CONTACT_POINTS)return;
			}
		}
	}
}
	
void Collisions::FindIntersectionPolygonToCircle(const Polygon2 & poly1,const Circle & circle, ContactManifold2 & manifold)
{
	manifold.count = 0;
	for (int32 p1Index1 = 0; p1Index1 < poly1.pointCount; ++p1Index1)
	{
		int32 p1Index2 = (p1Index1 + 1 != poly1.pointCount) ? (p1Index1 + 1) : (0);
		
		ContactManifold2 tempManifold;
		Collisions::Instance()->FindIntersectionLineToCircle(poly1.points[p1Index1], poly1.points[p1Index2], 
																 circle,
															 tempManifold);
		if (tempManifold.count != 0)
		{
			manifold.MergeWithManifold(tempManifold);
		}
	}
}

	
	
};