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
#include "Math/MathConstants.h"
#include "Math/Math2D.h"

namespace DAVA
{
#define B (4 / PI)
#define C (-4 / (PI * PI))
//  const float Q = 0.775;
#define P (0.225f)
#define ONE_DIV_TWO_PI (1.0f / PI_2)

	/*
	 Functions not finished yet
	 After investigation realized that it's no make sense on 3GS, default implementation is faster
	 */	
	float32 FastSin(float32 x)
	{
		//while(x >= PI)x -= PI_2;
		//while(x <= -PI)x += PI_2;
		int32 v = (x + PI) * ONE_DIV_TWO_PI;
		x -= v * PI_2;
		x -= PI;
			
		float32 y = B * x + C * x * Abs(x);
		y = P * (y * Abs(y) - y) + y;   // Q * y + P * y * abs(y)
		return y;
	}
//	Same function optimized for 3GS / Neon
//	Do not know how to compile
//	// Computes the sine and cosine of two angles
//	/// in: angles = Two angles, expressed in radians, in the [-PI,PI] range.
//	/// out: results = vector containing [sin(angles[0]),cos(angles[0]),sin(angles[1]),cos(angles[1])]
//	static inline void vsincos(const float angles[2], float results[4]) {
//		static const float constants[]  = { 
//			/* q1 */  0,                M_PI_2,           0,                M_PI_2,
//			/* q2 */  M_PI,             M_PI,             M_PI,             M_PI,
//			/* q3 */  4.f/M_PI,         4.f/M_PI,         4.f/M_PI,         4.f/M_PI,
//			/* q4 */ -4.f/(M_PI*M_PI), -4.f/(M_PI*M_PI), -4.f/(M_PI*M_PI), -4.f/(M_PI*M_PI),
//			/* q5 */  2.f,              2.f,              2.f,              2.f,
//			/* q6 */  .225f,            .225f,            .225f,            .225f
//		};  
//		asm volatile(
//					 // Load q0 with [angle1,angle1,angle2,angle2]
//					 "vldmia %1, { d3 }\n\t"
//					 "vdup.f32 d0, d3[0]\n\t"
//					 "vdup.f32 d1, d3[1]\n\t"
//					 // Load q1-q6 with constants
//					 "vldmia %2, { q1-q6 }\n\t"
//					 // Cos(x) = Sin(x+PI/2), so
//					 // q0 = [angle1, angle1+PI/2, angle2, angle2+PI/2]
//					 "vadd.f32 q0,q0,q1\n\t"
//					 // if angle1+PI/2>PI, substract 2*PI
//					 // q0-=(q0>PI)?2*PI:0
//					 "vcge.f32 q1,q0,q2\n\t"
//					 "vand.f32 q1,q1,q2\n\t"
//					 "vmls.f32 q0,q1,q5\n\t"
//					 // q0=(4/PI)*q0 - q0*abs(q0)*4/(PI*PI)
//					 "vabs.f32 q1,q0\n\t"
//					 "vmul.f32 q1,q0,q1\n\t"
//					 "vmul.f32 q0,q0,q3\n\t"
//					 "vmul.f32 q1,q1,q4\n\t"
//					 "vadd.f32 q0,q0,q1\n\t"
//					 // q0+=.225*(q0*abs(q0) - q0)
//					 "vabs.f32 q1,q0\n\t"
//					 "vmul.f32 q1,q0,q1\n\t"
//					 "vsub.f32 q1,q0\n\t"
//					 "vmla.f32 q0,q1,q6\n\t"
//					 "vstmia %0, { q0 }\n\t"
//					 :: "r"(results), "r"(angles), "r"(constants)
//					 : "memory","cc","q0","q1","q2","q3","q4","q5","q6"
//					 );  
//	}
	/*
	  // Sin/Cos experiments
	 float32 y1 = 0.0f;
	 float32 y2 = 0.0f;
	float32 maxError = 0.0f;
	
	for (int32 p = 0; p < 50000; ++p)
	{
		float32 error = sinf((float32)p) - FastSin((float32)p);
		if (error > maxError)
			maxError = error;
	}
	Logger::Debug("Fast sin accuracy: %f %%", maxError);  
	
	uint64 t1 = SystemTimer::Instance()->AbsoluteMS();
	float32 angle = 0.0f;
	for (int32 p = 0; p < 500000; ++p)
	{
		float angles[2] = {angle, angle};
		float results[4];
		vsincos(angles, results);
		y1 += results[0];
		angle += 0.0000001f;
	}
	
	uint64 t2 = SystemTimer::Instance()->AbsoluteMS();
	
	uint64 xt1 = SystemTimer::Instance()->AbsoluteMS();
	angle = 0.0f;
	for (int32 p = 0; p < 500000; ++p)
	{
		y2 += sinf(angle);
		angle += 0.0000001f;
	}
	uint64 xt2 = SystemTimer::Instance()->AbsoluteMS();
	
	Logger::Debug("sin default:%lld sin fast: %lld, %f %f, %f", t2 - t1, xt2 - xt1, y2 - y1, y2, y1);
	*/
};