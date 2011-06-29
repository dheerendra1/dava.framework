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
#ifndef __LOGENGINE_MATRIX_H__
#define __LOGENGINE_MATRIX_H__

namespace Log
{
namespace Math
{


//
//	we use vertors as rows 
//
//	v = [ x y z w ]
//	m = [ _00 _01 _02 _03 ] 
//		[ _10 _11 _12 _13 ]
//		[ _20 _21 _22 _23 ]
//		[ _30 _31 _32 _33 ]
//

struct Matrix4
{
	union
	{
		float32	data[16];
		float32	_data[4][4];
		struct {
			float32 _00, _01, _02, _03;
            float32 _10, _11, _12, _13;
            float32 _20, _21, _22, _23;
			float32 _30, _31, _32, _33;
		};
	};
	inline Matrix4();

	inline Matrix4(	float32 _D00, float32 _D01, float32 _D02, float32 _D03,
					float32 _D10, float32 _D11, float32 _D12, float32 _D13,
					float32 _D20, float32 _D21, float32 _D22, float32 _D23,
					float32 _D30, float32 _D31, float32 _D32, float32 _D33);

	inline Matrix4(const Matrix3 & m);
	inline Matrix4(const Matrix4 & m);
	
	inline Matrix4 & operator = (const Matrix4 & m);

	inline void	Identity();
	inline void	Zero();

	inline void	BuildProjectionFovLH(float32 _fovY, float32 _aspect, float32 _zn, float32 _zf);
	inline void	BuildOrthoLH(float _l, float _r, float32 _t, float32 _b, float32 _zn, float32 _zf);
	
	inline void BuildLookAtMatrixLH(const Math::Vector3 & _position, 
									const Math::Vector3 & _target, 
									const Math::Vector3 & _up);

	inline void Transpose();

	inline bool	Inverse();
	inline bool GetInverse(Matrix4 & out);
	

	//! create translation matrix
	inline void	CreateTranslation(const Vector3 & vector);

	//! create translation matrix
	inline void	CreateRotation(const Vector3 & axis, float32 angle);

	//! create scale matrix
	inline void	CreateScale(const Vector3 & vector);



	//! Simple operator for directly accessing every element of the matrix.
	float32 & operator()(int32 row, int32 col) { return _data[col][row]; }
	//! Simple operator for directly accessing every element of the matrix.
	const float32 & operator()(int32 row, int32 col) const { return _data[col][row]; }

	//! matrix mul vector
	inline Vector3 operator * (const Vector3 & ) const;

