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
#include "Math/AABBox3.h"

namespace DAVA 
{
	
//! \brief check if bounding box intersect ray
bool AABBox3::IsIntersectsWithRay(Ray3 & r, float32 & tmin, float32 & tmax, float32 t0, float32 t1)
{
	float32 tymin, tymax, tzmin, tzmax; 
	
	float32 divx = 1.0f / r.direction.x; 
	if (divx >= 0) 
	{ 
		tmin = (min.x - r.origin.x) * divx; 
		tmax = (max.x - r.origin.x) * divx; 
	} 
	else { 
		tmin = (max.x - r.origin.x) * divx; 
		tmax = (min.x - r.origin.x) * divx; 
	} 
	
	float32 divy = 1.0f / r.direction.y; 
	if (divy >= 0) 
	{ 
		tymin = (min.y - r.origin.y) * divy; 
		tymax = (max.y - r.origin.y) * divy; 
	} 
	else 
	{ 
		tymin = (max.y - r.origin.y) * divy; 
		tymax = (min.y - r.origin.y) * divy; 
	} 
	if ( (tmin > tymax) || (tymin > tmax) ) 
		return false; 
	
	if (tymin > tmin) 
		tmin = tymin; 
	if (tymax < tmax) 
		tmax = tymax; 
	
	float32 divz = 1.0f / r.direction.z; 
	if (divz >= 0) 
	{ 
		tzmin = (min.z - r.origin.z) * divz; 
		tzmax = (max.z - r.origin.z) * divz; 
	} 
	else 
	{ 
		tzmin = (max.z - r.origin.z) * divz; 
		tzmax = (min.z - r.origin.z) * divz; 
	} 
	
	if ( (tmin > tzmax) || (tzmin > tmax) ) 
		return false; 
	if (tzmin > tmin) 
		tmin = tzmin; 
	if (tzmax < tmax) 
		tmax = tzmax; 
	return ( (tmin < t1) && (tmax > t0) ); 
} 
	
void AABBox3::GetTransformedBox(const Matrix4 & transform, AABBox3 & result)
{
    result.min.x = transform.data[12];
    result.min.y = transform.data[13];
    result.min.z = transform.data[14];
    
    result.max.x = transform.data[12];
    result.max.y = transform.data[13];
    result.max.z = transform.data[14];
    
    
    for (int32 i = 0; i < 3; ++i)
    {
        for (int32 j = 0; j < 3; ++j)
        {
            float32 a = transform._data[i][j] * min.data[j];
            float32 b = transform._data[i][j] * max.data[j];
            
            if( a < b ) 
            { 
                result.min.data[i] += a; 
                result.max.data[i] += b;
            }
            else 
            { 
                result.min.data[i] += b; 
                result.max.data[i] += a;
            }
        };
    }
    
}

void AABBox3::GetCorners(Vector3 * cornersArray)
{
    cornersArray[0] = min;
    cornersArray[1] = max;
    cornersArray[2].Set(min.x, min.y, max.z);
    cornersArray[3].Set(min.x, max.y, min.z);
    cornersArray[4].Set(max.x, min.y, min.z);
    cornersArray[5].Set(max.x, max.y, min.z);
    cornersArray[6].Set(max.x, min.y, max.z);
    cornersArray[7].Set(min.x, max.y, max.z);
}

    
    
    
};

