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
#ifndef __DAVAENGINE_BEZIER_SPLINE_H__
#define __DAVAENGINE_BEZIER_SPLINE_H__

#include "Base/BaseTypes.h"
#include "Math/Vector.h"
#include "Math/Plane.h"

namespace DAVA
{
	
class BezierSplineSegment3
{
public:
	BezierSplineSegment3(const Vector3 & p1, const Vector3 & p2, const Vector3 &p3, const Vector3 & p4);
	~BezierSplineSegment3();
	
	Vector3 Evaluate(float t);
private:
	Vector3 a;
	Vector3 b;
	Vector3 c;
	Vector3 d;
};	
	
	
class BezierSpline2//why spline is not a base object?
{
public:
	BezierSpline2();
	~BezierSpline2();
	
	void AddPoint(const Vector2 & pt);
	int	 GetPointCount() const;	
	Vector2 Evaluate(int segment, float t);
	
private:
	bool	isConstructed;
	
	void	Construct();
	
	
	struct IPoint
	{
		Vector2 point;	 
	};
	int					pointCount;
	Vector<IPoint>		points;
	
	Vector2 a, b, c, d;
};

inline int BezierSpline2::GetPointCount() const 
{
	return pointCount;
}

//! Class for creation of basic and simple splines through n points

class BezierSpline3//why spline is not a base object?
{
public:
	BezierSpline3();
	~BezierSpline3();
	
	void AddPoint(const Vector3 & pt);
	int	 GetPointCount() const;	
	Vector3 Evaluate(int segment, float t);

private:
	bool	isConstructed;
	
	void	Construct();
	
	
	struct IPoint
	{
		Vector3 point;	 
	};
	int					pointCount;
	Vector<IPoint>		points;

	Vector3 a, b, c, d;
};
	
inline int BezierSpline3::GetPointCount() const 
{
	return pointCount;
}
	
};

#endif // __DAVAENGINE_SPLINE_H__

