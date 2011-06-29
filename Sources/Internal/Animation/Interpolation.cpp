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
#include "Animation/Interpolation.h"

namespace DAVA
{
	
Interpolation::Func Interpolation::GetFunction(FuncType type)
{
	if (type == LINEAR)return Linear;
	else if (type == EASY_IN)return EasyIn;
	else if (type == EASY_OUT)return EasyOut;
	else if (type == EASY_IN_EASY_OUT)return EasyInEasyOut;
	return 0;
}	
	
float Interpolation::Linear(float currentVal)
{
	return FloatClamp(0.0f, 1.0f, currentVal);
}

float Interpolation::EasyIn(float currentVal)
{
	float time = currentVal;
	float time2 = time * time;
	float time3 = time2 * time;
	
	float point1X = 0.04f * 3.0f;
	float point2X = 0.17f * 3.0f;
	
	float val = time3 * (1.0f + point1X - point2X) + time2 * (point2X - 2 * point1X) + point1X * time;
	return FloatClamp(0.0f, 1.0f, val);
//	float xCoord =	point1X * 3.0f * time * (1.0f - time) * (1.0f - time) + 
//					point2X * 3.0f * time * time * (1.0f - time) + time * time * time;	
//	return xCoord;
}

float Interpolation::EasyOut(float currentVal)
{
	float time = currentVal;
	float time2 = time * time;
	float time3 = time2 * time;
	
	float point1X = 0.83f * 3.0f;
	float point2X = 0.96f * 3.0f;
	
	float val = time3 * (1.0f + point1X - point2X) + time2 * (point2X - 2 * point1X) + point1X * time;
	return FloatClamp(0.0f, 1.0f, val);
}

float Interpolation::EasyInEasyOut(float currentVal)
{
	float time = currentVal;
	float time2 = time * time;
	float time3 = time2 * time;

	
	float point1X = 0.08f * 3.0f;
	float point2X = 0.92f * 3.0f;
	
	float val = time3 * (1.0f + point1X - point2X) + time2 * (point2X - 2 * point1X) + point1X * time;
	return FloatClamp(0.0f, 1.0f, val);
}	

	//float Interpolation::EasyInEasyOut(float currentVal)
//{
//	float time = currentVal;
//	float time2 = time * time;
//	float time3 = time2 * time;
//	float time4 = time2 * time2;
//	float time5 = time4 * time;
//	float time6 = time3 * time3;
//	float time7 = time3 * time;
//	
//	float val = 0.00167284 - 0.177891 * time + 4.87144 * time2 - 25.7396 * time3 + 96.7355 * time4 - 
//	164.689 * time5 + 125.995 * time6 - 35.9986 * time7;
//	return FloatClamp(0.0f, 1.0f, val);
//}	
	

float Interpolation::Linear(float moveFrom, float moveTo, float startVal, float currentVal, float endVal)
{
	return moveFrom + (moveTo - moveFrom) * (currentVal - startVal) / (endVal - startVal);
}

float Interpolation::EasyIn(float moveFrom, float moveTo, float startVal, float currentVal, float endVal)
{
	float time = (currentVal - startVal) / (endVal - startVal);
	float point1X = moveFrom + (moveTo - moveFrom) * 0.04f;
	float point2X = moveFrom + (moveTo - moveFrom) * 0.17f;
	float xCoord = moveFrom * (1.0f - time) * (1.0f - time) * (1.0f - time) +
	point1X * 3.0f * time * (1.0f - time) * (1.0f - time) +
	point2X * 3.0f * time * time * (1.0f - time) +
	moveTo * time * time * time;
	
	return xCoord;
}

float Interpolation::EasyOut(float moveFrom, float moveTo, float startVal, float currentVal, float endVal)
{
	float time = (currentVal - startVal) / (endVal - startVal);
	float point1X = moveFrom + (moveTo - moveFrom) * 0.83f;
	float point2X = moveFrom + (moveTo - moveFrom) * 0.96f;
	float xCoord = moveFrom * (1.0f - time) * (1.0f - time) * (1.0f - time) +
	point1X * 3.0f * time * (1.0f - time) * (1.0f - time) +
	point2X * 3.0f * time * time * (1.0f - time) +
	moveTo * time * time * time;
	
	return xCoord;
}
	
float Interpolation::EasyInEasyOut(float moveFrom, float moveTo, float startVal, float currentVal, float endVal)
{
	float time = (currentVal - startVal) / (endVal - startVal);
	float point1X = moveFrom + (moveTo - moveFrom) * 0.02f;
	float point2X = moveFrom + (moveTo - moveFrom) * 0.98f;
	float xCoord = moveFrom * (1.0f - time) * (1.0f - time) * (1.0f - time) +
	point1X * 3.0f * time * (1.0f - time) * (1.0f - time) +
	point2X * 3.0f * time * time * (1.0f - time) +
	moveTo * time * time * time;
	
	return xCoord;
}

	
};
