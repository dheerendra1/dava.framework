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
#include "Animation/AnimatedObject.h"
#include "Animation/AnimationManager.h"

namespace DAVA 
{

AnimatedObject::AnimatedObject()
//	: animationsStorage(0)
{
	
}

AnimatedObject::~AnimatedObject()
{
	StopAnimations();
}
	
void AnimatedObject::StopAnimations(int32 track)
{
	AnimationManager::Instance()->DeleteAnimations(this, track);
}

bool AnimatedObject::IsAnimating(int32 track)
{
	return AnimationManager::Instance()->IsAnimating(this, track);
}

Animation * AnimatedObject::FindPlayingAnimation(int32 track /*= -1*/)
{
	return AnimationManager::Instance()->FindPlayingAnimation(this, track);
}

}

/*
void AnimatedObject::AddAnimation(Animation * animation)
{
	
}
void RemoveAnimation(Animation * animation);
{
	
}*/
/*virtual Animation * FindNextAnimationForTrack(int track)
{

}

void Animation::Update()
{
	for (int k = 0; k < MAX_NUMBER_OF_TRACKS; ++k)
	{
		
		
	}
}*/