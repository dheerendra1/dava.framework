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
#ifndef __LOGENGINE_POLYGON_H__
#define __LOGENGINE_POLYGON_H__

#include "BaseTypes.h"
#include "Vector.h"
#include "Plane.h"


namespace Log
{
namespace Math
{

//! Class convex polygon in 3D-space (lay in one plane)
class Polygon
{
	const float32 POLYGON_EPSILON;
public:
	int32			pointCount;
	Vector<Vector3> points;
	Plane			plane;
	
	//! Construct base polygon with 0 points
	inline Polygon();

	//! Add point to polygon
	inline void AddPoint(const Vector3 & pt);
	
	//! Clip poly by plane
	inline void ClipByPlane(const Plane & plane);

	//! Check if polygon contains point
	inline bool Contains(const Math::Vector3 & point);
};


// Implementation


//! Construct base polygon with 0 points
inline Polygon::Polygon()
: POLYGON_EPSILON(0.000001f),
pointCount(0)
{
}

//! Add point to polygon
inline void Polygon::AddPoint(const Vector3 & pt)
{
	points.push_back(pt);
	++pointCount;
}

//! Clip poly by plane
inline void Polygon::ClipByPlane(const Plane & plane)
{
	// TODO: test / check performance / optimize
	Vector<Vector3> resultVector;
	int32 i2;
	int32 count;
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
	for (i = 0; i < count; ++i)
		points.push_back(resultVector[i]);
}

//! \todo Add this function
//! Check if polygon contains point
inline bool Contains(const Math::Vector3 & point)
{
	return false; 
}


};
};


#endif // __LOGENGINE_POLYGON_H__

