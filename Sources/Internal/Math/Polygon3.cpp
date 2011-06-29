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



namespace DAVA 
{
	
//! Clip poly by plane
void Polygon3::ClipByPlane(const Plane & plane)
{
	// TODO: test / check performance / optimize / эта пиздец! count чему в начале равен?
	Vector<Vector3> resultVector;
	int32 i2;
	int32 count = 0;
	for(int32 i = 0; i < pointCount;++i)
	{
		i2 = i + 1;
		if (i2 >= pointCount) i2 = 0;
		
		float32 d1 = plane.DistanceToPoint(points[i]);
		float32 d2 = plane.DistanceToPoint(points[i2]);
		if ((d1 >= POLYGON_EPSILON) && (d2 >= POLYGON_EPSILON))
		{
			resultVector.push_back(points[i]);
			resultVector.push_back(points[i2]);
			count += 2;
		}else if ((d1 >= POLYGON_EPSILON) && (d2 <= -POLYGON_EPSILON))
		{	
			Vector3 res;
			res.Lerp(points[i], points[i2], d1 / (d1 - d2));
			
			resultVector.push_back(points[i]);
			resultVector.push_back(res);
			count += 2;
		}else if ((d1 <= -POLYGON_EPSILON) && (d2 >= POLYGON_EPSILON))
		{
			Vector3 res;
			res.Lerp(points[i], points[i2], -d1 / (-d1 + d2));
			resultVector.push_back(res);
			//resultVector.push_back(points[i]);
			count += 1;
		}
	}
	points.clear();
	for (int32 i = 0; i < count; ++i)
		points.push_back(resultVector[i]);
}


//! Check if point lays inside polygon (polygon must be in one plane)
bool Polygon3::IsPointInside(const Vector3 & pt)
{
	// Do not check if this is not a polygon
	if (pointCount <= 2)return false;
	
	UpdatePlaneFromPoints();
	
	float planeDist = plane.DistanceToPoint(pt);
	// if not on plane exit
	if (!FLOAT_EQUAL(planeDist, 0.0f))return false;
	
	int intersectionsCount = 0;
	Vector2 ray0(pt.x, pt.y);
	Vector2 ray1(((points[0].x + points[1].x) / 2.0f) + 1000000.0f, ((points[0].y + points[1].y) / 2.0f) + 100000.0f);
	
	/*
	 If you enabling debug intersections do not forget to call this function inside Draw of your application.
	 If you'll call it inside update or mouse or keyboard input functions you can not get any output.
	 */	
//#define DEBUG_DRAW_INTERSECTIONS
#if defined(DEBUG_DRAW_INTERSECTIONS)
	RenderManager::Instance()->SetColor(0.0f, 1.0f, 0.0f, 1.0f);
	RenderHelper::DrawLine(ray0, ray1);
#endif 
	
	for (int i = 0; i < pointCount; ++i)
	{
		int32 i2 = (i == pointCount - 1) ? (0) : (i + 1);
		Vector2 pt1(points[i].x, points[i].y);
		Vector2 pt2(points[i2].x, points[i2].y);
		
		Vector2 result;
		if (Collisions::Instance()->IsSegmentIntersectsSegment(pt1, pt2, ray0, ray1, result))
		{
			
#if defined(DEBUG_DRAW_INTERSECTIONS)
			RenderManager::Instance()->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
			RenderHelper::DrawPoint(result, 5.0f);
#endif 
			
			intersectionsCount++;
		}
	}
	return intersectionsCount & 1;
}

//! Remove segment with index
void Polygon3::RemoveSegmentAtIndex(int32 index)
{
	for (int k = index + 1; k < pointCount; ++k)
		points[k - 1] = points[k];
	points.pop_back();
	
	pointCount--;
}

//! Remove small segments function remove all segments size of which smaller or equal than removalSize 
int32 Polygon3::RemoveSmallSegments(float32 removalSize)
{
	float32 removalSquareSize = removalSize * removalSize;
	int removedCount = 0;
	for (int segment = 0; segment < pointCount - 1; ++segment)
	{
		Vector3 v = points[segment] - points[segment + 1];
		if (v.SquareLength() <= removalSquareSize)
		{
			RemoveSegmentAtIndex(segment);
			segment--;
			removedCount++;
		}
	}
	return removedCount;
}

//! Move by polyline and get position on polyline based on distance
void Polygon3::InterpolatePositionFromDistance(float32 distance, int startSegment, Vector3 & resultPosition, int & resultSegmentIndex)
{
	float32 currentDistance = distance;
	
	int currentSegment = 0;
	float currentSegmentLenght = 0.0f;
	for (currentSegment = startSegment; currentSegment < pointCount - 1; ++currentSegment)
	{
		Vector3 v = points[currentSegment] - points[currentSegment + 1];
		currentSegmentLenght = v.Length();
		if (currentDistance - currentSegmentLenght < 0.0f)
			break; // we've found right segment
		currentDistance -= currentSegmentLenght;
	}
	
	resultSegmentIndex = -1;
	if (currentSegment == pointCount - 1)
	{
		return;
	}
	float t = currentDistance / currentSegmentLenght;
	if ((t >= 0.0f) && (t <= 1.0f))
	{
		resultPosition.Lerp(points[currentSegment], points[currentSegment + 1], t);
		resultSegmentIndex = currentSegment;
	}
}
	
//! Move by polyline and get position on polyline based on distance
void Polygon3::InterpolatePositionFromDistanceReverse(float32 distance, int startSegment, Vector3 & resultPosition, int & resultSegmentIndex)
{
	float32 currentDistance = distance;
	
	int currentSegment = 0;
	float currentSegmentLenght = 0.0f;
	for (currentSegment = startSegment; currentSegment >= 1; --currentSegment)
	{
		Vector3 v = points[currentSegment] - points[currentSegment - 1];
		currentSegmentLenght = v.Length();
		if (currentDistance - currentSegmentLenght < 0.0f)
			break; // we've found right segment
		currentDistance -= currentSegmentLenght;
	}
	
	resultSegmentIndex = -1;
	if (currentSegment == 0)
	{
		return;
	}
	float t = currentDistance / currentSegmentLenght;
	if ((t >= 0.0f) && (t <= 1.0f))
	{
		resultPosition.Lerp(points[currentSegment], points[currentSegment - 1], t);
		resultSegmentIndex = currentSegment;
	}
}
	
//! Remove points in given radius
void Polygon3::RemovePointsInSphere(const Vector3 & center, float32 radius)
{
	float32 rad2 = radius * radius;
	for (int p = 0; p < pointCount; ++p)
	{
		Vector3 dist = points[p] - center;
		if (dist.SquareLength() <= rad2)
		{
			RemoveSegmentAtIndex(p);
			p--;
		}
	}
}
	
//! Square length of poly segments
float32 Polygon3::SquareLength()
{
	float32 squareLength = 0.0f;
	for(int32 i = 0; i < pointCount - 1;++i)
	{
		Vector3 segment = points[i] - points[i + 1];
		squareLength += segment.SquareLength();
	}
	return squareLength;
}

//! Length
float32 Polygon3::Length()
{
	float32 length = 0.0f;
	for(int32 i = 0; i < pointCount - 1;++i)
	{
		Vector3 segment = points[i] - points[i + 1];
		length += segment.Length();
	}
	return length;
}
	
//! Scale whole polygon around given center point 
void Polygon3::Scale(const Vector3 & centerPoint, float32 scale)
{
	for (int p = 0; p < pointCount; ++p)
	{
		points[p] -= centerPoint;
		points[p] *= scale;
		points[p] += centerPoint;
	}	
}
	
void Polygon3::Translate(const Vector3 & translatePos)
{
	for (int p = 0; p < pointCount; ++p)
	{
		points[p] += translatePos;
	}
}
	
//! Transform polygon using matrix
void Polygon3::Transform(const Matrix4 & matrix)
{
	for (int p = 0; p < pointCount; ++p)
	{
		points[p] = points[p] * matrix;
	}
	
}

void Polygon3::CalculateCenterPoint(Vector3 & center)
{
	center.Set(0.0f, 0.0f, 0.0f);
	for (int p = 0; p < pointCount; ++p)
	{
		center += points[p];
	}
	center /= (float32)pointCount;
}
	
float32 Polygon3::CalculateSquareRadius(const Vector3 & center)
{
	float32 radius = 0.0f;
	for (int p = 0; p < pointCount; ++p)
	{
		float32 dist = (points[p] - center).SquareLength();
		if (dist > radius)
			radius = dist;
	}
	return radius;
}
	
//! Merge all segments triangle's height of which bigger or equal than minTriangleHeight 
// http://www.math.ru/dic/276
void Polygon3::MergeFlatPolygonSegments(Polygon3 &srcPolygon, Polygon3 &destPolygon, float32 minTriangleHeight)
{
	float mh2	= minTriangleHeight * minTriangleHeight;
	
	destPolygon.Clear();
	
	for (int i0 = 0; i0 < srcPolygon.pointCount - 2; ++i0)
	{
		destPolygon.AddPoint(srcPolygon.points[i0]);
	
		Vector3 v0 = srcPolygon.points[i0];
		
		for (int i2 = i0 + 2; i2 < srcPolygon.pointCount; ++i2)
		{
			Vector3	v2	= srcPolygon.points[i2];
			float	c	= (v2 - v0).Length();
			float	hc2	= 0.0f;
			
			for (int i1 = i0 + 1; i1 < i2; ++i1)
			{
				Vector3	v1 = srcPolygon.points[i1];
				float	a	= (v1 - v0).Length();
				float	b	= (v2 - v1).Length();
				float	p	= (a + b + c) / 2.0f;
				hc2			= 4.0f * p * (p - a) * (p - b) * (p - c) / (c * c);
				
				if (hc2 >= mh2)
				{
					break;
				}
			}
			
			if (hc2 >= mh2)
			{
				i0 = i2 - 1 - 1;
				break;
			}
		}
	}

	destPolygon.AddPoint(srcPolygon.points.back());
}

};



