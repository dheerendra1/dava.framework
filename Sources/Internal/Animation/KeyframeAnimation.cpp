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
#include "FileSystem/Logger.h"
#include "Animation/KeyframeAnimation.h"

namespace DAVA
{
	
	
KeyframeData::KeyframeData()
{
	head = 0;
	tail = 0;
}

KeyframeData::~KeyframeData()
{
	Keyframe * node = head;
	while(node)
	{
		Keyframe * next = node->next;
		delete node;
		node = next;
	}
}

void KeyframeData::AddKeyframe(int frame, float32 time)
{
	Keyframe * keyFrame = new Keyframe(frame, time);
	if (head == 0)
	{
		head = tail = keyFrame;
	}
	else 
	{
		tail->next = keyFrame;
	}
}
	
float32 KeyframeData::GetLength()
{
	return tail->time;
}

	

KeyframeAnimation::KeyframeAnimation(AnimatedObject * _owner, int * _var, KeyframeData * _data, float32 _animationTimeLength)
	: Animation(_owner, _animationTimeLength, Interpolation::LINEAR)
{
	var = _var;
	data = _data;
	data->Retain();
}
	
KeyframeAnimation::~KeyframeAnimation()
{
	SafeRelease(data);
}


void KeyframeAnimation::OnStart()
{
	currentFrame = data->head;
}

void KeyframeAnimation::Update(float32 timeElapsed)
{
	//Animation::Update(timeElapsed);
	if (state & STATE_IN_PROGRESS)
	{
		time += timeElapsed;
		if (time >= timeLength)
		{
			if (repeatCount == 0)
			{
				time = timeLength;
				normalizedTime = 1.0f;
				state |= STATE_FINISHED;
			}else 
			{
				time -= timeLength;
				repeatCount--;
			}
		}
		
		if (currentFrame->next == 0) // we've found last keyframe stop immediatelly
		{
			state |= STATE_FINISHED;
			return;
		}
		
		while (time >= currentFrame->next->time)
		{
			currentFrame = currentFrame->next;
			if (currentFrame->next == 0)
			{
				state |= STATE_FINISHED;
				return;
			}
		}
		
		// check isn't necessary
		if (currentFrame && currentFrame->next)
		{
			float32 frameDelta = time - currentFrame->time;
			float32 duration = currentFrame->next->time - currentFrame->time;
			float32 frame = currentFrame->frame + (currentFrame->next->frame - currentFrame->frame) * frameDelta / duration;
			*var = (int)frame;
			//Logger::Debug("%0.2f", frame);
		}
	}
	//*var = startValue + (endValue - startValue) * normalizedTime;
}
	

};