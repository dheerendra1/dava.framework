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
#ifndef __DAVAENGINE_SCENE_NODE_ANIMATION_LIST_H__
#define __DAVAENGINE_SCENE_NODE_ANIMATION_LIST_H__

#include "Scene3D/SceneNode.h"
#include "Scene3D/SceneNodeAnimation.h"
#include "Base/EventDispatcher.h"

namespace DAVA 
{
	
class SceneNodeAnimationList : public SceneNode
{
public:
	enum
	{
		ANIMATION_ENDED = 1,
	};
	
	
	SceneNodeAnimationList(Scene * _scene);
	virtual ~SceneNodeAnimationList();

	void AddAnimation(SceneNodeAnimation * node);
	
	// stop animation at the current moment
	void StopAnimation();
	
	// cycle animation
	void CycleAnimation();
	void Execute(float32 fadeInTime = 0.0f, float32 fadeOutTime = 0.0f, float32 timeFactor = 1.0f);
	void BlendTo(SceneNodeAnimationList * next, float32 blendTime, float32 timeFactor = 1.0f);

	SceneNodeAnimation * GetNode(const String & name);
		
	void Update(float32 timeElapsed);
	
	float32 GetDuration();
	float32 GetCurrentTime();
	
	void SetWeight(float32 weight);
	

	
//private:	
	bool	isDestination;
	bool	active;
	bool	cycled;
	float32 fadeInTime;
	float32 fadeOutTime;
	float32 timeFactor;
	float32 currentTime;
	float32 duration;							// in seconds
	SceneNodeAnimationList * blendTo;
	float32 blendTime;
	Vector<SceneNodeAnimation*> animations;		// animations for all nodes
	
	
	IMPLEMENT_EVENT_DISPATCHER(eventDispatcher);
};
	
	

};

#endif // __DAVAENGINE_SCENE_NODE_ANIMATION_LIST_H__
