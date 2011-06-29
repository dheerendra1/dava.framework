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
        * Created by Alexey 'Hottych' Prosin
=====================================================================================*/

#ifndef __DAVAENGINE_SCROLL_HELPER__
#define __DAVAENGINE_SCROLL_HELPER__

#include "UI/ScrollHelper.h"
#include "Base/BaseObject.h"
#include "Base/BaseTypes.h"

namespace DAVA 
{
	
	/**
	 \ingroup controlsystem
	 \brief Scrolling helper class.
	 Helps to work with scrolable objects. Responsable for inertia and other scrolling effects.
	 */
	
class ScrollHelper : public BaseObject 
{
	class MovesDelta
	{
	public:
		float deltaMove;
		float deltaTime;
	};
public:
	
	/**
	 \brief Constructor.
	 */
	ScrollHelper();
	
	/**
	 \brief Sets new scrolling position.
	 \param[in] pos new scrolling position.
	 */
	void SetPosition(float32 pos);
	/**
	 \brief Sets scrollable element size.
	 \param[in] newSize scrollable element size.
	 */
	void SetElementSize(float32 newSize);
	/**
	 \brief Sets view area size.
	 \param[in] size size of the view area.
	 */
	void SetViewSize(float32 size);
	/**
	 \brief Adding move impulse. After the impulse scrolling position moves
		to the impulse direction for the period of time.
	 \param[in] impulseSpeed impulse value.
	 */
	void Impulse(float impulseSpeed);
	
	/**
	 \brief Returns scroll position for the current scroll helper states.
	 \returns Current scroll position.
	 */
	float32 GetPosition();
    
    float32 GetViewSize();
    float32 GetElementSize();
	
	
	/**
	 \brief Returns scroll position accordingly to the new incoming data.
	 \param[in] positionDelta position changing from the last update.
	 \param[in] timeDelta time in seconds from the last position update.
	 \param[in] isPositionLocked send true if position is changing now.
	 \returns new scroll position.
	 */
	float GetPosition(float positionDelta, float timeDelta, bool isPositionLocked);
	
	/**
	 \brief Sets how fast scroll speed will be reduced
		(for example 0.25 reduces speed to zero for the 0.25 second ). 
		To remove inertia effect set tihs value to 0.
		Used 0.25 by default.
	 \param[in] newValue value of the slow down time in seconds.
	 */
	void SetSlowDownTime(float newValue);
	/**
	 \brief Sets the scrolling element behavior at the scroll borders.
		To remove movement effect after borders are reached set this value to 0.
		0.5 by default.
	 \param[in] newValue value of border movement length.
	 */
	void SetBorderMoveModifer(float newValue);
	
private:
	float position;
	float elementSize;
	float viewSize;
	float virtualViewSize;
	
	float totalDeltaTime;
	float totalDeltaMove;
	
	float slowDown;
	float backward;
	
	float speed;
	
	List<MovesDelta> moves;
};
};

#endif