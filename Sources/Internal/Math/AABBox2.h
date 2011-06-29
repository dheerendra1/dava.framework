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
#ifndef __DAVAENGINE_AABBOX2_H__
#define __DAVAENGINE_AABBOX2_H__

#include "Base/BaseTypes.h"
#include "Base/BaseMath.h"
#include "Math/Vector.h"
#include "Math/Ray.h"

namespace DAVA
{

#define AABBOX_INFINITY 1000000.0f	

/**
	\ingroup math
	\brief Class to work with axial-aligned bounding boxes in 2D space
 */
class AABBox2
{
public:
	Vector2 min, max;
	
	//! \brief construct empty bounding box
	inline AABBox2();

	//! \brief construct bounding box from another bounding box
	//! \param _box another bouding box
	inline AABBox2(const AABBox2 & _box);
	

	//! \brief construct bounding box from 2 points
	//! \param _min min point of bounding box
	//! \param _max max point of bounding box
	inline AABBox2(const Vector2 & _min, const Vector2 & _max);

	
	//! \brief construct bounding box from center & box size
	//! \param center center point of bounding box
	//! \param size size of bounding box
	inline AABBox2(const Vector2 & center, float32 size);

	//! \brief add point to bounding box
	//! if point inside bounding box bounding box not changed
	//! in another case bounding box become larger
	//! \param pt point to add
	inline void AddPoint(const Vector2 & pt);
	inline void AddAABBox(AABBox2 & bbox);
	
	//! \brief make bounding box empty
	inline void Empty();

	//! \brief check if bounding box intersect line
	inline bool IsIntersectLine(const Vector2 & l1, const Vector2 &l2);
	
	//! \brief check if bounding box intersect ray
	bool IsIntersectsWithRay(Ray2 & r, float32 & tmin, float32 & tmax, float32 t0 = 0.0f, float32 t1 = 1.0f);

	//! \brief check if bounding box intersect other bounding box. SAT.
	//! \param box another bounding box
	//! \return true if intersect, false otherwise
	bool IsIntersectsWithBox(const AABBox2 & box);
	
	//! \brief check if point inside bbox
	inline bool IsInside(const Vector2 & pt);
	
	//! \brief get center
	inline Vector2 GetCenter();


	//! \brief copy operator of bounding box class
	inline AABBox2 & operator =(const AABBox2 & _bbox);

        
        
        inline bool IntersectsSegment(const Vector2 & l1, const Vector2 &l2);
};

//! \brief construct empty bounding box
inline AABBox2::AABBox2()
{
	min = Vector2(AABBOX_INFINITY, AABBOX_INFINITY);
	max = Vector2(-AABBOX_INFINITY, -AABBOX_INFINITY);
};

//! \brief construct bounding box from another bounding box
//! \param _box another bouding box
inline AABBox2::AABBox2(const AABBox2 & _box)
{
	min = _box.min;
	max = _box.max;
}

inline AABBox2::AABBox2(const Vector2 & _min, const Vector2 & _max)
{
	min = _min;
	max = _max;
}

inline AABBox2::AABBox2(const Vector2 & center, float32 size)
{
	min = center - Vector2(size / 2.0f, size / 2.0f);
	max = center + Vector2(size / 2.0f, size / 2.0f);
}
	
inline void AABBox2::AddPoint(const Vector2 & pt)
{
	if (pt.x < min.x)
		min.x = pt.x;
	if (pt.y < min.y)
		min.y = pt.y;

	if (pt.x > max.x)
		max.x = pt.x;
	if (pt.y > max.y)
		max.y = pt.y;
}
	
inline void AABBox2::AddAABBox(AABBox2 & bbox)
{
	AddPoint(bbox.min);
	AddPoint(bbox.max);
}

//! \brief make bounding box empty
inline void AABBox2::Empty()
{
	min = Vector2(AABBOX_INFINITY, AABBOX_INFINITY);
	max = Vector2(-AABBOX_INFINITY, -AABBOX_INFINITY);
}


//! \brief check if bounding box intersect line
inline bool IsIntersectLine(const Vector2 & l1, const Vector2 &l2)
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
inline bool AABBox2::IsInside(const Vector2 & pt)
{
	if (
		(min.x <= pt.x)
		&&(min.y <= pt.y)
		&&(pt.x <= max.x)
		&&(pt.y <= max.y))return true;

	return false;
}

//! \brief copy operator of bounding box class
inline AABBox2 & AABBox2::operator =(const AABBox2 & _bbox)
{
	min = _bbox.min;
	max = _bbox.max;
	return *this;
}

//! \brief get center
inline Vector2 AABBox2::GetCenter()
{
	return (min + max) / 2.0f;
}


    inline bool AABBox2::IntersectsSegment(const Vector2 & l1, const Vector2 &l2)
    {
        float xmin, xmax, ymin, ymax;
        
        xmin = std::min(l1.x, l2.x);
        xmax = std::max(l1.x, l2.x);
        
        ymin = std::min(l1.y, l2.y);
        ymax = std::max(l1.y, l2.y);
        
        if ((xmin <= max.x) && (xmax >= min.x))
            if ((ymin <= max.y) && (ymax >= min.y))
                return true;
      
        return false;
    }



};


#endif // __DAVAENGINE_AABBOX_H__

