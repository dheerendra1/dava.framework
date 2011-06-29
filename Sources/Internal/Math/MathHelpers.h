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
#ifndef __DAVAENGINE_MATHHELPERS_H__
#define __DAVAENGINE_MATHHELPERS_H__

#include "Math/Math2D.h"
#include "Math/Vector.h"
#include "Math/Matrix4.h"
#include "Math/Quaternion.h"

namespace DAVA
{	
	/*
	 Radians to degrees and back conversion functions and constants
	 */
	
	static const float32 RAD_TO_DEG = 180.0f / PI; 
	static const float32 DEG_TO_RAD = PI / 180.0f;
	
	inline float32 RadToDeg(float32 f) { return f * RAD_TO_DEG; };
	inline float32 DegToRad(float32 f) { return f * DEG_TO_RAD; };
	
	/*
	 Function to conver euler angles to normalized axial vectors
	*/
	void	AnglesToVectors(const Vector3 & _angles, Vector3 & _vx, Vector3 & _vy, Vector3 & _vz);

    inline bool IsPowerOf2(int32 x)
    {
        if (x < 0)
            return false;

        if (x<1) return false;

        return (x&(x-1))==0;
    }

    inline void EnsurePowerOf2(int32 & x)
    {
        if (IsPowerOf2(x))
            return;

        int32 i = 1;
		while(i < x)
			i *= 2;

		x = i;
    }
	
} // end of namespace DAVA

#endif 
