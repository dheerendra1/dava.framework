/*======== (C) Copyright 1999 LogGames, L.L.C. All rights reserved. ==========

 The copyright to the contents herein is the property of LogGames, L.L.C.
 The contents may be used and/or copied only with the written permission of
 LogGames, L.L.C., or in accordance with the terms and conditions stipulated in
 the agreement/contract under which the contents have been supplied.

 Revision History
	* Created by Borodovsky Vitaliy
	* 19.03.2006 added full Vector2, Vector3, Vector4 functionality 
				 with unit tests.
				 finished basic functionality

	TODO: Enchanced unit tests
	TODO: Documentation
	TODO: Optimization
=============================================================================*/
#ifndef __LOGENGINE_VECTOR_H__
#define __LOGENGINE_VECTOR_H__

#include "BaseTypes.h"

namespace Log
{
namespace Math
{

//! Vector2
class Vector2
{
public:
	union{
		struct
		{
			float32 x, y;
		};
		float32 data[2];
	};
	
	//! Basic
	inline Vector2(float32 _x = 0, float32 _y = 0);
	inline Vector2(float32 *_data);
	inline Vector2 & operator =(const Vector2 & _v);

	//! On functions
	inline float32 SquareLength() const;
	inline float32 Length() const;
	inline void Normalize();
	
	//! Additional functions
	inline float32 DotProduct(const Vector2 & _v2) const;

	//! On operations
	inline const Vector2 & operator += (const Vector2 & _v);
	inline const Vector2 & operator -= (const Vector2 & _v);
	inline const Vector2 & operator *= (float32 f);
	inline const Vector2 & operator /= (float32 f);
	inline Vector2 operator -() const;

	//! Comparison operators
	inline bool operator == (const Vector2 & _v) const;
	inline bool operator != (const Vector2 & _v) const;
	
};
//! operators
inline Vector2 operator - (const Vector2 & _v1, const Vector2 & _v2);
inline Vector2 operator + (const Vector2 & _v1, const Vector2 & _v2);

//! with scalar
inline Vector2 operator + (const Vector2 & _v, float32 _f);
inline Vector2 operator + (float32 _f, const Vector2 & v);

inline Vector2 operator - (const Vector2 & _v, float32 _f);
inline Vector2 operator - (float32 _f, const Vector2 & v);

inline Vector2 operator * (const Vector2 & _v, float32 _f);
inline Vector2 operator * (float32 _f, const Vector2 & v);

inline Vector2 operator / (const Vector2 & _v, float32 _f);
inline Vector2 operator / (float32 _f, const Vector2 & v);


//! functions
inline float32 DotProduct(const Vector2 & _v1, const Vector2 & _v2);
inline Vector2 Normalize(const Vector2 & _v);


//! Vector3
class Vector3
{
public:
	union{
		struct
		{
			float32 x, y, z;
		};
		float32 data[3];
	};
	
	inline Vector3(float32 _x = 0, float32 _y = 0, float32 _z = 0);
	inline Vector3(float32 *_data);
	inline Vector3 & operator =(const Vector3 & _v);
		
	//! Additional functions
	inline Vector3	CrossProduct(const Vector3 & _v) const;
	inline float32	DotProduct(const Vector3 & _v) const;
	inline void		Lerp(const Vector3 & _v1, const Vector3 & _v2, float32 t);

	//! On functions
	inline float32 SquareLenght();
	inline float32 Length();
	inline void Normalize();
	inline void Clamp(float32 min, float32 max);

	//! On operations
	inline const Vector3 & operator += (const Vector3 & _v);
	inline const Vector3 & operator -= (const Vector3 & _v);
	inline const Vector3 & operator *= (float32 f);
	inline const Vector3 & operator /= (float32 f);
	inline Vector3 operator -() const;

