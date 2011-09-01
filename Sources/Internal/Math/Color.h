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
#ifndef __DAVAENGINE_COLORF_H__
#define __DAVAENGINE_COLORF_H__

namespace DAVA 
{

/**
	\ingroup math
	\brief Class that represents 4-component RGBA float color
	This class is used in all SDK subsystems to work with colors.
 */
class Color
{
public:

	static Color White() { return Color(); }
	static Color Transparent() { return Color(1.f, 1.f, 1.f, 0.f); }
    static Color Clear() { return Color(0.f, 0.f, 0.f, 0.f); }
	static Color Black() { return Color(0.f, 0.f, 0.f, 1.f); }

	union
	{
		float32	color[4];
		struct  
		{
			float32 r, g, b, a;
		};
	};
	
	inline Color();
	inline Color(float32 r, float32 g, float32 b, float32 a);
	
	inline const Color & operator += (const Color & _v);
	inline const Color & operator -= (const Color & _v);
	inline const Color & operator *= (const Color & _v);
	inline const Color & operator *= (float32 f);
	inline const Color & operator /= (float32 f);
	
	//! Comparison operators
	inline bool operator == (const Color & _v) const;
	inline bool operator != (const Color & _v) const;
};

//! with color
inline Color operator - (const Color & _v1, const Color & _v2);
inline Color operator + (const Color & _v1, const Color & _v2);
//! with scalar
inline Color operator + (const Color & _v, float32 _f);
inline Color operator + (float32 _f, const Color & _v);
inline Color operator - (const Color & _v, float32 _f);
inline Color operator - (float32 _f, const Color & _v);
inline Color operator * (const Color & _v, float32 _f);
inline Color operator * (float32 _f, const Color & _v);
inline Color operator * (const Color & _v1, const Color & _v2);
inline Color operator / (const Color & _v, float32 _f);
inline Color operator / (float32 _f, const Color & _v);

	
	
// Color inline implementation	
inline Color::Color()
{
	r = g = b = a = 1.0f;
}
inline Color::Color(float32 _r, float32 _g, float32 _b, float32 _a)
{
	r = _r; g = _g; b = _b; a = _a;
}


//! On operations
inline const Color & Color::operator += (const Color & _v)
{
	r += _v.r;
	g += _v.g;
	b += _v.b;
	a += _v.a;
	return *this;
}

inline const Color & Color::operator -= (const Color & _v)
{
	r -= _v.r;
	g -= _v.g;
	b -= _v.b;
	a -= _v.a;
	return *this;
}

inline const Color & Color::operator *= (const Color & _v)
{
	r *= _v.r;
	g *= _v.g;
	b *= _v.b;
	a *= _v.a;
	return *this; 
}

inline const Color & Color::operator *= (float32 f)
{
	r *= f;
	g *= f;
	b *= f;
	a *= f;
	return *this;
}
inline const Color & Color::operator /= (float32 f)
{
	r /= f;
	g /= f;
	b /= f;
	a /= f;
	return *this;
}

//! Comparison operators
inline bool Color::operator == (const Color & _v) const
{
	return ((r == _v.r) && (g == _v.g) && (b == _v.b) && (a == _v.a));
}
inline bool Color::operator != (const Color & _v) const
{
	return ((r != _v.r) || (g != _v.g) || (b != _v.b) || (a != _v.a));
}

//! operators
inline Color operator - (const Color & _v1, const Color & _v2)
{
	return Color(_v1.r - _v2.r, _v1.g - _v2.g, _v1.b - _v2.b, _v1.a - _v2.a);
}
inline Color operator + (const Color & _v1, const Color & _v2)
{
	return Color(_v1.r + _v2.r, _v1.g + _v2.g, _v1.b + _v2.b, _v1.a + _v2.a);
}

//! with scalar
inline Color operator + (const Color & _v, float32 _f)
{
	return Color(_v.r + _f, _v.g + _f, _v.b + _f, _v.a + _f);
}

inline Color operator + (float32 _f, const Color & _v)
{
	return Color(_v.r + _f, _v.g + _f, _v.b + _f, _v.a + _f);
}

inline Color operator - (const Color & _v, float32 _f)
{
	return Color(_v.r - _f, _v.g - _f, _v.b - _f, _v.a - _f);
}
inline Color operator - (float32 _f, const Color & _v)
{
	return Color(_f - _v.r, _f -_v.g , _f - _v.b, _f - _v.a);
}

inline Color operator * (const Color & _v, float32 _f)
{
	return Color(_v.r * _f, _v.g * _f, _v.b * _f, _v.a * _f);
}

inline Color operator * (float32 _f, const Color & _v)
{
	return Color(_f * _v.r, _f * _v.g , _f * _v.b, _f * _v.a);
}

inline Color operator * (const Color & _v1, const Color & _v2)
{
	return Color(_v1.r * _v2.r, _v1.g * _v2.g, _v1.b * _v2.b,  _v1.a * _v2.a);
}

inline Color operator / (const Color & _v, float32 _f)
{
	return Color(_v.r / _f, _v.g / _f, _v.b / _f, _v.a / _f);
}

inline Color operator / (float32 _f, const Color & _v)
{
	return Color(_f / _v.r, _f / _v.g , _f / _v.b, _f / _v.a);
}
	

};


#endif // __DAVAENGINE_COLORF_H__