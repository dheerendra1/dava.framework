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
#ifndef __DAVAENGINE_RECT_H__
#define __DAVAENGINE_RECT_H__

#include <math.h>

namespace DAVA
{
	
/**	
	\ingroup math
	\brief Rect in 2D space. This class is used for all rectangles along all SDK subsystems.
 */
struct Rect
{
	float32 x;
	float32 y;
	float32 dx;
	float32 dy;

	inline Rect();
	inline Rect(float32 _x, float32 _y, float32 _dx, float32 _dy);
	inline Rect(const Rect & rect);
	inline Rect(const Vector2 & point, const Vector2 & size);

	inline bool PointInside(const Vector2 & point) const; 
	inline Rect Intersection(const Rect & rect) const;
	inline bool RectIntersects(const Rect & rect) const;
	inline void ClampToRect(Vector2 & point) const;

	inline Vector2 GetCenter() const;
	inline Vector2 GetPosition() const;
	inline Vector2 GetSize() const;
	
	inline void SetCenter(const Vector2 & center);
	inline void SetPosition(const Vector2 & position);
	inline void SetSize(const Vector2 & size);

	inline bool operator == (const Rect  & _s)const;
	inline bool operator != (const Rect  & _s)const;

	inline Rect &	operator += (const Vector2 & point);
	inline Rect &	operator -= (const Vector2 & point);

	inline Rect 	operator + (const Vector2 & Point) const;
	inline Rect 	operator - (const Vector2 & Point) const;
};



inline Rect::Rect()
{
	x = y = dx = dy = 0;
};

inline Rect::Rect(float32 _x, float32 _y, float32 _dx, float32 _dy)
{
	x = _x;
	y = _y;
	dx = _dx;
	dy = _dy;
};

inline Rect::Rect(const Rect & rect)
{
	x = rect.x;
	y = rect.y;
	dx = rect.dx;
	dy = rect.dy;
};

inline bool Rect::PointInside(const Vector2 & point) const
{	
    if ((point.x >= x) && (point.x <= x + dx) 
		&& (point.y >= y) && (point.y <= y + dy))
			return true;
	return false;
}

inline bool Rect::RectIntersects(const Rect & rect) const
{
	float32 left1, left2;
	float32 right1, right2;
	float32 top1, top2;
	float32 bottom1, bottom2;

	left1 = x;
	left2 = rect.x;
	right1 = x + dx;
	right2 = rect.x + rect.dx;
	top1 = y;
	top2 = rect.y;
	bottom1 = y + dy;
	bottom2 = rect.y + rect.dy;

	if (bottom1 < top2) return(false);
	if (top1 > bottom2) return(false);

	if (right1 < left2) return(false);
	if (left1 > right2) return(false);

	return(true);
}

inline Rect Rect::Intersection(const Rect & rect) const
{
	float32 nx = Max(x, rect.x);
	float32 ny = Max(y, rect.y);
	float32 ndx = Min((dx + x) - nx, (rect.dx + rect.x) - nx);
	float32 ndy = Min((dy + y) - ny, (rect.dy + rect.y) - ny);
	if (ndx <= 0 || ndy <= 0)
	{
		ndx = 0;
		ndy = 0;
	}
	
	return Rect(nx, ny, ndx, ndy);
}
	
inline void Rect::ClampToRect(Vector2 & point) const
{
	if (point.x < x)point.x = x;
	if (point.y < y)point.y = y;
	if (point.x > x + dx)point.x = x + dx;
	if (point.y > y + dy)point.y = y + dy;
}

inline Vector2 Rect::GetCenter() const
{
	return Vector2(x + (dx * .5f), y + (dy * .5f));
}

inline Vector2 Rect::GetPosition() const
{
	return Vector2(x, y);
}

inline Vector2 Rect::GetSize() const
{
	return Vector2(dx, dy);
}

inline void Rect::SetCenter(const Vector2 & center)
{
	x = center.x - dx * 0.5f;
	y = center.y - dy * 0.5f;
}

inline void Rect::SetPosition(const Vector2 & position)
{
	x = position.x;
	y = position.y;
}

inline void Rect::SetSize(const Vector2 & size)
{
	dx = size.dx;
	dy = size.dy;
}

inline bool Rect::operator == (const Rect & _r)const
{
	return (x == _r.x && y == _r.y && dx == _r.dx && dy == _r.dy);
}

inline bool Rect::operator != (const Rect & _r)const
{
	return (!Rect::operator==(_r));
}

inline Rect & Rect::operator += (const Vector2 & pt)
{
	x += pt.x;
	y += pt.y;
	return *this;
}

inline Rect & Rect::operator -= (const Vector2 & pt)
{
	x -= pt.x;
	y -= pt.y;
	return *this;
}

inline Rect	Rect::operator + (const Vector2 & pt) const
{
	return Rect( x + pt.x, y + pt.y, dx, dy);
}

inline Rect Rect::operator - (const Vector2 & pt) const
{
	return Rect( x - pt.x, y - pt.y, dx, dy);
}


}; // end of namespace DAVA


#endif // __DAVAENGINE_MATH2DBASE_H__