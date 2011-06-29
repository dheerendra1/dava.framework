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
#ifndef __DAVAENGINE_SCENE_NODE_ANIMATION_H__
#define __DAVAENGINE_SCENE_NODE_ANIMATION_H__

#include "Base/BaseTypes.h"
#include "Scene3D/SceneNode.h"
#include "Scene3D/SceneNodeAnimationKey.h"
namespace DAVA 
{
	
	
/*
	TODO: efficient pack & unpack of animations (Vector / Quaternion spline approximation)
 */
class SceneNodeAnimationList;
class SceneNodeAnimation : public BaseObject
{
public:
	SceneNodeAnimation(int32 keyCount);
	virtual ~SceneNodeAnimation();
	
	SceneNodeAnimationKey & Intepolate(float32 t);
	
	void SetKey(int32 index, const SceneNodeAnimationKey & key);
	
	inline int32 GetKeyCount();
	inline SceneNodeAnimationKey * GetKeys();
	
	void SetDuration(float32 _duration);
	inline float32 GetDuration(); 
		
	void SetBindName(const String & bindName); 
	void SetBindNode(SceneNode * bindNode);
	
	virtual void Update(float32 timeElapsed);
	virtual void Execute();
	
	inline float32 GetCurrentTime();
	inline float32 GetNormalDuration();
	
	Vector3 SetStartPosition(const Vector3 & position);
	void ShiftStartPosition(const Vector3 & position);
	
	
	// this is node of animation this animation is supposed for
	SceneNode * bindNode;
	String bindName;
	bool apply;
	float32 weight;

	float32 delayTime;
	float32 currentTime;
	float32 duration;
	
	int32 startIdx;
	
	int32 keyCount;
	SceneNodeAnimationKey * keys;
	SceneNodeAnimationKey currentValue;

	void SetParent(SceneNodeAnimationList * list);
	SceneNodeAnimationList * GetParent();
	
private:
	SceneNodeAnimationList * parent;
};
	
inline float32 SceneNodeAnimation::GetCurrentTime()	
{
	return currentTime;
}
	
inline float32 SceneNodeAnimation::GetDuration()
{
	return duration;
}
	
inline float32 SceneNodeAnimation::GetNormalDuration()
{
	if (keyCount == 0)
		return 0;
	
	return keys[keyCount-1].time;
}
	
inline int32 SceneNodeAnimation::GetKeyCount()
{
	return keyCount;
}

inline SceneNodeAnimationKey * SceneNodeAnimation::GetKeys()
{
	return keys;
}
	
};

#endif // __DAVAENGINE_SCENE_NODE_ANIMATION_KEY_H__
