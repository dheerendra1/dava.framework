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
#include "Scene3D/SceneNodeAnimationList.h"

namespace DAVA 
{
SceneNodeAnimationList::SceneNodeAnimationList(Scene * _scene)
:	SceneNode(_scene)
,	active(false)
,	cycled(false)
,	fadeInTime(0.0f)
,	fadeOutTime(0.0f)
,	timeFactor(1.0f)
,	currentTime(0.0f)
,	duration(0.0f)
,	blendTo(0)
{	
}

SceneNodeAnimationList::~SceneNodeAnimationList()
{
	for (Vector<SceneNodeAnimation*>::iterator t = animations.begin(); t != animations.end(); ++t)
	{
		SceneNodeAnimation * obj = *t;
		obj->Release();
	}
	animations.clear();
}

void SceneNodeAnimationList::AddAnimation(SceneNodeAnimation * node)
{
	animations.push_back(node);
	node->Retain();
	node->SetParent(this);
	
	float32 nodeDuration = node->GetNormalDuration();
	if (nodeDuration > duration) 
		duration = nodeDuration;
}
	
void SceneNodeAnimationList::CycleAnimation()
{
	Execute();
	cycled = true;
}


void SceneNodeAnimationList::Execute(float32 _fadeInTime, float32 _fadeOutTime, float32 _timeFactor)
{
	if (active)return; // do not reexecute active animations
	
	Logger::Debug("[SceneNodeAnimationList::Execute] %s\n", name.c_str());
	
	cycled = false;
	fadeInTime = _fadeInTime;
	fadeOutTime = _fadeOutTime;
	timeFactor = _timeFactor;
	currentTime = 0.0f;
	active = true;
	
	int32 animCount = (int32)animations.size();
	for (int32 k = 0; k < animCount; ++k)
	{
		SceneNodeAnimation * anim = animations[k];
		anim->delayTime = 0.0f;
		anim->Execute();
	}
}
	
void SceneNodeAnimationList::BlendTo(SceneNodeAnimationList * next, float32 _blendTime, float32 _timeFactor)
{
	if (active)return; // do not reexecute active animations
	
	Logger::Debug("[SceneNodeAnimationList::BlendTo] from: %s to: %s\n", name.c_str(), next->name.c_str());

	blendTo = next;
	blendTime = _blendTime;
	fadeOutTime = _blendTime;
	timeFactor = _timeFactor;
	currentTime = 0.0f;
	active = true;
	isDestination = false;
	blendTo->isDestination = true;
	
	int32 animCount = (int32)animations.size();
	for (int32 k = 0; k < animCount; ++k)
	{
		SceneNodeAnimation * anim = animations[k];
		anim->delayTime = 0.0f;
		anim->Execute();
	}

	animCount = (int32)next->animations.size();
	for (int32 k = 0; k < animCount; ++k)
	{
		SceneNodeAnimation * anim = next->animations[k];
		anim->weight = 0.0f;
		anim->delayTime = duration;
		anim->Execute();
	}
}

float32 SceneNodeAnimationList::GetDuration()
{
	return duration;
}

float32 SceneNodeAnimationList::GetCurrentTime()
{
	if (animations.size() == 0)
		return 0;
	
	return animations[0]->GetCurrentTime();
}
	
void SceneNodeAnimationList::Update(float32 timeElapsed)
{
	if (!active)return;
	
	currentTime += timeElapsed;
	
	float32 tWeight = 1.0f;
	if (currentTime < fadeInTime)
		tWeight = FloatClamp(0.0f, 1.0f, currentTime / fadeInTime);

	float32 fadeOutStart = duration - fadeOutTime;
	if (currentTime > fadeOutStart)
	{
		tWeight = FloatClamp(0.0f, 1.0f, 1.0f - ((currentTime - fadeOutStart) / fadeOutTime));
		if (blendTo)
		{
			blendTo->SetWeight(1.0f - tWeight);
		}
	}
	
	// Logger::Debug("anim: %s currentTime:%f, wei: %f", name.c_str(), currentTime, tWeight);
	
	int32 size = (int32)animations.size();
	for (int32 k = 0; k < size; ++k)
	{
		SceneNodeAnimation * anim = animations[k];
		if (!isDestination)
			anim->weight = tWeight;
		anim->Update(timeElapsed);		
	}
	if (currentTime > duration) // animation finished
	{
		if (cycled)
		{
			currentTime = 0;
			active = false;
			Execute();
			cycled = true;
		}else
		{
			if (blendTo)
			{
				blendTo->active = true;
				blendTo->StopAnimation();	// stop blendTo because user have to add it himself
				blendTo = 0;
			}
			
			this->StopAnimation();
			eventDispatcher.PerformEvent(ANIMATION_ENDED);
			return;
		}
	}
}

// stop animation at the current moment
void SceneNodeAnimationList::StopAnimation()
{
	if (!active)return;
	int32 size = (int32)animations.size();
	for (int32 k = 0; k < size; ++k)
	{
		SceneNodeAnimation * anim = animations[k];
		anim->bindNode->DetachAnimation(anim);
	}
	active = false;
	Logger::Debug("[SceneNodeAnimationList] Finished: %s\n", name.c_str());
}

void SceneNodeAnimationList::SetWeight(float32 weight)
{
	int32 size = (int32)animations.size();
	for (int32 k = 0; k < size; ++k)
	{
		SceneNodeAnimation * anim = animations[k];
		anim->weight = weight;
	}
}

SceneNodeAnimation * SceneNodeAnimationList::GetNode(const String & name)
{
	int32 size = (int32)animations.size();
	for (int32 k = 0; k < size; ++k)
	{
		SceneNodeAnimation * anim = animations[k];
		if (name == anim->bindName)
			return anim;
	}
	return 0;
}

	
};
