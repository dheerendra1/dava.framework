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
#ifndef __DAVAENGINE_INTERPOLATION_H__
#define __DAVAENGINE_INTERPOLATION_H__

#include "Base/BaseTypes.h"

namespace DAVA
{

/**
	\ingroup animationsystem
	\brief Interpolation class implements all interpolation functions supported by our animation manager.
 */
class Interpolation
{
public:
	enum FuncType
	{
		LINEAR = 0,
		EASY_IN,
		EASY_OUT,
		EASY_IN_EASY_OUT
	};
	/**
		\brief This type describes our interpolation functions. 
		It implement math equation - y = f(x) where x and y must lie in range [0; 1] 
	 */
	typedef float32 (*Func)(float32 t);
	/**
		\brief Return interpolation function by it's type
		\param[in] type function type you want to get 
		\returns function that implements this type of interpolation
	 */
	static Func GetFunction(FuncType type);
	
	/**
		\brief returns linear interpolation y = x
	 */
	static float32 Linear(float32 currentVal);
	/**
		\brief function with easy in
	 */
	static float32 EasyIn(float32 currentVal);
	/**
		\brief function with easy out
	 */
	static float32 EasyOut(float32 currentVal);
	/**
		\brief function with easy in & easy out
	 */
	static float32 EasyInEasyOut(float32 currentVal);

	static float32 Linear(float32 moveFrom, float32 moveTo, float32 startVal, float32 currentVal, float32 endVal);
	static float32 EasyIn(float32 moveFrom, float32 moveTo, float32 startVal, float32 currentVal, float32 endVal);
	static float32 EasyOut(float32 moveFrom, float32 moveTo, float32 startVal, float32 currentVal, float32 endVal);
	static float32 EasyInEasyOut(float32 moveFrom, float32 moveTo, float32 startVal, float32 currentVal, float32 endVal);
	
};
	
};

#endif // __DAVAENGINE_INTERPOLATION_H__
