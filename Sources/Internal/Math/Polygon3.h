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
#ifndef __DAVAENGINE_POLYGON3_H__
#define __DAVAENGINE_POLYGON3_H__

#include "Base/BaseTypes.h"
#include "Math/Vector.h"
#include "Math/Plane.h"

namespace DAVA
{


/**	
	\ingroup math
	\brief Class that represents Polygon in 3D space. 
 */		
class Polygon3
{
	const float32 POLYGON_EPSILON;
public:
	
	int32			pointCount;
	Vector<Vector3> points;
	Plane			plane;
	
	//! Construct base polygon with 0 points
	inline Polygon3();

	//! Copy constructor 
	inline Polygon3(const Polygon3 & p);
	
	//! Copy operator
	inline Polygon3 & operator = (const Polygon3 & p);

	//! Function to copy source polygon to destination polygon)
	static inline void Copy(const Polygon3 & sourcePolygon3, Polygon3 & destPolygon3);
	
	//! Clear polygon point list
	inline void Clear();

	//! Add point to polygon
	inline void AddPoint(const Vector3 & pt);
	
	//! Update polygon plane from polygon points
	inline void UpdatePlaneFromPoints();
	
	//! Get point array
	inline Vector3 * GetPoints();
	
	//! Remove segment with index
	void RemoveSegmentAtIndex(int32 index);
	
	//! Remove small segments function remove all segments size of which smaller or equal than removalSize 
	int32 RemoveSmallSegments(float32 removalSize);

	//! Remove points in given sphere
	void RemovePointsInSphere(const Vector3 & center, float32 radius);
	
	//! Move by polyline and get position on polyline based on distance
	void InterpolatePositionFromDistance(float32 distance, int startSegment, Vector3 & resultPosition, int & resultSegmentIndex);

	//! Move by polyline and get position on polyline based on distance
	void InterpolatePositionFromDistanceReverse(float32 distance, int startSegment, Vector3 & resultPosition, int & resultSegmentIndex);

	//! Clip poly by plane (TODO: not tested)
	void ClipByPlane(const Plane & plane);
	
	//! Check if point lays inside polygon (polygon must be in one plane)
	bool IsPointInside(const Vector3 & pt);
	
	//! Square length of poly segments
	float32 SquareLength();

	//! Length of poly segments
	float32 Length();

	//! Translate whole polygon
	void	Translate(const Vector3 & translatePos);

	//! Scale whole polygon around given center point 
	void	Scale(const Vector3 & centerPoint, float32 scale);
	
	//! Transform polygon using matrix
	void	Transform(const Matrix4 & matrix);
	

	//! Calculate center point for the polygon
	void CalculateCenterPoint(Vector3 & center); 
	
	//! Calculate center point and radius for polygon
	float32 CalculateSquareRadius(const Vector3 & center);
	
	//! Merge all segments triangle's height of which bigger or equal than minTriangleHeight 
	static void MergeFlatPolygonSegments(Polygon3 &srcPolygon3, Polygon3 &destPolygon3, float32 minTriangleHeight);
};


// Implementation


//! Construct base polygon with 0 points
inline Polygon3::Polygon3()
: POLYGON_EPSILON(0.000001f),
pointCount(0)
{
}
	
//! Copy constructor 
inline Polygon3::Polygon3(const Polygon3 & p)
	: POLYGON_EPSILON(0.000001f)
{
	Copy(p, *this);
}
	
//! Copy operator
inline Polygon3 & Polygon3::operator = (const Polygon3 & p)
{
	Copy(p, *this);
	return *this;
}
	
//! Function to copy source polygon to destination polygon)
inline void Polygon3::Copy(const Polygon3 & sourcePolygon3, Polygon3 & destPolygon3)
{
	destPolygon3.pointCount = sourcePolygon3.pointCount;
	destPolygon3.points = sourcePolygon3.points;
}	
	
inline void Polygon3::Clear()
{
	pointCount = 0;
	points.clear();
}		

//! Add point to polygon
inline void Polygon3::AddPoint(const Vector3 & pt)
{
	points.push_back(pt);
	++pointCount;
}

	
//! Update polygon plane from polygon points
inline void Polygon3::UpdatePlaneFromPoints()
{
	if (pointCount >= 3)
		plane = Plane(points[0], points[1], points[2]);
}
	
	
//! Get point array
inline Vector3 * Polygon3::GetPoints()
{
	return &points.front();
}

};

#endif // __DAVAENGINE_POLYGON3_H__