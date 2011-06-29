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
BasicSpline3::BasicSpline3()
{
	pointCount = 0;
}
	
BasicSpline3::BasicSpline3(const Polygon3 & poly)
{
	Construct(poly);
}
	
void BasicSpline3::Construct(const Polygon3 & poly)
{
	points.clear();
	pointCount = 0;
	
	for (int k = 0; k < poly.pointCount; ++k)
	{
		SplinePoint point;
		point.point = poly.points[k];
		points.push_back(point);
	}
	pointCount = poly.pointCount;
	RebuildSpline();
}

void BasicSpline3::RebuildSpline()
{
	if (pointCount < 4)return;
	
	points[0].r = points[1].point - points[0].point;
	points[pointCount - 1].r = points[pointCount - 1].point - points[pointCount - 2].point;
	
	for (int k = 1; k < pointCount - 1; ++k)
	{
//		Vector3 g1 = points[k].point - points[k - 1].point;
//		Vector3 g2 = points[k + 1].point - points[k].point;
//		Vector3 g3 = g2 - g1;
//		points[k].r = g1 + 0.5f * g3;
		points[k].r = (points[k + 1].point - points[k - 1].point) * 0.5f; 
		// to smooth spline this vector can be increased but no more than 3 * cos(a) (a angle between segments)
	}
}
	
Vector3 BasicSpline3::Evaluate(int segment, float t)
{
	//int segment = tglobal / (pointCount - 1);
	//float t = fmod(tglobal, 1.0f);
	
	Vector3 p1 = points[segment].point;
	Vector3 p2 = p1;
	Vector3 r1 = points[segment].r;
	Vector3 r2 = r1; 
	if (segment + 1 < pointCount)
	{
		p2 = points[segment + 1].point;
		r2 = points[segment + 1].r;
	}
	
	Vector3 a = 2 * p1 - 2 * p2 + r1 + r2;
	Vector3 b = -3 * p1 + 3 * p2 - 2 * r1 - r2;
	Vector3 c = r1;
	Vector3 d = p1;
	
	Vector3 res = a * t * t * t + b * t * t + c * t + d;
	return res;
}


BasicSpline2::BasicSpline2()
{
	pointCount = 0;
}

BasicSpline2::BasicSpline2(const Polygon2 & poly)
{
	Construct(poly);
}

void BasicSpline2::Construct(const Polygon2 & poly)
{
	points.clear();
	pointCount = 0;
	
	for (int k = 0; k < poly.pointCount; ++k)
	{
		SplinePoint point;
		point.point = poly.points[k];
		points.push_back(point);
	}
	pointCount = poly.pointCount;
	RebuildSpline();
}

void BasicSpline2::RebuildSpline()
{
	if (pointCount < 4)return;
	
	points[0].r = points[1].point - points[0].point;
	points[pointCount - 1].r = points[pointCount - 1].point - points[pointCount - 2].point;
	
	for (int k = 1; k < pointCount - 1; ++k)
	{
		//		Vector3 g1 = points[k].point - points[k - 1].point;
		//		Vector3 g2 = points[k + 1].point - points[k].point;
		//		Vector3 g3 = g2 - g1;
		//		points[k].r = g1 + 0.5f * g3;
		points[k].r = (points[k + 1].point - points[k - 1].point) * 0.5f; 
		// to smooth spline this vector can be increased but no more than 3 * cos(a) (a angle between segments)
	}
}

Vector2 BasicSpline2::Evaluate(int segment, float t)
{
	//int segment = tglobal / (pointCount - 1);
	//float t = fmod(tglobal, 1.0f);
	
	Vector2 p1 = points[segment].point;
	Vector2 p2 = p1;
	Vector2 r1 = points[segment].r;
	Vector2 r2 = r1; 
	if (segment + 1 < pointCount)
	{
		p2 = points[segment + 1].point;
		r2 = points[segment + 1].r;
	}
	
	Vector2 a = 2 * p1 - 2 * p2 + r1 + r2;
	Vector2 b = -3 * p1 + 3 * p2 - 2 * r1 - r2;
	Vector2 c = r1;
	Vector2 d = p1;
	
	Vector2 res = a * t * t * t + b * t * t + c * t + d;
	return res;
}

	
	
};






