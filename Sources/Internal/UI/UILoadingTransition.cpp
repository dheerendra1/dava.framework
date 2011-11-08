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

#include "UI/UILoadingTransition.h"
#include "Render/RenderManager.h"
#include "Platform/SystemTimer.h"
#include "UI/UIControlSystem.h"
#include "Debug/Replay.h"

namespace DAVA 
{

	
UILoadingTransition::UILoadingTransition()
{
	thread = 0;
	backgroundSprite = 0;
	animationSprite = 0;
	inTransition = 0;
	outTransition = 0;
}

UILoadingTransition::~UILoadingTransition()
{
	SafeRelease(inTransition);
	SafeRelease(outTransition);
	SafeRelease(backgroundSprite);
	SafeRelease(animationSprite);
}
	
// Setup of default loading screen
void UILoadingTransition::SetInTransition(UIScreenTransition * transition)
{
	SafeRelease(inTransition);
	inTransition = transition;
	if (inTransition)inTransition->Retain();
}

void UILoadingTransition::SetOutTransition(UIScreenTransition * transition)
{
	SafeRelease(outTransition);
	outTransition = transition;
	if (outTransition)outTransition->Retain();
}

void UILoadingTransition::SetBackgroundSprite(Sprite * sprite)
{
	SafeRelease(backgroundSprite);
	backgroundSprite = sprite;
	if (backgroundSprite)backgroundSprite->Retain();
}

void UILoadingTransition::SetAnimationSprite(Sprite * _animationSprite)
{
	SafeRelease(animationSprite);
	animationSprite = _animationSprite;
	if (animationSprite)animationSprite->Retain();
}

void UILoadingTransition::SetAnimationDuration(float32 durationInSeconds)
{
	animationDuration = durationInSeconds;
}
	
void UILoadingTransition::StartTransition(UIScreen * _prevScreen, UIScreen * _nextScreen)
{
	prevScreen = _prevScreen;
	nextScreen = _nextScreen;
	thread = 0;
	animationTime = 0;
}	

int32 UILoadingTransition::GetGroupId()
{
    if (!nextScreen || !thread) 
    {
        return UIScreen::GetGroupId();
    }
	return nextScreen->GetGroupId();
}
	
void UILoadingTransition::ThreadMessage(BaseObject * obj, void * userData, void *callerData)
{
	//RenderManager::Instance()->
	if (prevScreen->GetGroupId() != nextScreen->GetGroupId())
    {
		nextScreen->LoadGroup();
    }
}

void UILoadingTransition::DidAppear()
{
	if (!thread)
	{
		thread = Thread::Create(Message(this, &UILoadingTransition::ThreadMessage));
		thread->EnableCopyContext();
		thread->Start();
	}
}

void UILoadingTransition::Update(float32 timeElapsed)
{
	if ((thread) && (thread->GetState() == Thread::STATE_ENDED))
	{
		UIControlSystem::Instance()->SetScreen(nextScreen, outTransition);
        if (!inTransition) 
        {
            UIControlSystem:: Instance()->UnlockInput();//need to call this because once its calls on loading start
        }
		SafeRelease(thread);
	}
	float32 elapsedTime = SystemTimer::FrameDelta();
	animationTime += elapsedTime; 
	if (animationSprite)
	{
		if (animationTime > animationDuration)
			animationTime -= animationDuration;
	}
}

void UILoadingTransition::Draw(const UIGeometricData &geometricData)
{
	if (backgroundSprite)
		backgroundSprite->Draw();

	if (animationSprite)
	{
		int frame = (int32)((animationTime/animationDuration) * animationSprite->GetFrameCount());
		if (frame > animationSprite->GetFrameCount() - 1)
			frame = animationSprite->GetFrameCount() - 1;
		
		animationSprite->SetFrame(frame);
		animationSprite->Draw();
	}
}
	
bool UILoadingTransition::IsLoadingTransition()
{
	return true;
}

void UILoadingTransition::SetDuration(float32 timeInSeconds)
{
}

void UILoadingTransition::WillAppear()
{
	if(Replay::IsRecord() || Replay::IsPlayback())
	{
		Replay::Instance()->PauseReplay(true);
	}
}

void UILoadingTransition::WillDisappear()
{
	if(Replay::Instance())
	{
		Replay::Instance()->PauseReplay(false);
		SystemTimer::Instance()->SetFrameDelta(0.33f);//TODO: this is temporary solution for "first frame after loading" issue
	}
}


};

