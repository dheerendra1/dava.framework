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
#include "Base/BaseMath.h"

namespace DAVA 
{
	
/*
 x(t) = axt3 + bxt2 + cxt + x0
 
 x1 = x0 + cx / 3
 x2 = x1 + (cx + bx) / 3
 x3 = x0 + cx + bx + ax
 
 y(t) = ayt3 + byt2 + cyt + y0
 
 y1 = y0 + cy / 3
 y2 = y1 + (cy + by) / 3
 y3 = y0 + cy + by + ay
 
 This method of definition can be reverse-engineered so that it'll 
 give up the coefficient values based on the points described above:
 
 cx = 3 (x1 - x0)
 bx = 3 (x2 - x1) - cx
 ax = x3 - x0 - cx - bx
 
 cy = 3 (y1 - y0)
 by = 3 (y2 - y1) - cy
 ay = y3 - y0 - cy - by
*/
	
BezierSplineSegment3::BezierSplineSegment3(const Vector3 & p0, const Vector3 & p1, const Vector3 &p2, const Vector3 & p3)
{
	d = p0;
	c = 3 * (p1 - p0);
	b = 3 * (p2 - p1) - c;
	a = p3 - p0 - b - c;
}
	
BezierSplineSegment3::~BezierSplineSegment3()
{
	
}

Vector3 BezierSplineSegment3::Evaluate(float t)
{
	float t2 = t * t;
	float t3 = t2 * t;
	Vector3 r = a * t3 + b * t2 + c * t + d;
	return r;
}
	
BezierSpline2::BezierSpline2()
{
	pointCount = 0;
	isConstructed = false;
}

BezierSpline2::~BezierSpline2()
{
	
}

void BezierSpline2::AddPoint(const Vector2 & pt)
{
	IPoint p;
	p.point = pt;
	pointCount++;
	points.push_back(p);
	isConstructed = false;
}

void BezierSpline2::Construct()
{
	if (isConstructed)return;
	if (GetPointCount() == 4)
	{
		d = points[0].point;
		c = 3 * (points[1].point - points[0].point);
		b = 3 * (points[2].point - points[1].point) - c;
		a = points[3].point - points[0].point - b - c;
		isConstructed = true;
	}
}	

Vector2 BezierSpline2::Evaluate(int segment, float t)
{
	Construct();
	if (segment == 0)
	{
		float t2 = t * t;
		float t3 = t2 * t;
		Vector2 r = a * t3 + b * t2 + c * t + d;
		return r;
	}
	return Vector2();
}


BezierSpline3::BezierSpline3()
{
	pointCount = 0;
	isConstructed = false;
}

BezierSpline3::~BezierSpline3()
{
	
}

void BezierSpline3::AddPoint(const Vector3 & pt)
{
	IPoint p;
	p.point = pt;
	pointCount++;
	points.push_back(p);
	isConstructed = false;
}

void BezierSpline3::Construct()
{
	if (isConstructed)return;
	if (GetPointCount() == 4)
	{
		d = points[0].point;
		c = 3 * (points[1].point - points[0].point);
		b = 3 * (points[2].point - points[1].point) - c;
		a = points[3].point - points[0].point - b - c;
		isConstructed = true;
	}
}	
	
Vector3 BezierSpline3::Evaluate(int segment, float t)
{
	Construct();
	if (segment == 0)
	{
		float t2 = t * t;
		float t3 = t2 * t;
		Vector3 r = a * t3 + b * t2 + c * t + d;
		return r;
	}
	return Vector3();
}

	

}


