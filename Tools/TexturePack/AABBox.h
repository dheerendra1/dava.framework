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
#ifndef __LOGENGINE_AABBOX_H__
#define __LOGENGINE_AABBOX_H__

#include "BaseTypes.h"
#include "Vector.h"

namespace Log
{
namespace Math
{

//! \brief class to work with Axial-Aligned Bouding Boxes
//! This class can be used for clipping together with Frustum 
class AABBox
{
	const float32 INFINITY;
public:
	union
	{
		struct
		{
			Vector3 min, max;
		};
		struct
		{
			float32 dmin[3], dmax[3];
		};
	};
	
	//! \brief construct empty bounding box
	inline AABBox();

	//! \brief construct bounding box from another bounding box
	//! \param _box another bouding box
	inline AABBox(const AABBox & _box);
	

	//! \brief construct bounding box from 2 points
	//! \param _min min point of bounding box
	//! \param _max max point of bounding box
	inline AABBox(const Vector3 & _min, const Vector3 & _max);


	//! \brief add point to bounding box
	//! if point inside bounding box bounding box not changed
	//! in another case bounding box become larger
	//! \param pt point to add
	inline void AddPoint(Vector3 pt);
	
	//! \brief check if bounding box intersect other bounding box
	//! \param box another bounding box
	//! \return true if intersect, false otherwise
	inline bool IsIntersect(const AABBox & box);

	//! \brief make bounding box empty
	inline void Empty();

	//! \brief check if bounding box intersect line
	inline bool IsIntersectLine(const Vector3 & l1, const Vector3 &l2);

	//! \brief check if point inside bbox
	inline bool IsInside(const Vector3 & pt);


	//! \brief copy operator of bounding box class
	inline AABBox & operator =(const AABBox & _bbox);

};

//! \brief construct empty bounding box
inline AABBox::AABBox()
: INFINITY(1000000.0f)
{
	min = Vector3(INFINITY, INFINITY, INFINITY);
	max = Vector3(-INFINITY, -INFINITY, -INFINITY);
};

//! \brief construct bounding box from another bounding box
//! \param _box another bouding box
inline AABBox::AABBox(const AABBox & _box)
: INFINITY(1000000.0f)
{
	min = _box.min;
	max = _box.max;
}

//! \brief construct bounding box from 2 points
//! \param _min min point of bounding box
//! \param _max max point of bounding box
inline AABBox::AABBox(const Vector3 & _min, const Vector3 & _max)
: INFINITY(1000000.0f)
{
	min = _min;
	max = _max;
}

//! \brief add point to bounding box
//! if point inside bounding box bounding box not changed
//! in another case bounding box become larger
//! \param pt point to add
inline void AABBox::AddPoint(Vector3 pt)
{
	if (pt.x < min.x)
		min.x = pt.x;
	if (pt.y < min.y)
		min.y = pt.y;
	if (pt.z < min.z)
		min.z = pt.z;

	if (pt.x > max.x)
		max.x = pt.x;
	if (pt.y > max.y)
		max.y = pt.y;
	if (pt.z > max.z)
		max.z = pt.z;
}

//! \brief check if bounding box intersect other bounding box
//! \param box another bounding box
//! \return true if intersect, false otherwise
inline bool AABBox::IsIntersect(const AABBox & box)
{
	// TODO: implement this function
	return false;
};	

//! \brief make bounding box empty
inline void AABBox::Empty()
{
	min = Vector3(INFINITY, INFINITY, INFINITY);
	max = Vector3(-INFINITY, -INFINITY, -INFINITY);
}


//! \brief check if bounding box intersect line
inline bool IsIntersectLine(const Vector3 & l1, const Vector3 &l2)
{
	//float32 tmin[3];
	//float32 tmax[3];
	//
	//Vector3 center = (min + max) / 2.0f;
	//Vector3 p = center  - l1;
	//
	//for (int i = 0; i < 3; ++i)
	//{
	//	float32 e = 
	//	float32 d = 
	//}
	return false;
}

//! \brief check if point inside bbox
inline bool AABBox::IsInside(const Vector3 & pt)
{
	if (
		(min.x <= pt.x)
		&&(min.y <= pt.y)
		&&(min.z <= pt.z)
		&&(pt.x <= max.x)
		&&(pt.y <= max.y)
		&&(pt.z <= max.z))return true;

	return false;
}

//! \brief copy operator of bounding box class
inline AABBox & AABBox::operator =(const AABBox & _bbox)
{
	min = _bbox.min;
	max = _bbox.max;
	return *this;
}




};
};

#endif // __LOGENGINE_AABBOX_H__