	//! matrix multiplication
	inline Matrix4 operator *(const Matrix4 & m) const;
	inline const Matrix4 & operator *=(const Matrix4 & m);

};

inline Vector3 operator * (const Vector3 & _v, const Matrix4 & _m);


// Implementation of matrix4

inline Matrix4::Matrix4()
{
	_00 = 0; _01 = 0; _02 = 0; _03 = 0;
	_10 = 0; _11 = 0; _12 = 0; _13 = 0;
	_20 = 0; _21 = 0; _22 = 0; _23 = 0;
	_30 = 0; _31 = 0; _32 = 0; _33 = 0;
}

inline Matrix4::Matrix4(	float32 _D00, float32 _D01, float32 _D02, float32 _D03,
							float32 _D10, float32 _D11, float32 _D12, float32 _D13,
							float32 _D20, float32 _D21, float32 _D22, float32 _D23,
							float32 _D30, float32 _D31, float32 _D32, float32 _D33)
{
	_00 = _D00; _01 = _D01; _02 = _D02; _03 = _D03;
	_10 = _D10; _11 = _D11; _12 = _D12; _13 = _D13;
	_20 = _D20; _21 = _D21; _22 = _D22; _23 = _D23;
	_30 = _D30; _31 = _D31; _32 = _D32; _33 = _D33;
}

inline Matrix4::Matrix4(const Matrix4 & m)
{
	_00 = m._00; _01 = m._01; _02 = m._02; _03 = m._03;
	_10 = m._10; _11 = m._11; _12 = m._12; _13 = m._13;
	_20 = m._20; _21 = m._21; _22 = m._22; _23 = m._23;
	_30 = m._30; _31 = m._31; _32 = m._32; _33 = m._33;
}

inline Matrix4 & Matrix4::operator= (const Matrix4 & m)
{
	_00 = m._00; _01 = m._01; _02 = m._02; _03 = m._03;
	_10 = m._10; _11 = m._11; _12 = m._12; _13 = m._13;
	_20 = m._20; _21 = m._21; _22 = m._22; _23 = m._23;
	_30 = m._30; _31 = m._31; _32 = m._32; _33 = m._33;
	return *this;
}


inline void	Matrix4::Identity()
{
	data[0] = 1.0f; data[1] = 0.0f; data[2] = 0.0f; data[3] = 0.0f;
	data[4] = 0.0f; data[5] = 1.0f; data[6] = 0.0f; data[7] = 0.0f;
	data[8] = 0.0f; data[9] = 0.0f; data[10] = 1.0f; data[11] = 0.0f;
	data[12] = 0.0f; data[13] = 0.0f; data[14] = 0.0f; data[15] = 1.0f;
}

inline void	Matrix4::Zero()
{
	data[0] = 0; data[1] = 0; data[2] = 0; data[3] = 0;
	data[4] = 0; data[5] = 0; data[6] = 0; data[7] = 0;
	data[8] = 0; data[9] = 0; data[10] = 0; data[11] = 0;
	data[12] = 0; data[13] = 0; data[14] = 0; data[15] = 0;
}

inline void	Matrix4::BuildProjectionFovLH(float32 _fovY, float32 _aspect, float32 _zn, float32 _zf)
{
	// DX9 formula
	Zero();		

	float32 sinF2 = sinf(_fovY / 2.0f);
	float32 cosF2 = cosf(_fovY / 2.0f);

	float h = cosF2 / sinF2;
	float w = h / _aspect;

	_data[0][0] =  w;
	_data[1][1] =  h;
	_data[2][2] = _zf / (_zf - _zn);
	_data[3][2] = (-_zn *_zf ) / (_zf - _zn);
	_data[2][3] = 1.0f;
}


inline void	Matrix4::BuildOrthoLH(float _l, float _r, float32 _b, float32 _t, float32 _zn, float32 _zf)
{
	// DX9 formula
	Zero();		

	_data[0][0] =  2.0f / (_r - _l);
	_data[1][1] =  2.0f / (_t - _b);
	_data[2][2] = 1 / (_zf - _zn);
	
	_data[3][2] = (_zn ) / (_zn - _zf);
	_data[3][0] = (_l + _r) / (_l - _r);
	_data[3][1] = (_b + _t) / (_b - _t);
	_data[3][3] = 1.0f;
}

inline void Matrix4::BuildLookAtMatrixLH(
								const Math::Vector3 & _position, 
								const Math::Vector3 & _target, 
								const Math::Vector3 & _up)
{
	Vector3 left, forward, up;

	forward = _target - _position;
	forward.Normalize();

	left = CrossProduct(_up, forward);
	left.Normalize();

	up = CrossProduct(forward, left);
	up.Normalize();

	Identity();

	Math::Vector3 & vx = left;
	Math::Vector3 & vy = up;
	Math::Vector3 & vz = forward;


	_data[0][0] = vx.x;
	_data[1][0] = vx.y;
	_data[2][0] = vx.z;
	_data[3][0] = -_position.DotProduct(vx);

	_data[0][1] = vy.x;
	_data[1][1] = vy.y;
	_data[2][1] = vy.z;
	_data[3][1] = -_position.DotProduct(vy);

	_data[0][2] = vz.x;
	_data[1][2] = vz.y;
	_data[2][2] = vz.z;
	_data[3][2] = -_position.DotProduct(vz);
}

inline Vector3 operator * (const Vector3 & _v, const Matrix4 & _m)
{
	Vector3 res;
	
	res.x = _v.x * _m._00 + _v.y * _m._10 + _v.z * _m._20 + _m._30;
	res.y = _v.x * _m._01 + _v.y * _m._11 + _v.z * _m._21 + _m._31;
	res.z = _v.x * _m._02 + _v.y * _m._12 + _v.z * _m._22 + _m._32;

    return res;
}


inline void Matrix4::Transpose()
{
	Matrix4 t;
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			t._data[i][j] = _data[j][i];
	*this = t;
}

inline bool Matrix4::GetInverse(Matrix4 & out)
{
	/// Calculates the inverse of this Matrix 
	/// The inverse is calculated using Cramers rule.
	/// If no inverse exists then 'false' is returned.
	const Matrix4 &m = *this;

	float32 d = (m(0, 0) * m(1, 1) - m(1, 0) * m(0, 1)) * (m(2, 2) * m(3, 3) - m(3, 2) * m(2, 3))	- (m(0, 0) * m(2, 1) - m(2, 0) * m(0, 1)) * (m(1, 2) * m(3, 3) - m(3, 2) * m(1, 3))
		+ (m(0, 0) * m(3, 1) - m(3, 0) * m(0, 1)) * (m(1, 2) * m(2, 3) - m(2, 2) * m(1, 3))	+ (m(1, 0) * m(2, 1) - m(2, 0) * m(1, 1)) * (m(0, 2) * m(3, 3) - m(3, 2) * m(0, 3))
		- (m(1, 0) * m(3, 1) - m(3, 0) * m(1, 1)) * (m(0, 2) * m(2, 3) - m(2, 2) * m(0, 3))	+ (m(2, 0) * m(3, 1) - m(3, 0) * m(2, 1)) * (m(0, 2) * m(1, 3) - m(1, 2) * m(0, 3));

	if (d == 0.f)
	return false;

	d = 1.f / d;

	out(0, 0) = d * (m(1, 1) * (m(2, 2) * m(3, 3) - m(3, 2) * m(2, 3)) + m(2, 1) * (m(3, 2) * m(1, 3) - m(1, 2) * m(3, 3)) + m(3, 1) * (m(1, 2) * m(2, 3) - m(2, 2) * m(1, 3)));
	out(1, 0) = d * (m(1, 2) * (m(2, 0) * m(3, 3) - m(3, 0) * m(2, 3)) + m(2, 2) * (m(3, 0) * m(1, 3) - m(1, 0) * m(3, 3)) + m(3, 2) * (m(1, 0) * m(2, 3) - m(2, 0) * m(1, 3)));
	out(2, 0) = d * (m(1, 3) * (m(2, 0) * m(3, 1) - m(3, 0) * m(2, 1)) + m(2, 3) * (m(3, 0) * m(1, 1) - m(1, 0) * m(3, 1)) + m(3, 3) * (m(1, 0) * m(2, 1) - m(2, 0) * m(1, 1)));
	out(3, 0) = d * (m(1, 0) * (m(3, 1) * m(2, 2) - m(2, 1) * m(3, 2)) + m(2, 0) * (m(1, 1) * m(3, 2) - m(3, 1) * m(1, 2)) + m(3, 0) * (m(2, 1) * m(1, 2) - m(1, 1) * m(2, 2)));
	out(0, 1) = d * (m(2, 1) * (m(0, 2) * m(3, 3) - m(3, 2) * m(0, 3)) + m(3, 1) * (m(2, 2) * m(0, 3) - m(0, 2) * m(2, 3)) + m(0, 1) * (m(3, 2) * m(2, 3) - m(2, 2) * m(3, 3)));
	out(1, 1) = d * (m(2, 2) * (m(0, 0) * m(3, 3) - m(3, 0) * m(0, 3)) + m(3, 2) * (m(2, 0) * m(0, 3) - m(0, 0) * m(2, 3)) + m(0, 2) * (m(3, 0) * m(2, 3) - m(2, 0) * m(3, 3)));
	out(2, 1) = d * (m(2, 3) * (m(0, 0) * m(3, 1) - m(3, 0) * m(0, 1)) + m(3, 3) * (m(2, 0) * m(0, 1) - m(0, 0) * m(2, 1)) + m(0, 3) * (m(3, 0) * m(2, 1) - m(2, 0) * m(3, 1)));
	out(3, 1) = d * (m(2, 0) * (m(3, 1) * m(0, 2) - m(0, 1) * m(3, 2)) + m(3, 0) * (m(0, 1) * m(2, 2) - m(2, 1) * m(0, 2)) + m(0, 0) * (m(2, 1) * m(3, 2) - m(3, 1) * m(2, 2)));
	out(0, 2) = d * (m(3, 1) * (m(0, 2) * m(1, 3) - m(1, 2) * m(0, 3)) + m(0, 1) * (m(1, 2) * m(3, 3) - m(3, 2) * m(1, 3)) + m(1, 1) * (m(3, 2) * m(0, 3) - m(0, 2) * m(3, 3)));
	out(1, 2) = d * (m(3, 2) * (m(0, 0) * m(1, 3) - m(1, 0) * m(0, 3)) + m(0, 2) * (m(1, 0) * m(3, 3) - m(3, 0) * m(1, 3)) + m(1, 2) * (m(3, 0) * m(0, 3) - m(0, 0) * m(3, 3)));
	out(2, 2) = d * (m(3, 3) * (m(0, 0) * m(1, 1) - m(1, 0) * m(0, 1)) + m(0, 3) * (m(1, 0) * m(3, 1) - m(3, 0) * m(1, 1)) + m(1, 3) * (m(3, 0) * m(0, 1) - m(0, 0) * m(3, 1)));
	out(3, 2) = d * (m(3, 0) * (m(1, 1) * m(0, 2) - m(0, 1) * m(1, 2)) + m(0, 0) * (m(3, 1) * m(1, 2) - m(1, 1) * m(3, 2)) + m(1, 0) * (m(0, 1) * m(3, 2) - m(3, 1) * m(0, 2)));
	out(0, 3) = d * (m(0, 1) * (m(2, 2) * m(1, 3) - m(1, 2) * m(2, 3)) + m(1, 1) * (m(0, 2) * m(2, 3) - m(2, 2) * m(0, 3)) + m(2, 1) * (m(1, 2) * m(0, 3) - m(0, 2) * m(1, 3)));
	out(1, 3) = d * (m(0, 2) * (m(2, 0) * m(1, 3) - m(1, 0) * m(2, 3)) + m(1, 2) * (m(0, 0) * m(2, 3) - m(2, 0) * m(0, 3)) + m(2, 2) * (m(1, 0) * m(0, 3) - m(0, 0) * m(1, 3)));
	out(2, 3) = d * (m(0, 3) * (m(2, 0) * m(1, 1) - m(1, 0) * m(2, 1)) + m(1, 3) * (m(0, 0) * m(2, 1) - m(2, 0) * m(0, 1)) + m(2, 3) * (m(1, 0) * m(0, 1) - m(0, 0) * m(1, 1)));
	out(3, 3) = d * (m(0, 0) * (m(1, 1) * m(2, 2) - m(2, 1) * m(1, 2)) + m(1, 0) * (m(2, 1) * m(0, 2) - m(0, 1) * m(2, 2)) + m(2, 0) * (m(0, 1) * m(1, 2) - m(1, 1) * m(0, 2)));

	return true;
}
inline bool	Matrix4::Inverse()
{
	Matrix4 temp;
	bool can = GetInverse(temp);
	if (can)
	{
		*this = temp;
	}
	return can;
}

inline Matrix4 Matrix4::operator *(const Matrix4 & m) const
{
	return Matrix4( _00 * m._00 + _01 * m._10 + _02 * m._20 + _03 * m._30,		
					_00 * m._01 + _01 * m._11 + _02 * m._21 + _03 * m._31,		
					_00 * m._02 + _01 * m._12 + _02 * m._22 + _03 * m._32,  
					_00 * m._03 + _01 * m._13 + _02 * m._23 + _03 * m._33,  

					_10 * m._00 + _11 * m._10 + _12 * m._20 + _13 * m._30,		
					_10 * m._01 + _11 * m._11 + _12 * m._21 + _13 * m._31,		
					_10 * m._02 + _11 * m._12 + _12 * m._22 + _13 * m._32, 
					_10 * m._03 + _11 * m._13 + _12 * m._23 + _13 * m._33, 

					_20 * m._00 + _21 * m._10 + _22 * m._20 + _23 * m._30,		
					_20 * m._01 + _21 * m._11 + _22 * m._21 + _23 * m._31,		
					_20 * m._02 + _21 * m._12 + _22 * m._22 + _23 * m._32, 
					_20 * m._03 + _21 * m._13 + _22 * m._23 + _23 * m._33,

					_30 * m._00 + _31 * m._10 + _32 * m._20 + _33 * m._30,		
					_30 * m._01 + _31 * m._11 + _32 * m._21 + _33 * m._31,		
					_30 * m._02 + _31 * m._12 + _32 * m._22 + _33 * m._32, 
					_30 * m._03 + _31 * m._13 + _32 * m._23 + _33 * m._33); 
}

inline const Matrix4 & Matrix4::operator *=(const Matrix4 & m)
{
	*this = (*this) * m;
	return * this;
}


inline void	Matrix4::CreateRotation(const Vector3 & r, float32 angle)
{
	float32 cosA = cosf(angle);
	float32 sinA = sinf(angle);
	Identity();
	_data[0][0] = cosA + (1 - cosA) * r.x * r.x;
	_data[0][1] = (1 - cosA) * r.x * r.y -  r.z * sinA;
	_data[0][2] = (1 - cosA) * r.x * r.z +  r.y * sinA;


	_data[1][0] = (1 - cosA) * r.x * r.y +  r.z * sinA;
	_data[1][1] = cosA + (1 - cosA) * r.y * r.y;
	_data[1][2] = (1 - cosA) * r.y * r.z -  r.x * sinA;


	_data[2][0] = (1 - cosA) * r.x * r.z -  r.y * sinA;
	_data[2][1] = (1 - cosA) * r.y * r.z +  r.x * sinA;
	_data[2][2] = cosA + (1 - cosA) * r.z * r.z;
}

inline void	Matrix4::CreateTranslation(const Vector3 & _v)
{
	Identity();
	_30 = _v.x;
	_31 = _v.y;
	_32 = _v.z;
}

inline void	Matrix4::CreateScale(const Vector3 & _v)
{
	Identity();
	_00 = _v.x;
	_11 = _v.y;
	_22 = _v.z;
}


}; // end of namespace Math
}; // end of namespace Log



#endif // __LOGENGINE_MATRIX_H__


