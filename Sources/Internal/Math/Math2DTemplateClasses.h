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
#ifndef __DAVAENGINE_MATH2DBASE_H__
#define __DAVAENGINE_MATH2DBASE_H__

#include <math.h>
#include "Math/Matrix2.h"
#include "Math/Matrix3.h"

namespace DAVA
{

struct Matrix2;

//! Base class template for 2D points
template <class TYPE> 
struct Point2Base
{
	union
	{
		struct{
			TYPE x, y;
		};
		TYPE data[2];
	};
	
	inline Point2Base();
	inline Point2Base(TYPE _x, TYPE _y);
	inline Point2Base(const Point2Base<TYPE> & Point);
	
	inline Point2Base<TYPE> &	operator += (const Point2Base<TYPE> & Point);
	inline Point2Base<TYPE> &	operator -= (const Point2Base<TYPE> & Point);

	inline Point2Base<TYPE> 	operator + (const Point2Base<TYPE> & Point) const;
	inline Point2Base<TYPE> 	operator - (const Point2Base<TYPE> & Point) const;


	inline Point2Base<TYPE>		operator * (const TYPE t) const;
	inline Point2Base<TYPE>	&	operator *= (const TYPE t);

	inline Point2Base<TYPE>		operator * (const Matrix2 & Matrix) const;
	inline Point2Base<TYPE>	&	operator *= (const Matrix2 & Matrix);

	inline Point2Base<TYPE>		operator * (const Matrix3 & Matrix) const;
	inline Point2Base<TYPE>	&	operator *= (const Matrix3 & Matrix);
	

	inline bool operator == (const Point2Base<TYPE> & )const;
	inline bool operator != (const Point2Base<TYPE> & )const;
	

	// Vector2 operations (for using 2d point as 2d vector)
	inline TYPE Lenght();
	inline void Normalize();

};

template <class TYPE> 
inline TYPE LineLength(Point2Base<TYPE> p1, Point2Base<TYPE> p2);

//! Base class template for 2D sizes
template <class TYPE> 
struct Size2Base
{
	TYPE dx, dy;
	inline Size2Base();
	inline Size2Base(TYPE _dx, TYPE _dy);
	inline Size2Base(const Size2Base<TYPE> & Size);	
	
	
	inline bool operator == (const Size2Base<TYPE> & _s)const;
	inline bool operator != (const Size2Base<TYPE> & _s)const;
};

//! Base class template for 2D rects
template <class TYPE> 
struct Rect2Base
{
	TYPE x;
	TYPE y;
	TYPE dx;
	TYPE dy;

	inline Rect2Base();
	inline Rect2Base(TYPE _x, TYPE _y, TYPE _dx, TYPE _dy);
	inline Rect2Base(const Rect2Base<TYPE> & Rect);
	inline Rect2Base(const Point2Base<TYPE> & Point, const Size2Base<TYPE> & Size);
	inline bool PointInside(const Point2Base<TYPE> & Point) const; 
	inline Rect2Base<TYPE> Intersection(const Rect2Base<TYPE> & Rect) const;
	inline bool RectIntersects(const Rect2Base<TYPE> & Rect) const;

	inline Point2Base<TYPE> GetCenter() const;
	inline Point2Base<TYPE> GetPosition() const;
	inline Size2Base<TYPE> GetSize() const;
	
	inline void SetCenter(const Point2Base<TYPE>& center);
	inline void SetPosition(const Point2Base<TYPE>& position);
	inline void SetSize(const Size2Base<TYPE>& size);

	inline bool operator == (const Rect2Base<TYPE> & _s)const;
	inline bool operator != (const Rect2Base<TYPE> & _s)const;

	inline Rect2Base<TYPE> &	operator += (const Point2Base<TYPE> & Point);
	inline Rect2Base<TYPE> &	operator -= (const Point2Base<TYPE> & Point);

