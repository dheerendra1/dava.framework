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
#include "Scene3D/SceneNodeAnimation.h"
#include "Scene3D/SceneNodeAnimationList.h"

namespace DAVA 
{


SceneNodeAnimation::SceneNodeAnimation(int32 _keyCount)
{
	keyCount = _keyCount;
	bindNode = 0;
	startIdx = 0;
	keys = new SceneNodeAnimationKey[keyCount];
	apply = true;
	weight = 0.0f;
	delayTime = 0.0f;
	parent = 0;
}

SceneNodeAnimation::~SceneNodeAnimation()
{
	SafeDeleteArray(keys);
}
	
void SceneNodeAnimation::SetKey(int32 index, const SceneNodeAnimationKey & key)
{
	keys[index] = key;
}

SceneNodeAnimationKey & SceneNodeAnimation::Intepolate(float32 t)
{
	if (keyCount == 1)
	{
		currentValue = keys[0];
		return currentValue;
	}
	
	if (t < keys[startIdx].time)
	{
		startIdx = 0;
	}
	
	int32 endIdx = 0;
	for (endIdx = startIdx; endIdx < keyCount; ++endIdx)
	{
		if (keys[endIdx].time > t)
		{
			break;
		}
		startIdx = endIdx;
	}
	
	if (endIdx == keyCount)
	{
		currentValue = keys[keyCount - 1];
		return currentValue;
	}
	
	SceneNodeAnimationKey & key1 = keys[startIdx];
	SceneNodeAnimationKey & key2 = keys[endIdx];

	float32 tInter = (t - key1.time) / (key2.time - key1.time);

	currentValue.translation.Lerp(key1.translation, key2.translation, tInter);
	currentValue.rotation.Slerp(key1.rotation, key2.rotation, tInter);
	//currentValue.matrix = key1.matrix;
	return currentValue;
}

void SceneNodeAnimation::SetDuration(float32 _duration)
{
	duration = _duration;
}
	
void SceneNodeAnimation::SetBindNode(SceneNode * _bindNode)
{
	bindNode = _bindNode;
}
	
void SceneNodeAnimation::SetBindName(const String & _bindName)
{
	bindName = _bindName;
}
	
void SceneNodeAnimation::Update(float32 timeElapsed)
{
	delayTime -= timeElapsed;
	if (delayTime <= 0.0f)
	{
		delayTime = 0.0f;
		currentTime += timeElapsed + delayTime;
		if (currentTime > duration)
		{
			currentTime = duration;
			//bindNode->DetachAnimation(this);
		}
	}
}
	
void SceneNodeAnimation::Execute()
{
	startIdx = 0;
	currentTime = 0;
	bindNode->ExecuteAnimation(this);
}
	
Vector3 SceneNodeAnimation::SetStartPosition(const Vector3 & position)
{
	Vector3 sPos = keys[0].translation;
	for (int idx = 0; idx < keyCount; ++idx)
	{
		keys[idx].translation = position + keys[idx].translation - sPos;
	}
	return position - sPos;
}	

void SceneNodeAnimation::ShiftStartPosition(const Vector3 & shift)
{
	for (int idx = 0; idx < keyCount; ++idx)
	{
		keys[idx].translation += shift;
	}
}
	
void SceneNodeAnimation::SetParent(SceneNodeAnimationList * list)
{
	parent = list;
}

SceneNodeAnimationList * SceneNodeAnimation::GetParent()
{
	return parent;
}

}