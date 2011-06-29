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
#ifndef __LOGENGINE_MATH2DMATRIX2_H__
#define __LOGENGINE_MATH2DMATRIX2_H__

#include <math.h>
#include "BaseTypes.h"

namespace Log
{
namespace Math
{

//!	Matrix2 
struct Matrix2
{
	union
	{
		float32 data[4];
		struct {
			float32 _00, _01;
            float32 _10, _11;
		};
	};
	
	inline Matrix2() {};
	inline Matrix2(float32 m00, float32 m01, float32 m10, float32 m11);
	inline Matrix2(const Matrix2 & m);

	
	inline float32 Det() const;
	
	// Helpers
	inline void SetIdentity();
	void	BuildRotation(float32 Angle);

	inline Matrix2& operator *= (const Matrix2 & arg);
	inline Matrix2 operator *	(const Matrix2 & arg) const;

	inline Matrix2& operator -= (const Matrix2 & arg);
	inline Matrix2 operator -	(const Matrix2 & arg) const;

	inline Matrix2& operator += (const Matrix2 & arg);
	inline Matrix2 operator +	(const Matrix2 & arg) const;
};





inline Matrix2::Matrix2(float32 m00, float32 m01, float32 m10, float32 m11)
{
	data[0] = m00;
	data[1] = m01;
	data[2] = m10;
	data[3] = m11;
};

inline Matrix2::Matrix2(const Matrix2 & m)
{
	*this = m;
}

inline float32 Matrix2::Det() const
{
	return data[0] * data[3] - data[1] * data[2];
}

inline void Matrix2::SetIdentity()
{
	data[0] = 1; data[1] = 0;
	data[2] = 0; data[3] = 1;
}

inline void Matrix2::BuildRotation(float32 Angle)
{
	float32	CosA = cosf(Angle);
	float32	SinA = sinf(Angle);

	data[0] = CosA; data[1] = SinA;
	data[2] = -SinA; data[3] = CosA;
}

inline Matrix2 Matrix2::operator *(const Matrix2 & m) const
{
	return Matrix2( _00 * m._00 + _01 * m._10, 
					_00 * m._01 + _01 * m._11, 
					
					_10 * m._00 + _11 * m._10, 
					_10 * m._01 + _11 * m._11);
}

inline Matrix2& Matrix2::operator *= (const Matrix2 & m)
{
	return (*this = *this * m);
}

inline Matrix2 Matrix2::operator +(const Matrix2 & m) const
{
	return Matrix2( _00 + m._00, _01 + m._01, 
						  _10 + m._10, _11 + m._11);
}

inline Matrix2& Matrix2::operator += (const Matrix2 & m)
{
	return (*this = *this + m);
}

inline Matrix2 Matrix2::operator -(const Matrix2 & m) const
{
	return Matrix2( _00 - m._00, _01 - m._01, 
						  _10 - m._10, _11 - m._11);
}

inline Matrix2& Matrix2::operator -= (const Matrix2 & m)
{
	return (*this = *this - m);
}




};	// end of namespace	Math
};	// end of namespace Log



#endif // __LOGENGINE_MATH2DMATRIX2_H__

