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

#include "UI/ScrollHelper.h"
#include "FileSystem/Logger.h"

namespace DAVA 
{
	ScrollHelper::ScrollHelper()
	:	BaseObject()
	,	position(0)
	,	elementSize(0)
	,	totalDeltaTime(0)
	,	totalDeltaMove(0)
	,   speed(0)
	{
		slowDown = 0.25f;
		backward = 0.3f;
	}

	void ScrollHelper::SetPosition(float32 pos)
	{
		position = pos;
        position = Min(position, 0.f);
        position = Max(position, -elementSize);
	}
	void ScrollHelper::SetElementSize(float32 newSize)
	{
		elementSize = newSize;
		virtualViewSize = viewSize;
		if(viewSize > elementSize)
		{
			virtualViewSize = elementSize;
		}
	}
	
	float ScrollHelper::GetPosition()
	{
		return position;
	}
	
	void ScrollHelper::SetViewSize(float32 size)
	{
		viewSize = size;
		virtualViewSize = viewSize;
		if(viewSize > elementSize)
		{
			virtualViewSize = elementSize;
		}
	}
    
    float32 ScrollHelper::GetViewSize()
    {
        return viewSize;
    }
    
    float32 ScrollHelper::GetElementSize()
    {
        return elementSize;
    }


	void ScrollHelper::SetSlowDownTime(float newValue)
	{
		slowDown = newValue;
	}
	
	void ScrollHelper::SetBorderMoveModifer(float newValue)
	{
		backward = newValue;
	}

	void ScrollHelper::Impulse(float impulseSpeed)
	{
		if((position > 0 && impulseSpeed > 0) || (position + elementSize < virtualViewSize && impulseSpeed < 0))
		{
			return;
		}
		speed = impulseSpeed;
	}

	
	float ScrollHelper::GetPosition(float positionDelta, float timeDelta, bool isPositionLocked)
	{
		if(isPositionLocked)
		{
			if(position + positionDelta > 0)
			{
				positionDelta *= (1.0f - position / virtualViewSize) * backward;
			}
			if(position + elementSize + positionDelta  < virtualViewSize)
			{
                
				positionDelta *= (1.0f - (virtualViewSize - (position + elementSize)) / virtualViewSize) * backward;
			}
			position += positionDelta;
			speed = 0;
			MovesDelta m;
			m.deltaMove = positionDelta;
			m.deltaTime = timeDelta;
			moves.push_back(m);
			totalDeltaTime += timeDelta;
			totalDeltaMove += positionDelta;
			if(totalDeltaTime >= 0.4)
			{
				List<MovesDelta>::iterator it = moves.begin();
				totalDeltaTime -= it->deltaTime;
				totalDeltaMove -= it->deltaMove;
				moves.erase(it);
			}
		}
		else
		{
			if(totalDeltaMove != 0)
			{
				speed = totalDeltaMove / totalDeltaTime;
				speed = Min(speed, virtualViewSize * 2);
				speed = Max(speed, -virtualViewSize * 2);
			}
			
			if(position > 0)
			{
				if(backward != 0 && slowDown != 0)
				{
					if(slowDown != 0)
					{
						speed -= virtualViewSize * timeDelta / slowDown / backward;
					}
					else
					{
						speed -= virtualViewSize * timeDelta * 4 / backward;
					}
					position += speed * timeDelta;
					if(position < 0)
					{
						position = 0;
						speed = 0;
					}
				}
				else
				{
					position = 0;
					speed = 0;
				}
			}
			else if(position + elementSize < virtualViewSize)
			{
				if(backward != 0)
				{
					if(slowDown != 0)
					{
						speed += virtualViewSize * timeDelta / slowDown / backward;
					}
					else
					{
						speed += virtualViewSize * timeDelta * 4 / backward;
					}
					position += speed * timeDelta;
					if(position + elementSize > virtualViewSize)
					{
						position = virtualViewSize - elementSize;
						speed = 0;
					}
				}
				else
				{
					position = virtualViewSize - elementSize;
					speed = 0;
				}
			}
			else if(speed != 0)
			{
				if(slowDown != 0)
				{
					float oldSpd = speed;
					speed = speed - speed / slowDown * timeDelta;
					if((oldSpd > 0 && speed < 1.0) || (oldSpd < 0 && speed > -1.0))
					{
						speed = 0;
					}
					position += speed * timeDelta;
				}
				else
				{
					speed = 0;
				}
			}
			
			moves.clear();
			totalDeltaTime = 0;
			totalDeltaMove = 0;
		}
		
		return position;
	}
}




