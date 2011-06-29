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
#include "Animation/Animation.h"
#include "Animation/AnimationManager.h"
#include "FileSystem/Logger.h"

namespace DAVA 
{

Animation::Animation(AnimatedObject * _owner, float32 _animationTimeLength, Interpolation::FuncType _interpolationFunc, int _defaultState)
{
	owner = _owner;
	timeLength = _animationTimeLength;
	interpolationFunc = Interpolation::GetFunction(_interpolationFunc);
	state = _defaultState;
	next = 0;
	repeatCount = 0;
	AnimationManager::Instance()->AddAnimation(this);
}

Animation::~Animation()
{
	AnimationManager::Instance()->RemoveAnimation(this);	
}

void Animation::Reset()
{
	time = 0.0f;
	normalizedTime = 0.0f;
	next = 0;
}

void Animation::Start(int _groupId)
{
//#ifdef ANIMATIONS_DEBUG
//	if(AnimationManager::Instance()->IsAnimationLoggerEnabled())
//	{
//		Logger::Debug("ANIMATION LOGGER: Animation::Start 0x%x    for owner 0x%x", (int)this, (int)owner);
//	}
//#endif
	//Logger::Debug("Animation started: %d", _groupId);
	Reset();
	groupId = _groupId;
	
	Animation * prevAnimation = AnimationManager::Instance()->FindLastAnimation(owner, groupId);
	
	if (!prevAnimation || (prevAnimation == this))
	{
		//Logger::Debug("real anim start");
		state |= STATE_IN_PROGRESS;
		OnStart();
	}
	else
	{	
		//Logger::Debug("add to queue");
//#ifdef ANIMATIONS_DEBUG
//		if(AnimationManager::Instance()->IsAnimationLoggerEnabled())
//		{
//			Logger::Debug("ANIMATION LOGGER: Animation::Set animation 0x%x as next for 0x%x   for owner 0x%x", (int)this, (int)prevAnimation, (int)owner);
//		}
//#endif
		prevAnimation->next = this;
	}
}

void Animation::Stop()
{
	//Logger::Debug("Animation stopped: %d", groupId);
	state &= ~STATE_IN_PROGRESS; 
	state |= STATE_DELETE_ME;
	OnStop();
}
	
bool Animation::IsPlaying()
{
	return (state & STATE_IN_PROGRESS);
}
	
void Animation::Update(float32 timeElapsed)
{
	if (state & STATE_IN_PROGRESS)
	{
		if (state & STATE_REVERSE)
		{
			time += timeElapsed;
			
			float halfTimeLength = 0.5f * timeLength;
			if (time <= halfTimeLength)
			{	// normal interpolation
				normalizedTime = interpolationFunc(time / halfTimeLength);
			}else
			{	// reverse interpolation
				normalizedTime = interpolationFunc(2.0f - (time / halfTimeLength));/*1.0f - ((time - halfTimeLength) / halfTimeLength)*/
			}
			
			if (time >= timeLength)
			{
				if (repeatCount == 0)
				{
					time = timeLength;
					normalizedTime = 0.0f;
					state |= STATE_FINISHED;
				}else
				{
					time -= timeLength;
					repeatCount--;
				}
			}
		}else // 
		{
			time += timeElapsed;
			normalizedTime = interpolationFunc(time / timeLength);
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
		}
	}
}

void Animation::OnStart()
{
//#ifdef ANIMATIONS_DEBUG
//	if(AnimationManager::Instance()->IsAnimationLoggerEnabled())
//	{
//		Logger::Debug("ANIMATION LOGGER: Animation::OnStart 0x%x    for owner 0x%x", (int)this, (int)owner);
//	}
//#endif
	PerformEvent(EVENT_ANIMATION_START);
};
	
void Animation::OnStop()
{
//#ifdef ANIMATIONS_DEBUG
//	if(AnimationManager::Instance()->IsAnimationLoggerEnabled())
//	{
//		Logger::Debug("ANIMATION LOGGER: Animation::OnStop 0x%x    for owner 0x%x", (int)this, (int)owner);
//	}
//#endif
	PerformEvent(EVENT_ANIMATION_END);
//#ifdef ANIMATIONS_DEBUG
//	if(AnimationManager::Instance()->IsAnimationLoggerEnabled())
//	{
//		Logger::Debug("ANIMATION LOGGER: Animation::OnStop DONE 0x%x    for owner 0x%x", (int)this, (int)owner);
//	}
//#endif
};

void Animation::OnCancel()
{
	PerformEvent(EVENT_ANIMATION_CANCELLED);
}

void Animation::Pause(bool _isPaused)
{
	if(_isPaused)
	{
		state |= STATE_PAUSED;
	}
	else
	{
		state &= ~STATE_PAUSED;
	}
}

bool Animation::IsPaused()
{
	return (0 != (state & STATE_PAUSED));
}

}