	//! Comparison operators
	inline bool operator == (const Vector3 & _v) const;
	inline bool operator != (const Vector3 & _v) const;	

};

//! operators
inline Vector3 operator - (const Vector3 & _v1, const Vector3 & _v2);
inline Vector3 operator + (const Vector3 & _v1, const Vector3 & _v2);

//! with scalar
inline Vector3 operator + (const Vector3 & _v, float32 _f);
inline Vector3 operator + (float32 _f, const Vector3 & v);

inline Vector3 operator - (const Vector3 & _v, float32 _f);
inline Vector3 operator - (float32 _f, const Vector3 & v);

inline Vector3 operator * (const Vector3 & _v, float32 _f);
inline Vector3 operator * (float32 _f, const Vector3 & v);

inline Vector3 operator / (const Vector3 & _v, float32 _f);
inline Vector3 operator / (float32 _f, const Vector3 & v);


//! additional
inline Vector3 Normalize(const Vector3 & v);
inline Vector3 CrossProduct(const Vector3 & v1, const Vector3 & v2);
inline float32 DotProduct(const Vector3 & v1, const Vector3 & v2);
inline Vector3 Lerp(const Vector3 & _v1, const Vector3 & _v2, float32 t);

//! Vector4
class Vector4
{
public:
	union{
		struct
		{
			float32 x, y, z, w;
		};
		float32 data[4];
	};

	inline Vector4(float32 _x = 0, float32 _y = 0, float32 _z = 0, float32 _w = 0);
	inline Vector4(float32 *_data);
	inline Vector4 & operator =(const Vector4 & _v);

	//! Additional functions
	inline Vector4	CrossProduct(const Vector4 & _v) const;
	inline float32	DotProduct(const Vector4 & _v) const;
	inline void		Lerp(const Vector4 & _v1, const Vector4 & _v2, float32 t);

	//! On functions
	inline float32 SquareLenght();
	inline float32 Length();
	inline void Normalize();
	inline void Clamp(float32 min, float32 max);

	//! On operations
	inline const Vector4 & operator += (const Vector4 & _v);
	inline const Vector4 & operator -= (const Vector4 & _v);
	inline const Vector4 & operator *= (float32 f);
	inline const Vector4 & operator /= (float32 f);
	inline Vector4 operator -() const;

