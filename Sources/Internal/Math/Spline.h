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
#ifndef __DAVAENGINE_SPLINE_H__
#define __DAVAENGINE_SPLINE_H__

#include "Base/BaseTypes.h"
#include "Math/Vector.h"
#include "Math/Plane.h"

namespace DAVA
{

// Base spline class 
// all splines must be childs of this class
// all splines is 3D functions and they parametric

class Spline
{
public:	
	Spline(){};
	virtual ~Spline(){};
	
	virtual int GetPointCount();
	virtual Vector3 Evaluate(float32 t);
};	
	

//! Class for creation of basic and simple splines through n points

class BasicSpline2
{
public:
	struct SplinePoint
	{
		Vector2 point;	// 
		Vector2 r;		// 
	};
	int					pointCount;
	Vector<SplinePoint> points;
	
	BasicSpline2();
	BasicSpline2(const Polygon2 & poly);
	
	void	Construct(const Polygon2 & poly);
	void	RebuildSpline();
	Vector2 Evaluate(int segment, float t);
};
	
class BasicSpline3
{
public:
	struct SplinePoint
	{
		Vector3 point;	// 
		Vector3 r;		// 
	};
	int					pointCount;
	Vector<SplinePoint> points;
	
	BasicSpline3();
	BasicSpline3(const Polygon3 & poly);
		
	void	Construct(const Polygon3 & poly);
	void	RebuildSpline();
	Vector3 Evaluate(int segment, float t);
};

};

#endif // __DAVAENGINE_SPLINE_H__

