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

#include "UI/UIScreenTransition.h"
#include "Render/RenderManager.h"
#include "Render/RenderHelper.h"
#include "Platform/SystemTimer.h"
#include "UI/UIControlSystem.h"

namespace DAVA 
{

	
Sprite * UIScreenTransition::renderTargetPrevScreen = 0;	
Sprite * UIScreenTransition::renderTargetNextScreen = 0;	
	
UIScreenTransition::UIScreenTransition()
{
	duration = 0.7f;
	interpolationFunc = Interpolation::GetFunction(Interpolation::EASY_IN_EASY_OUT);
	SetFillBorderOrder(UIScreen::FILL_BORDER_NONE);
}

UIScreenTransition::~UIScreenTransition()
{
}
	
void UIScreenTransition::CreateRenderTargets()
{
	if (renderTargetPrevScreen || renderTargetNextScreen)
	{
		Logger::Debug("Render targets already created");
		return;
	}
	
	renderTargetPrevScreen = Sprite::CreateAsRenderTarget(
		(Core::Instance()->GetVirtualScreenXMax() - Core::Instance()->GetVirtualScreenXMin()), 
		(Core::Instance()->GetVirtualScreenYMax() - Core::Instance()->GetVirtualScreenYMin()), Texture::FORMAT_RGB565);
	renderTargetPrevScreen->SetDefaultPivotPoint(-Core::Instance()->GetVirtualScreenXMin(), -Core::Instance()->GetVirtualScreenYMin());
	
	renderTargetNextScreen = Sprite::CreateAsRenderTarget(
		(Core::Instance()->GetVirtualScreenXMax() - Core::Instance()->GetVirtualScreenXMin()), 
		(Core::Instance()->GetVirtualScreenYMax() - Core::Instance()->GetVirtualScreenYMin()), Texture::FORMAT_RGB565);
	renderTargetNextScreen->SetDefaultPivotPoint(-Core::Instance()->GetVirtualScreenXMin(), -Core::Instance()->GetVirtualScreenYMin());
}

void UIScreenTransition::ReleaseRenderTargets()
{
	SafeRelease(renderTargetPrevScreen);
	SafeRelease(renderTargetNextScreen);
}
	
void UIScreenTransition::StartTransition(UIScreen * _prevScreen, UIScreen * _nextScreen)
{
	CreateRenderTargets();
	nextScreen = _nextScreen;
	prevScreen = _prevScreen;
	
	Rect screenRect = Rect(Core::Instance()->GetVirtualScreenXMin()
						   , Core::Instance()->GetVirtualScreenYMin()
						   , Core::Instance()->GetVirtualScreenXMax() - Core::Instance()->GetVirtualScreenXMin()
						   , Core::Instance()->GetVirtualScreenYMax() - Core::Instance()->GetVirtualScreenYMin());
	
	RenderManager::Instance()->SetRenderTarget(renderTargetPrevScreen);
	RenderManager::Instance()->SetVirtualViewOffset();
	RenderManager::Instance()->SetColor(0.0f, 0.0f, 0.0f, 1.0f);
	RenderHelper::Instance()->FillRect(screenRect);
	RenderManager::Instance()->ResetColor(); //SetColor(1.0f, 1.0f, 1.0f, 1.0f);

//	int32 prevScreenGroupId = 0;
	if (prevScreen)
	{
		prevScreen->SystemDraw(UIControlSystem::Instance()->GetBaseGeometricData());
		
		prevScreen->SystemWillDisappear();
		// prevScreen->UnloadResources();
		if (prevScreen->GetGroupId() != nextScreen->GetGroupId())
			prevScreen->UnloadGroup();
		prevScreen->SystemDidDisappear();
		
		SafeRelease(prevScreen);
	}
	RenderManager::Instance()->RestoreRenderTarget();

	nextScreen->LoadGroup();
	
	nextScreen->SystemWillAppear();
	
	//
	
	RenderManager::Instance()->SetRenderTarget(renderTargetNextScreen);
	RenderManager::Instance()->SetVirtualViewOffset();
	
	RenderManager::Instance()->SetColor(0.0f, 0.0f, 0.0f, 1.0f);
	RenderHelper::Instance()->FillRect(screenRect);
	RenderManager::Instance()->ResetColor();//SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	float32 timeElapsed = SystemTimer::FrameDelta();
	nextScreen->SystemUpdate(timeElapsed);
	nextScreen->SystemDraw(UIControlSystem::Instance()->GetBaseGeometricData());
	
	RenderManager::Instance()->RestoreRenderTarget();
	currentTime = 0;
}
	
void UIScreenTransition::Update(float32 timeElapsed)
{
	currentTime += timeElapsed;
	normalizedTime = interpolationFunc(currentTime / duration);
	if (currentTime >= duration)
	{
		currentTime = duration;
		nextScreen->SystemDidAppear();
		ReleaseRenderTargets();
		// go to next screen
		UIControlSystem::Instance()->ReplaceScreen(nextScreen);
		UIControlSystem::Instance()->UnlockInput();
		
		/*
			Right now we are in update so when we change control we miss Update for new screen
			Here we call update control to make calls to update / draw sequential and avoid problem with missing Update
			We pass current timeElapsed because we miss current frame time
		 */
		nextScreen->SystemUpdate(timeElapsed); // 
	}
}

void UIScreenTransition::Draw(const UIGeometricData &geometricData)
{
	renderTargetPrevScreen->SetScale(0.5f, 1.0f);
	renderTargetPrevScreen->SetPosition(0, 0);
	renderTargetPrevScreen->Draw();

	renderTargetNextScreen->SetScale(0.5f, 1.0f);
	renderTargetNextScreen->SetPosition(240, 0);
	renderTargetNextScreen->Draw();
}
	
void UIScreenTransition::SetDuration(float32 timeInSeconds)
{
	duration = timeInSeconds;
};	

bool UIScreenTransition::IsLoadingTransition()
{
	return false;
}
	
};