	//! Comparison operators
	inline bool operator == (const Vector4 & _v) const;
	inline bool operator != (const Vector4 & _v) const;	
};

//! operators
inline Vector4 operator - (const Vector4 & _v1, const Vector4 & _v2);
inline Vector4 operator + (const Vector4 & _v1, const Vector4 & _v2);

//! with scalar
inline Vector4 operator + (const Vector4 & _v, float32 _f);
inline Vector4 operator + (float32 _f, const Vector4 & v);

inline Vector4 operator - (const Vector4 & _v, float32 _f);
inline Vector4 operator - (float32 _f, const Vector4 & v);

inline Vector4 operator * (const Vector4 & _v, float32 _f);
inline Vector4 operator * (float32 _f, const Vector4 & v);

inline Vector4 operator / (const Vector4 & _v, float32 _f);
inline Vector4 operator / (float32 _f, const Vector4 & v);


//! additional
inline Vector4 Normalize(const Vector4 & v);
inline Vector4 CrossProduct(const Vector4 & v1, const Vector4 & v2);
inline float32 DotProduct(const Vector4 & v1, const Vector4 & v2);
inline Vector4 Lerp(const Vector4 & _v1, const Vector4 & _v2, float32 t);


// Vector2 Implementation
inline Vector2::Vector2(float32 _x, float32 _y)
{
	x = _x;
	y = _y;
}

inline Vector2::Vector2(float32 *_data)
{
	data[0] = _data[0];
	data[1] = _data[1];
}

inline Vector2 & Vector2::operator =(const Vector2 & _v)
{
	x = _v.x;
	y = _v.y;
	return *this;
}


// On operators

inline const Vector2 & Vector2::operator +=(const Vector2 & _v)
{
	x += _v.x;
	y += _v.y;
	return *this;
}

inline const Vector2 & Vector2::operator -=(const Vector2 & _v)
{
	x -= _v.x;
	y -= _v.y;
	return *this;
}

inline const Vector2 & Vector2::operator *= (float32 _f)
{
	x *= _f;
	y *= _f;
	return *this;
}
inline const Vector2 & Vector2::operator /= (float32 _f)
{
	x /= _f;
	y /= _f;
	return *this;
}

inline Vector2 Vector2::operator -() const
{
	return Vector2(-x, -y);
}

//! Comparison operators
inline bool Vector2::operator == (const Vector2 & _v) const
{
	return ((x == _v.x) && (y == _v.y));
}
inline bool Vector2::operator != (const Vector2 & _v) const
{
	return ((x != _v.x) && (y != _v.y));
}

//! On functions

inline float32 Vector2::SquareLength() const
{
	return x * x + y * y;
}

inline float32 Vector2::Length() const
{
	return sqrtf(SquareLength());
}


inline void Vector2::Normalize()
{
	float32 len = Length();
	x /= len;
	y /= len;
}


//! Additional functions
inline float32 Vector2::DotProduct(const Vector2 & _v2) const
{
	return (x * _v2.x + y * _v2.y);
}

inline Vector2 operator - (const Vector2 & _v1, const Vector2 & _v2)
{
	return Vector2(_v1.x - _v2.x, _v1.y - _v2.y);
}

inline Vector2 operator + (const Vector2 & _v1, const Vector2 & _v2)
{
	return Vector2(_v1.x + _v2.x, _v1.y + _v2.y);
}

//! With scalars
inline Vector2 operator + (const Vector2 & _v, float32 _f)
{
	return Vector2(_v.x + _f, _v.y + _f);
}

inline Vector2 operator + (float32 _f, const Vector2 & _v)
{
	return Vector2(_v.x + _f, _v.y + _f);
}

inline Vector2 operator - (const Vector2 & _v, float32 _f)
{
	return Vector2(_v.x - _f, _v.y - _f);
}

inline Vector2 operator - (float32 _f, const Vector2 & _v)
{
	return Vector2(_f - _v.x, _f - _v.y);
}

inline Vector2 operator * (const Vector2 & _v, float32 _f)
{
	return Vector2(_v.x * _f, _v.y * _f);
}

inline Vector2 operator * (float32 _f, const Vector2 & _v)
{
	return Vector2(_v.x * _f, _v.y * _f);
}

inline Vector2 operator / (const Vector2 & _v, float32 _f)
{
	return Vector2(_v.x / _f, _v.y / _f);
}

inline Vector2 operator / (float32 _f, const Vector2 & _v)
{
	return Vector2(_f / _v.x, _f / _v.y);
}


//! functions
inline float32 DotProduct(const Vector2 & _v1, const Vector2 & _v2)
{
	return _v1.DotProduct(_v2);
}
inline Vector2 Normalize(const Vector2 & _v)
{
	Vector2 v(_v);
	v.Normalize();
	return v;
}	



// Vector3 Implementation
inline Vector3::Vector3(float32 _x, float32 _y, float32 _z)
{
	x = _x;
	y = _y;
	z = _z;
}

inline Vector3::Vector3(float32 *_data)
{
	data[0] = _data[0];
	data[1] = _data[1];
	data[2] = _data[2];
}

inline Vector3 & Vector3::operator =(const Vector3 & _v)
{
	x = _v.x;
	y = _v.y;
	z = _v.z;
	return (*this);
}



//! Additional functions
inline float32 Vector3::DotProduct(const Vector3 & _v) const
{
	return (x * _v.x) + (y * _v.y) + (z * _v.z);
}

inline Vector3 Vector3::CrossProduct(const Vector3 & v) const
{
	return Vector3(y * v.z - v.y * z, z * v.x - x * v.z, x * v.y - y * v.x);
}

//! On functions
inline float32 Vector3::SquareLenght()
{
	return x * x + y * y + z * z;
}
inline float32 Vector3::Length()
{
	return sqrtf(SquareLenght());
}
inline void Vector3::Normalize()
{
	float32 len = Length();
	x /= len;
	y /= len;
	z /= len;
}

inline void Vector3::Lerp(const Vector3 & _v1, const Vector3 & _v2, float32 t)
{
	x = _v1.x * (1.0f - t) + _v2.x * t;
	y = _v1.y * (1.0f - t) + _v2.y * t;
	z = _v1.z * (1.0f - t) + _v2.z * t;
}

inline void Vector3::Clamp(float32 min, float32 max)
{
	if (x < min)x = min;
	if (y < min)y = min;
	if (z < min)z = min;

	if (x > max)x = max;
	if (y > max)y = max;
	if (z > max)z = max;
}

//! On operations
inline const Vector3 & Vector3::operator += (const Vector3 & _v)
{
	x += _v.x;
	y += _v.y;
	z += _v.z;
	return *this;
}

inline const Vector3 & Vector3::operator -= (const Vector3 & _v)
{
	x -= _v.x;
	y -= _v.y;
	z -= _v.z;
	return *this;
}

inline const Vector3 & Vector3::operator *= (float32 f)
{
	x *= f;
	y *= f;
	z *= f;
	return *this;
}
inline const Vector3 & Vector3::operator /= (float32 f)
{
	x /= f;
	y /= f;
	z /= f;
	return *this;
}
inline Vector3 Vector3::operator -() const
{
	return Vector3(-x, -y, -z);
}

//! Comparison operators
inline bool Vector3::operator == (const Vector3 & _v) const
{
	return ((x == _v.x) && (y == _v.y) && (z == _v.z));
}
inline bool Vector3::operator != (const Vector3 & _v) const
{
	return ((x != _v.x) || (y != _v.y) || (z != _v.z));
}


//! operators
inline Vector3 operator - (const Vector3 & _v1, const Vector3 & _v2)
{
	return Vector3(_v1.x - _v2.x, _v1.y - _v2.y, _v1.z - _v2.z);
}
inline Vector3 operator + (const Vector3 & _v1, const Vector3 & _v2)
{
	return Vector3(_v1.x + _v2.x, _v1.y + _v2.y, _v1.z + _v2.z);
}

//! with scalar
inline Vector3 operator + (const Vector3 & _v, float32 _f)
{
	return Vector3(_v.x + _f, _v.y + _f, _v.z + _f);
}

inline Vector3 operator + (float32 _f, const Vector3 & _v)
{
	return Vector3(_v.x + _f, _v.y + _f, _v.z + _f);
}

inline Vector3 operator - (const Vector3 & _v, float32 _f)
{
	return Vector3(_v.x - _f, _v.y - _f, _v.z - _f);
}
inline Vector3 operator - (float32 _f, const Vector3 & _v)
{
	return Vector3(_f - _v.x, _f -_v.y , _f - _v.z );
}

inline Vector3 operator * (const Vector3 & _v, float32 _f)
{
	return Vector3(_v.x * _f, _v.y * _f, _v.z * _f);
}

inline Vector3 operator * (float32 _f, const Vector3 & _v)
{
		return Vector3(_f * _v.x, _f * _v.y , _f * _v.z );
}

inline Vector3 operator / (const Vector3 & _v, float32 _f)
{
	return Vector3(_v.x / _f, _v.y / _f, _v.z / _f);
}

inline Vector3 operator / (float32 _f, const Vector3 & _v)
{
	return Vector3(_f / _v.x, _f / _v.y , _f / _v.z );
}


//! additional
inline Vector3 CrossProduct(const Vector3 & v1, const Vector3 & v2)
{
	return v1.CrossProduct(v2);
}
inline float32 DotProduct(const Vector3 & v1, const Vector3 & v2)
{
	return v1.DotProduct(v2);
}
inline Vector3 Lerp(const Vector3 & _v1, const Vector3 & _v2, float32 t)
{
	Vector3 v;
	v.Lerp(_v1, _v2, t);
	return v;
}

inline Vector3 Normalize(const Vector3 & v)
{
	Vector3 res(v);
	res.Normalize();
	return res;
}


// Vector4 implementation


inline Vector4::Vector4(float32 _x, float32 _y, float32 _z, float32 _w)
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

inline Vector4::Vector4(float32 *_data)
{
	data[0] = _data[0];
	data[1] = _data[1];
	data[2] = _data[2];
	data[3] = _data[3];
}

inline Vector4 & Vector4::operator =(const Vector4 & _v)
{
	x = _v.x;
	y = _v.y;
	z = _v.z;
	w = _v.w;
	return (*this);
}

//! Additional functions
inline float32 Vector4::DotProduct(const Vector4 & _v) const
{
	return (x * _v.x) + (y * _v.y) + (z * _v.z);
}

inline Vector4 Vector4::CrossProduct(const Vector4 & v) const
{
	return Vector4(y * v.z - v.y * z, z * v.x - x * v.z, x * v.y - y * v.x);
}

//! On functions
inline float32 Vector4::SquareLenght()
{
	return x * x + y * y + z * z + w * w;
}
inline float32 Vector4::Length()
{
	return sqrtf(SquareLenght());
}
inline void Vector4::Normalize()
{
	float32 len = Length();
	x /= len;
	y /= len;
	z /= len;
	w /= len;
}

inline void Vector4::Lerp(const Vector4 & _v1, const Vector4 & _v2, float32 t)
{
	x = _v1.x * (1.0f - t) + _v2.x * t;
	y = _v1.y * (1.0f - t) + _v2.y * t;
	z = _v1.z * (1.0f - t) + _v2.z * t;
	w = _v1.w * (1.0f - t) + _v2.w * t;
}

inline void Vector4::Clamp(float32 min, float32 max)
{
	if (x < min)x = min;
	if (y < min)y = min;
	if (z < min)z = min;
	if (w < min)w = min;

	if (x > max)x = max;
	if (y > max)y = max;
	if (z > max)z = max;
	if (w > max)w = max;
}




//! On operations
inline const Vector4 & Vector4::operator += (const Vector4 & _v)
{
	x += _v.x;
	y += _v.y;
	z += _v.z;
	w += _v.w;
	return *this;
}

inline const Vector4 & Vector4::operator -= (const Vector4 & _v)
{
	x -= _v.x;
	y -= _v.y;
	z -= _v.z;
	w -= _v.w;
	return *this;
}

inline const Vector4 & Vector4::operator *= (float32 f)
{
	x *= f;
	y *= f;
	z *= f;
	w *= f;
	return *this;
}
inline const Vector4 & Vector4::operator /= (float32 f)
{
	x /= f;
	y /= f;
	z /= f;
	w /= f;
	return *this;
}
inline Vector4 Vector4::operator -() const
{
	return Vector4(-x, -y, -z, -w);
}

//! Comparison operators
inline bool Vector4::operator == (const Vector4 & _v) const
{
	return ((x == _v.x) && (y == _v.y) && (z == _v.z) && (w == _v.w));
}
inline bool Vector4::operator != (const Vector4 & _v) const
{
	return ((x != _v.x) || (y != _v.y) || (z != _v.z) || (w != _v.w));
}


//! operators
inline Vector4 operator - (const Vector4 & _v1, const Vector4 & _v2)
{
	return Vector4(_v1.x - _v2.x, _v1.y - _v2.y, _v1.z - _v2.z, _v1.w - _v2.w);
}
inline Vector4 operator + (const Vector4 & _v1, const Vector4 & _v2)
{
	return Vector4(_v1.x + _v2.x, _v1.y + _v2.y, _v1.z + _v2.z, _v1.w + _v2.w);
}

//! with scalar
inline Vector4 operator + (const Vector4 & _v, float32 _f)
{
	return Vector4(_v.x + _f, _v.y + _f, _v.z + _f, _v.w + _f);
}

inline Vector4 operator + (float32 _f, const Vector4 & _v)
{
	return Vector4(_v.x + _f, _v.y + _f, _v.z + _f, _v.w + _f);
}

inline Vector4 operator - (const Vector4 & _v, float32 _f)
{
	return Vector4(_v.x - _f, _v.y - _f, _v.z - _f, _v.w - _f);
}
inline Vector4 operator - (float32 _f, const Vector4 & _v)
{
	return Vector4(_f - _v.x, _f -_v.y , _f - _v.z, _f - _v.w);
}

inline Vector4 operator * (const Vector4 & _v, float32 _f)
{
	return Vector4(_v.x * _f, _v.y * _f, _v.z * _f, _v.w * _f);
}

inline Vector4 operator * (float32 _f, const Vector4 & _v)
{
	return Vector4(_f * _v.x, _f * _v.y , _f * _v.z, _f * _v.w);
}

inline Vector4 operator / (const Vector4 & _v, float32 _f)
{
	return Vector4(_v.x / _f, _v.y / _f, _v.z / _f, _v.w / _f);
}

inline Vector4 operator / (float32 _f, const Vector4 & _v)
{
	return Vector4(_f / _v.x, _f / _v.y , _f / _v.z, _f / _v.w);
}


//! additional
inline Vector4 CrossProduct(const Vector4 & v1, const Vector4 & v2)
{
	return v1.CrossProduct(v2);
}
inline float32 DotProduct(const Vector4 & v1, const Vector4 & v2)
{
	return v1.DotProduct(v2);
}
inline Vector4 Lerp(const Vector4 & _v1, const Vector4 & _v2, float32 t)
{
	Vector4 v;
	v.Lerp(_v1, _v2, t);
	return v;
}

inline Vector4 Normalize(const Vector4 & v)
{
	Vector4 res(v);
	res.Normalize();
	return res;
}


};
};


#endif // __LOGENGINE_VECTOR_H__

