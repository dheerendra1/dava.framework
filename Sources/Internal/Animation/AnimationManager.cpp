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
#include "Animation/AnimationManager.h"
#include "FileSystem/Logger.h"

#if !defined (__DAVAENGINE_ANDROID__)
#include <typeinfo>
#endif //#if !defined (__DAVAENGINE_ANDROID__)

namespace DAVA
{
		
AnimationManager::AnimationManager()
{
#ifdef ANIMATIONS_DEBUG	
	animationLoggerEnabled = false;
#endif
}
	
AnimationManager::~AnimationManager()
{
	
}
	
void AnimationManager::SetAnimationLoggerEnabled(bool isEnabled)
{
#ifdef ANIMATIONS_DEBUG
	animationLoggerEnabled = isEnabled;
#endif
}
bool AnimationManager::IsAnimationLoggerEnabled()
{
#ifdef ANIMATIONS_DEBUG
	return animationLoggerEnabled;
#endif
	return false;
}


void AnimationManager::AddAnimation(Animation * animation)
{
	animations.push_back(animation);
//#ifdef ANIMATIONS_DEBUG
//	if(animationLoggerEnabled)
//	{
//		Logger::Debug("ANIMATION LOGGER: AnimationManager::AddAnimation 0x%x   new animations size %d", (int)animation, animations.size());
//	}
//#endif
}

void AnimationManager::RemoveAnimation(Animation * animation)
{
	//Debug::Log("");
	//Logger::Debug("RemoveAnimation: before animations: %d\n", animations.size());
	//std::remo ve(animations.begin(), animations.end(), animation);
	for (Vector<Animation*>::iterator t = animations.begin(); t != animations.end(); ++t)
	{
		if (*t == animation)
		{
			animations.erase(t);
//#ifdef ANIMATIONS_DEBUG
//			if(animationLoggerEnabled)
//			{
//				Logger::Debug("ANIMATION LOGGER: AnimationManager::RemoveAnimation 0x%x   new animations size %d", (int)animation, animations.size());
//			}
//#endif
			break;
		}

	}
	//Logger::Debug("RemoveAnimation: after animations: %d\n", animations.size());
	
}
    
void AnimationManager::StopAnimations()
{
    for (Vector<Animation*>::iterator t = animations.begin(); t != animations.end(); ++t)
	{
		Animation * animation = *t;
		
        animation->owner = 0;   // zero owner to avoid any issues (it was a problem with DumpState, when animations was deleted before). 
        animation->state &= ~Animation::STATE_IN_PROGRESS;
        animation->state &= ~Animation::STATE_FINISHED;
        animation->state |= Animation::STATE_DELETE_ME;
	}	
}
	
void AnimationManager::DeleteAnimations(AnimatedObject * _owner, int32 track)
{
	for (Vector<Animation*>::iterator t = animations.begin(); t != animations.end(); ++t)
	{
		Animation * animation = *t;
		if ((track != -1) && (animation->groupId != track))continue;
		
		if (animation->owner == _owner)
		{
//#ifdef ANIMATIONS_DEBUG
//			if(animationLoggerEnabled)
//			{
//				Logger::Debug("ANIMATION LOGGER: AnimationManager::Set  DELETE_ME 0x%x    for owner 0x%x", (int)animation, (int)_owner);
//			}
//#endif
            animation->owner = 0;   // zero owner to avoid any issues (it was a problem with DumpState, when animations was deleted before). 
			animation->state &= ~Animation::STATE_IN_PROGRESS;
			animation->state &= ~Animation::STATE_FINISHED;
			animation->state |= Animation::STATE_DELETE_ME;
		}
	}	
}
	
Animation * AnimationManager::FindLastAnimation(AnimatedObject * _owner, int32 _groupId)
{
	for (Vector<Animation*>::iterator t = animations.begin(); t != animations.end(); ++t)
	{
		Animation * animation = *t;
		if ((animation->owner == _owner) && (animation->groupId == _groupId))
		{
			while(animation->next != 0)
			{
				animation = animation->next;
			}
			return animation; // return latest animation in current group
		}
	}
	return 0;
}

bool AnimationManager::IsAnimating(AnimatedObject * owner, int32 track)
{
	for (Vector<Animation*>::iterator t = animations.begin(); t != animations.end(); ++t)
	{
		Animation * animation = *t;
		if ((track != -1) && (animation->groupId != track))continue;
		
		if ((animation->owner == owner) && (animation->state & Animation::STATE_IN_PROGRESS))
		{
			return true;
		}
	}	
	return false;
}

Animation * AnimationManager::FindPlayingAnimation(AnimatedObject * owner, int32 _groupId)
{
	for (Vector<Animation*>::iterator t = animations.begin(); t != animations.end(); ++t)
	{
		Animation * animation = *t;
		if ((_groupId != -1) && (animation->groupId != _groupId))continue;

		if ((animation->owner == owner) && (animation->state & Animation::STATE_IN_PROGRESS))
		{
			return animation;
		}
	}	
	return 0;
}

void AnimationManager::Update(float timeElapsed)
{
	//int animationsCount = (int)animations.size();
	//NSLog(@"animations: %d\n", animations.size());
	
	// update animations first
	for (int k = 0; k < (int)animations.size(); ++k)
	{
		Animation * animation = animations[k];
		if (animation->state & Animation::STATE_IN_PROGRESS)
		{
			if(!(animation->state & Animation::STATE_PAUSED))
			{
				animation->Update(timeElapsed);
			}	
		}
	}
	
	// process all finish callbacks
	for (int k = 0; k < (int)animations.size(); ++k)
	{
		Animation * animation = animations[k];
		if (animation->state & Animation::STATE_FINISHED)
		{
			//#ifdef ANIMATIONS_DEBUG
			//			if(animationLoggerEnabled)
			//			{
			//				Logger::Debug("ANIMATION LOGGER: AnimationManager::Finishing animation 0x%x    for owner 0x%x", (int)animation, (int)animation->owner);
			//				Logger::Debug("ANIMATION LOGGER: AnimationManager::Finishing index %d", k);
			//			}
			//#endif
			animation->Stop(); 
		}
	}

	//remove all old animations
	for (int k = 0; k < (int)animations.size(); ++k)
	{
		Animation * animation = animations[k];

		if (animation->state & Animation::STATE_DELETE_ME)
		{
			if (!(animation->state & Animation::STATE_FINISHED))
			{
				animation->OnCancel();
			}

//#ifdef ANIMATIONS_DEBUG
//			if(animationLoggerEnabled)
//			{
//				Logger::Debug("ANIMATION LOGGER: AnimationManager::Deleting animation 0x%x    for owner 0x%x", (int)animation, (int)animation->owner);
//				Logger::Debug("ANIMATION LOGGER: AnimationManager::Finishing index %d", k);
//			}
//#endif
			if(animation->next && !(animation->next->state  & Animation::STATE_DELETE_ME))
			{
//#ifdef ANIMATIONS_DEBUG
//				if(animationLoggerEnabled)
//				{
//					Logger::Debug("ANIMATION LOGGER: AnimationManager::Starting next animation 0x%x    for owner 0x%x", (int)animation->next, (int)animation->next->owner);
//				}
//#endif
				animation->next->state |= Animation::STATE_IN_PROGRESS;
				animation->next->OnStart();
			}
//#ifdef ANIMATIONS_DEBUG
//			if(animationLoggerEnabled)
//			{
//				for (int n = 0; n < (int)animations.size(); ++n)
//				{
//					if(animations[n]->next == animation)
//					{
//						Logger::Error("PIZDEC");
//						Logger::Debug("ANIMATION LOGGER: AnimationManager::Animation 0x%x (%d) used as next for 0x%x (%d)   for owner 0x%x", (int)animation, k, animations[n], n, (int)animation->owner);
//					}
//				}
//			}
//#endif

			SafeRelease(animation);
			k--;
//#ifdef ANIMATIONS_DEBUG
//			if(animationLoggerEnabled)
//			{
//				Logger::Debug("ANIMATION LOGGER: AnimationManager::After Deleting index %d", k);
//			}
//#endif
		}
	}
}
	
void AnimationManager::DumpState()
{
	Logger::Info("============================================================");
	Logger::Info("------------ Currently allocated animations - %2d ---------", animations.size());
	for (int k = 0; k < (int)animations.size(); ++k)
	{
		Animation * animation = animations[k];  

#if defined(__DAVAENGINE_ANDROID__)
		//TOD: VK: add typeid for android
#else //#if defined(__DAVAENGINE_ANDROID__)
        String ownerName = "no owner";
        if (animation->owner)
            ownerName = typeid(*animation->owner).name();
		Logger::Debug("addr:0x%08x state:%d class: %s ownerClass: %s", animation, animation->state, typeid(*animation).name(), ownerName.c_str());
#endif //#if defined(__DAVAENGINE_ANDROID__)

	}
	Logger::Info("============================================================");
}


void AnimationManager::PauseAnimations(bool isPaused, int tag)
{
    for(Vector<Animation*>::iterator i = animations.begin(); i != animations.end(); ++i)
    {
        Animation * &a = *i;
        
        if (a->GetTagId() == tag)
        {
            a->Pause(isPaused);
        }
    }
}

void AnimationManager::SetAnimationsMultiplier(float32 f, int tag)
{
    for(Vector<Animation*>::iterator i = animations.begin(); i != animations.end(); ++i)
    {
        Animation * &a = *i;
        
        if (a->GetTagId() == tag)
        {
            a->SetTimeMultiplier(f);
        }
    }
}

};