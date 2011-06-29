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
#include "Base/BaseMath.h"
#include "Collision/Collisions.h"

namespace DAVA 
{
Collisions::Collisions()
{
	separationAxes.reserve(8);
}
Collisions::~Collisions()
{
	
}
	
	
	
bool Collisions::Is2DLineIntersects2DLine
								(const Vector2 & p1, const Vector2 & p2, 
								 const Vector2 & p3, const Vector2 & p4, 
								 Vector2 & inters)
{
	float a1 = p2.y - p1.y;
	float b1 = p1.x - p2.x;
	float c1 = -a1 * p1.x - b1 * p1.y; 
	
	float a2 = p4.y - p3.y;
	float b2 = p3.x - p4.x;
	float c2 = -a2 * p3.x - b2 * p3.y; 
	
	float determ = (a1 * b2 - a2 * b1);
	if (FLOAT_EQUAL(determ, 0.0f))
	{
		return false;
	}
	
	inters.x = (b1 * c2 - b2 * c1) / determ;
	inters.y = (a2 * c1 - a1 * c2) / determ;
	return true;
}


bool Collisions::IsSegmentIntersectsSegment(	
									   const Vector2 & p1, const Vector2 & p2, 
									   const Vector2 & p3, const Vector2 & p4, 
										  Vector2 & inters)
{
	float x1 = p1.x;
	float y1 = p1.y;
	
	float x2 = p2.x;
	float y2 = p2.y;
	
	float x3 = p3.x;
	float y3 = p3.y;
	
	float x4 = p4.x;
	float y4 = p4.y;
	
	float determ = (y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1);
	if (FLOAT_EQUAL(determ, 0.0f))
	{
		return false;
	}
	
	float t1 = ( (x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3) ) / determ;
	float t2 = ( (x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3) ) / determ;
	
	if ((t1 >= 0.0f) && (t1 <= 1.0f))
		if ((t2 >= 0.0f) && (t2 <= 1.0f))
		{
			inters.x = x1 + (x2 - x1) * t1;
			inters.y = y1 + (y2 - y1) * t1;
			return true;
		}
	return false;
}
	
	
bool Collisions::Is2DPolygonIntersects2DPolygon(const Polygon2 & poly1,	
												const Polygon2 & poly2,
												Vector2 & inters)
{
	for(int pnt = 0; pnt < poly1.pointCount; pnt ++)
	{
		int count = 0;
		float pntY = poly1.points[pnt].y;
		for(int i = 0; i < poly2.pointCount; i ++)
		{
			int j = i + 1;
			float iY = poly2.points[i].y;
			float jY = poly2.points[j].y;
			
			if( iY == jY)
			{
				continue;
			}
			if(iY > pntY && jY > pntY)
			{
				continue;
			}
			if(iY < pntY && jY < pntY)
			{
				continue;
			}
			if(Max(iY, jY) == pntY)
			{
				count++;
			}
			else if(Min(iY, jY) == pntY)
			{
				continue;
			}
			else
			{
				float fpntY = poly1.points[pnt].y;
				float fiY = poly2.points[i].y;
				float fjY = poly2.points[j].y;
				float t = (float)(fpntY - fiY) / (float)(fjY - fiY);
				if(poly2.points[i].x + t * (poly2.points[j].x - poly2.points[i].x) >= poly1.points[pnt].x)
				{
					count++;
				}
			}
		}
		if(count & 1)
		{
			inters.x = poly1.points[pnt].x;
			inters.y = pntY;
			return true;
		}
	}
	
	return false;
}

	
void Collisions::FindIntersectionLineToCircle(const Vector2 & p1, const Vector2 & p2, const Circle & circle, ContactManifold2 & manifold)
{
	float32 a = (p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y);
	float32 b = 2 * ( (p2.x - p1.x) * (p1.x - circle.center.x) + (p2.y - p1.y) * (p1.y - circle.center.y) );
	float32 c = circle.center.x * circle.center.x + circle.center.y * circle.center.y 
	+ p1.x * p1.x + p1.y * p1.y - 2 * (circle.center.x * p1.x + circle.center.y * p1.y) 
	- circle.radius * circle.radius;
		// -- -- -- -- //
	float32 d = (b * b - 4 * a * c);
	
	manifold.count = 0;
	
	if (d < 0.0f)
	{
		// no solutions
		// we do not change anything because we assume that manifold already initialized
		return;
	}
	
	d = sqrtf(d);
	
	if ((d >= 0.0f) && (d <= EPSILON))
	{
		// one solution
		
		float32 u = -b / (2 * a);
		manifold.count = 0;
		if (u >= 0.0f && u <= 1.0f)
		{
			manifold.AddContactPoint(Vector2(p1.x + (p2.x - p1.x) * u, p1.y + (p2.y - p1.y) * u));
		}
	}else if (d > EPSILON)
	{
		// two solutions
		float32 inv2A = 1.0f / (2.0f * a);
		float32 u1 = (-b - d) * inv2A;
		//		if (u1 >= 0.0f - EPSILON && u1 <= 1.0f + EPSILON)
		if (u1 >= 0.0f && u1 <= 1.0f)
		{
			manifold.AddContactPoint(Vector2(p1.x + (p2.x - p1.x) * u1, p1.y + (p2.y - p1.y) * u1));
		}
		
		float32 u2 = (-b + d) * inv2A;
		//		if (u2 >= 0.0f - EPSILON && u2 <= 1.0f + EPSILON)
		if (u2 >= 0.0f && u2 <= 1.0f)
		{
			manifold.AddContactPoint(Vector2(p1.x + (p2.x - p1.x) * u2, p1.y + (p2.y - p1.y) * u2));
		}
	}
}

void Collisions::FindIntersectionCircleToCircle(const Circle & c1, const Circle & c2, ContactManifold2 & manifold)
{
	Vector2 cdiff = c1.center - c2.center;
	float32 radii = c1.radius + c2.radius;
	radii *= radii;
	float32 squareDistance = cdiff.SquareLength();
	if (squareDistance > radii)
	{
		manifold.count = 0;
		return;
	}
	
	cdiff /= 2;
	manifold.count = 1;
	manifold.contactPoints[0] = c2.center + cdiff;
	// (x - cx1) ^ 2 + (y - cy1) ^ 2 = r1 ^ 2
	// (x - cx2) ^ 2 + (y - cy2) ^ 2 = r2 ^ 2
	// x = sqrt(r2^2 - (y - cy2) ^ 2) + cx2 
	
	// x^2 - 2*x*cx1 + cx1^2 + y^2 - 2*y*cy1 + cy1^2 = r1^2
	// (r2^2 - (y - cy2) ^ 2 + cx2)^2 + 2 * (sqrt(r2^2 - (y - cy2) ^ 2) + cx2) * cx1 + cx1 ^ 2 + y^2 - 2*y*cy1 + cy1^2 = r1^2
	
	
	
	// 8293349181
	// Понаморева 11, съезд направо к рынку, поворот на лево, проезжаю мимо школы, поворот еще один на лево. 
}

};