	inline Rect2Base<TYPE> 	operator + (const Point2Base<TYPE> & Point) const;
	inline Rect2Base<TYPE> 	operator - (const Point2Base<TYPE> & Point) const;
};



// Point2Base implementation

template <class TYPE>
inline Point2Base<TYPE>::Point2Base()
{
	x = y = 0;
}

template <class TYPE>
inline Point2Base<TYPE>::Point2Base(TYPE _x, TYPE _y)
{
	x = _x;
	y = _y;
};

template <class TYPE>
inline Point2Base<TYPE>::Point2Base(const Point2Base<TYPE> & Point)
{
	x = Point.x;
	y = Point.y;
}

template <class TYPE> 
inline TYPE LineLength(Point2Base<TYPE> p1, Point2Base<TYPE> p2)
{
	return (TYPE)sqrtf(((float32)p1.x - (float32)p2.x) * ((float32)p1.x - (float32)p2.x) + ((float32)p1.y - (float32)p2.y) * ((float32)p1.y - (float32)p2.y));
}

// Size2Base implementation

template <class TYPE>
inline Size2Base<TYPE>::Size2Base()
{
	dx = dy = 0;
};

template <class TYPE>
inline Size2Base<TYPE>::Size2Base(TYPE _dx, TYPE _dy)
{
	dx = _dx;
	dy = _dy;
};

template <class TYPE>
inline Size2Base<TYPE>::Size2Base(const Size2Base<TYPE> & Size)
{
	dx = Size.dx;
	dy = Size.dy;
};

template <class TYPE>
inline Rect2Base<TYPE>::Rect2Base(const Point2Base<TYPE> & Point, const Size2Base<TYPE> & Size)
{
	x = Point.x;
	y = Point.y;
	dx = Size.dx;
	dy = Size.dy;
};

template <class TYPE>
inline Point2Base<TYPE> & Point2Base<TYPE>::operator+=(const Point2Base<TYPE> & Point)
{
	x += Point.x;
	y += Point.y;
    return (*this);
};

template <class TYPE>
inline Point2Base<TYPE> & Point2Base<TYPE>::operator-=(const Point2Base<TYPE> & Point)
{
	x -= Point.x;
	y -= Point.y;
    return  *this;
};

template <class TYPE>
inline Point2Base<TYPE> Point2Base<TYPE>::operator + (const Point2Base<TYPE> & Point) const
{
	return Point2Base<TYPE> ( x + Point.x, y + Point.y);
}

template <class TYPE>
inline Point2Base<TYPE> Point2Base<TYPE>::operator - (const Point2Base<TYPE> & Point) const
{
	return Point2Base<TYPE> ( x - Point.x, y - Point.y);
}

template <class TYPE>
inline Point2Base<TYPE>	Point2Base<TYPE>::operator * (const Matrix2 & Matrix) const
{
	return Point2Base<TYPE>( x * Matrix._00 + y * Matrix._10, 
						 x * Matrix._01 + y * Matrix._11);
}

template <class TYPE>
inline Point2Base<TYPE>& Point2Base<TYPE>::operator *= (const Matrix2 & Matrix)
{
	return (*this = *this * Matrix);
}

template <class TYPE>
inline Point2Base<TYPE>	Point2Base<TYPE>::operator * (const TYPE t) const
{
	return Point2Base<TYPE>( x * t, y * t);
}

template <class TYPE>
inline Point2Base<TYPE>& Point2Base<TYPE>::operator *= (const TYPE t)
{
	return (*this = *this * t);
}

template <class TYPE>
inline Point2Base<TYPE>	Point2Base<TYPE>::operator * (const Matrix3 & Matrix) const
{
	return Point2Base<TYPE>( x * Matrix._00 + y * Matrix._10 + Matrix._20, 
						 x * Matrix._01 + y * Matrix._11 + Matrix._21);
}

template <class TYPE>
inline Point2Base<TYPE>& Point2Base<TYPE>::operator *= (const Matrix3 & Matrix)
{
	return (*this = *this * Matrix);
}

template <class TYPE>
inline bool Point2Base<TYPE>::operator == (const Point2Base<TYPE> & _p)const
{
	return (x == _p.x && y == _p.y);
}

template <class TYPE>
inline bool Point2Base<TYPE>::operator != (const Point2Base<TYPE> & _p)const
{
	return (x != _p.x || y != _p.y);
}

template <class TYPE>
inline TYPE Point2Base<TYPE>::Lenght()
{
	return (TYPE) (sqrt(x * x + y * y));
}

template <class TYPE>
inline void Point2Base<TYPE>::Normalize()
{
	TYPE Len = Lenght();
	x /= Len;
	y /= Len;
}


// Size2Base implementation

template <class TYPE>
inline bool Size2Base<TYPE>::operator == (const Size2Base<TYPE> & _s)const
{
	return (dx == _s.dx && dy == _s.dy);
}

template <class TYPE>
inline bool Size2Base<TYPE>::operator != (const Size2Base<TYPE> & _s)const
{
	return (dx != _s.dx || dy != _s.dy);
}


// Rect2Base implementation
template <class TYPE>
inline Rect2Base<TYPE>::Rect2Base()
{
	x = y = dx = dy = 0;
};

template <class TYPE>
inline Rect2Base<TYPE>::Rect2Base(TYPE _x, TYPE _y, TYPE _dx, TYPE _dy)
{
	x = _x;
	y = _y;
	dx = _dx;
	dy = _dy;
};

template <class TYPE>
inline Rect2Base<TYPE>::Rect2Base(const Rect2Base<TYPE> & Rect)
{
	x = Rect.x;
	y = Rect.y;
	dx = Rect.dx;
	dy = Rect.dy;
};

template <class TYPE>
inline bool Rect2Base<TYPE>::PointInside(const Point2Base<TYPE> & Point) const
{	
    if ((Point.x >= x) && (Point.x <= x + dx) 
		&& (Point.y >= y) && (Point.y <= y + dy))
			return true;
	return false;
}

template <class TYPE>
inline bool Rect2Base<TYPE>::RectIntersects(const Rect2Base<TYPE> & Rect) const
{
	int left1, left2;
	int right1, right2;
	int top1, top2;
	int bottom1, bottom2;

	left1 = x;
	left2 = Rect.x;
	right1 = x + dx;
	right2 = Rect.x + Rect.dx;
	top1 = y;
	top2 = Rect.y;
	bottom1 = y + dy;
	bottom2 = Rect.y + Rect.dy;

	if (bottom1 < top2) return(false);
	if (top1 > bottom2) return(false);

	if (right1 < left2) return(false);
	if (left1 > right2) return(false);

	return(true);
}

template <class TYPE> 
inline Rect2Base<TYPE> Rect2Base<TYPE>::Intersection(const Rect2Base<TYPE> & Rect) const
{
	TYPE nx = Max(x, Rect.x);
	TYPE ny = Max(y, Rect.y);
	TYPE ndx = Min((dx + x) - nx, (Rect.dx + Rect.x) - nx);
	TYPE ndy = Min((dy + y) - ny, (Rect.dy + Rect.y) - ny);
	if (ndx <= 0 || ndy <= 0)
	{
		ndx = 0;
		ndy = 0;
	}
	
	return Rect2Base<TYPE>(nx, ny, ndx, ndy);
}

template <class TYPE> 
inline Point2Base<TYPE> Rect2Base<TYPE>::GetCenter() const
{
	return Point2Base<TYPE>(x + (dx * .5f), y + (dy * .5f));
}

template <class TYPE> 
inline Point2Base<TYPE> Rect2Base<TYPE>::GetPosition() const
{
	return Point2Base<TYPE>(x, y);
}

template <class TYPE> 
inline Size2Base<TYPE> Rect2Base<TYPE>::GetSize() const
{
	return Size2Base<TYPE>(dx, dy);
}

template <class TYPE> 
inline void Rect2Base<TYPE>::SetCenter(const Point2Base<TYPE>& center)
{
	x = center.x - dx*0.5f;
	y = center.y - dy*0.5f;
}

template <class TYPE> 
inline void Rect2Base<TYPE>::SetPosition(const Point2Base<TYPE>& position)
{
	x = position.x;
	y = position.y;
}

template <class TYPE> 
inline void Rect2Base<TYPE>::SetSize(const Size2Base<TYPE>& size)
{
	dx = size.dx;
	dy = size.dy;
}

template <class TYPE>
inline bool Rect2Base<TYPE>::operator == (const Rect2Base<TYPE> & _r)const
{
	return (x == _r.x && y == _r.y && dx == _r.dx && dy == _r.dy);
}

template <class TYPE>
inline bool Rect2Base<TYPE>::operator != (const Rect2Base<TYPE> & _r)const
{
	return (!Rect2Base<TYPE>::operator==(_r));
}

template <class TYPE>
inline Rect2Base<TYPE> &	Rect2Base<TYPE>::operator += (const Point2Base<TYPE> & pt)
{
	x += pt.x;
	y += pt.y;
	return *this;
}

template <class TYPE>
inline Rect2Base<TYPE> &	Rect2Base<TYPE>::operator -= (const Point2Base<TYPE> & pt)
{
	x -= pt.x;
	y -= pt.y;
	return *this;
}

template <class TYPE>
inline Rect2Base<TYPE> 	Rect2Base<TYPE>::operator + (const Point2Base<TYPE> & pt) const
{
	return Rect2Base<TYPE> ( x + pt.x, y + pt.y, dx, dy);
}

template <class TYPE>
inline Rect2Base<TYPE> 	Rect2Base<TYPE>::operator - (const Point2Base<TYPE> & pt) const
{
	return Rect2Base<TYPE> ( x - pt.x, y - pt.y, dx, dy);
}


}; // end of namespace DAVA


#endif // __DAVAENGINE_MATH2DBASE_H__