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
#ifndef __DAVAENGINE_LINEAR_ANIMATION_H__
#define __DAVAENGINE_LINEAR_ANIMATION_H__

#include "Base/BaseTypes.h"
#include "Base/BaseObject.h"
#include "Animation/Animation.h"
#include "Animation/AnimatedObject.h"

namespace DAVA 
{

/**
	\ingroup animationsystem
	\brief Template class to make linear animations of objects
	This class is base class to create linear animations. It can be used to animation any values that 
	supports the following operators: [+, -, *]
	This class interpolate from currentValue of the variable and finish then value is reached some endValue. 
	This class is used inside all animation functions that implemented in UIControl & GameObject. 
	
	Usage example to interpolate float32 value: 
	\code
	LinearAnimation<float32> * animation = new LinearAnimation<float32>(this, &localDrawState.angle, newAngle, time, interpolationFunc);
	animation->Start(track);
	// Be very carefull here, it's only one case in our SDK where you do not need to call SafeRelease for object you've created
	// Releasing of all animations is handled by AnimationManager itself
	\endcode
	
	Usage example to interpolate Vector2 value: 
	\code
	LinearAnimation<float32> * animation = new LinearAnimation<Vector2>(this, &localDrawState.position, newPosition, time, interpolationFunc);
	animation->Start(track);
	// Be very carefull here, it's only one case in our SDK where you do not need to call SafeRelease for object you've created
	// Releasing of all animations is handled by AnimationManager itself
	\endcode
 */
template<class T>
class LinearAnimation : public Animation
{
public:
	LinearAnimation(AnimatedObject * _owner, T * _var, T _endValue, float32 _animationTimeLength, Interpolation::FuncType _iType);
	// TODO: 
	//LinearAnimation(AnimatedObject * _owner, T * _var, T _endValue, float32 _fixedSpeed);
	
	virtual void Update(float32 timeElapsed);
	virtual void OnStart();
private:
	T * var;
	T endValue;
	T startValue;
};

template<class T>
LinearAnimation<T>::LinearAnimation(AnimatedObject * _owner, T * _var, T _endValue, float32 _animationTimeLength, Interpolation::FuncType _iType)
	: Animation(_owner, _animationTimeLength, _iType)
{
	var = _var;
	endValue = _endValue;
}
/*
template<class T>
LinearAnimation<T>::LinearAnimation(AnimatedObject * _owner, T * _var, T _endValue, float32 _fixedSpeed)
	: Animation(_owner, 0.0f, Interpolation::LINEAR)
{
	var = _var;
	endValue = _endValue;
	
}
*/

template<class T>
void LinearAnimation<T>::OnStart()
{
	startValue = *var;
}

template<class T>
void LinearAnimation<T>::Update(float32 timeElapsed)
{
	Animation::Update(timeElapsed);
	*var = startValue + (endValue - startValue) * normalizedTime;
}

template<>
inline //Dizz: inline tells compiler that symbol will be present in multiple object files
void LinearAnimation<int32>::Update(float32 timeElapsed)
{
	Animation::Update(timeElapsed);
	*var = startValue + (int32)((float32)(endValue - startValue) * normalizedTime);
}
	
//template<Rect>
	
class RectLinearAnimation : public Animation
{
public:
	RectLinearAnimation(AnimatedObject * _owner, Rect * _var, Rect _endValue, float32 _animationTimeLength, Interpolation::FuncType _iType);
	virtual void Update(float32 timeElapsed);
	virtual void OnStart();
private:
	Rect * var;
	Rect endValue;
	Rect startValue;
};


class TwoVector2LinearAnimation : public Animation
{
public:
	TwoVector2LinearAnimation(AnimatedObject * _owner, Vector2 * _var1, Vector2 _endValue1, Vector2 * _var2, Vector2 _endValue2, float32 _animationTimeLength, Interpolation::FuncType _iType);
	virtual void Update(float32 timeElapsed);
	virtual void OnStart();
private:
	Vector2 * var1;
	Vector2 endValue1;
	Vector2 startValue1;
	Vector2 * var2;
	Vector2 endValue2;
	Vector2 startValue2;
};
	

	
};
#endif // __DAVAENGINE_LINEAR_ANIMATION_H__