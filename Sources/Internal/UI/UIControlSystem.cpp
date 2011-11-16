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

#include "UI/UIControlSystem.h"
#include "UI/UIScreen.h"
#include "FileSystem/Logger.h"
#include "Render/RenderManager.h"
#include "Debug/DVAssert.h"
#include "Platform/SystemTimer.h"
#include "Debug/Replay.h"

namespace DAVA 
{
UIControlSystem::~UIControlSystem()
{
	SafeRelease(currentScreen); 
	SafeRelease(popupContainer);
}
	
UIControlSystem::UIControlSystem()
{
	frameSkip = 0;
	transitionType = 0;
	
	currentScreen = 0;
	nextScreen = 0;
	prevScreen = NULL;
	removeCurrentScreen = false;
    hovered = NULL;
	//mainControl = 0;

	popupContainer = new UIControl(Rect(0, 0, 1, 1));
	popupContainer->SetInputEnabled(false);
	
	exclusiveInputLocker = NULL;
	
	lockInputCounter = 0;
	
	baseGeometricData.position = Vector2(0, 0);
	baseGeometricData.size = Vector2(0, 0);
	baseGeometricData.pivotPoint = Vector2(0, 0);
	baseGeometricData.scale = Vector2(1.0f, 1.0f);
	baseGeometricData.angle = 0;
}
	
void UIControlSystem::SetScreen(UIScreen *_nextScreen, UIScreenTransition * _transition)
{
	if (_nextScreen == currentScreen)
	{
		Logger::Warning("Tried to switch to current screen again.");
		return;
	}

	LockInput();
	transition = SafeRetain(_transition);
	
	if (_nextScreen == 0)
	{
		removeCurrentScreen = true;
	}
	
	if (nextScreen)
	{
		Logger::Warning("2 screen switches during one frame.");
	}
	nextScreen = SafeRetain(_nextScreen);
}
	
	
void UIControlSystem::ReplaceScreen(UIScreen *newMainControl)
{
	prevScreen = currentScreen;
	currentScreen = newMainControl;
}

	
UIScreen *UIControlSystem::GetScreen()
{
	return currentScreen;	
}
	
void UIControlSystem::AddPopup(UIPopup *newPopup)
{
	for (Vector<UIPopup*>::iterator it = popupsToRemove.begin(); it != popupsToRemove.end(); it++)
	{
        if (*it == newPopup) 
        {
            popupsToRemove.erase(it);
            return;
        }
	}
	newPopup->LoadGroup();
	popupContainer->AddControl(newPopup);
}
	
void UIControlSystem::RemovePopup(UIPopup *popup)
{
	popupsToRemove.push_back(popup);
}
	
void UIControlSystem::RemoveAllPopups()
{
	const List<UIControl*> &totalChilds = popupContainer->GetChildren();
	for (List<UIControl*>::const_iterator it = totalChilds.begin(); it != totalChilds.end(); it++)
	{
		popupsToRemove.push_back((UIPopup *)*it);
	}
}
	
UIControl *UIControlSystem::GetPopupContainer()
{
	return popupContainer;
}

	
void UIControlSystem::Reset()
{
	SetScreen(0);
}
	
void UIControlSystem::ProcessScreenLogic()
{
	/*
	 if next screen or we need to removecurrent screen
	 */
	if (nextScreen || removeCurrentScreen)
	{
		CancelAllInputs();
		
		// If we have transition set
		if (transition)
		{
			// check if we have not loading transition
			if (!transition->IsLoadingTransition())
			{
				// start transition and set currentScreen 
				transition->StartTransition(currentScreen, nextScreen);
				currentScreen = transition;
			}else
			{
				// if we got loading transition
				UILoadingTransition * loadingTransition = dynamic_cast<UILoadingTransition*> (transition);
				
				// Firstly start transition
				loadingTransition->StartTransition(currentScreen, nextScreen);
				
				// Manage transfer to loading transition through InTransition of LoadingTransition
                if (loadingTransition->GetInTransition())
                {
                    loadingTransition->GetInTransition()->StartTransition(currentScreen, loadingTransition);
                    currentScreen = SafeRetain(loadingTransition->GetInTransition());
                }
                else 
                {
                    if(currentScreen)
                    {
                        currentScreen->SystemWillDisappear();
                        if ((nextScreen == 0) || (currentScreen->GetGroupId() != nextScreen->GetGroupId()))
                        {
                            currentScreen->UnloadGroup();
                        }
                        currentScreen->SystemDidDisappear();
                    }
                        // if we have next screen we load new resources, if it equal to zero we just remove screen
                    loadingTransition->LoadGroup();
                    loadingTransition->SystemWillAppear();
                    currentScreen = loadingTransition;
                    loadingTransition->SystemDidAppear();
                }

			}
		}
        else	// if there is no transition do change immediatelly
		{	
			// if we have current screen we call events, unload resources for it group
			if(currentScreen)
			{
				currentScreen->SystemWillDisappear();
				if ((nextScreen == 0) || (currentScreen->GetGroupId() != nextScreen->GetGroupId()))
				{
					currentScreen->UnloadGroup();
				}
				currentScreen->SystemDidDisappear();
			}
			// if we have next screen we load new resources, if it equal to zero we just remove screen
			if (nextScreen)
			{
				nextScreen->LoadGroup();
				nextScreen->SystemWillAppear();
			}
			currentScreen = nextScreen;
            if (nextScreen) 
            {
				nextScreen->SystemDidAppear();
            }
			
			UnlockInput();
		}
		frameSkip = FRAME_SKIP;
		nextScreen = 0; // switch already happen so set nextScreen to zero
		removeCurrentScreen = false;
	}
	
	/*
	 if we have popups to remove, we removes them here
	 */
	for (Vector<UIPopup*>::iterator it = popupsToRemove.begin(); it != popupsToRemove.end(); it++)
	{
		UIPopup *p = *it;
		if (p) 
		{
			p->Retain();
			popupContainer->RemoveControl(p);
			p->UnloadGroup();
            p->Release();
		}
	}
	popupsToRemove.clear();
}

void UIControlSystem::Update()
{
	ProcessScreenLogic();
	
	float32 timeElapsed = SystemTimer::FrameDelta();
	
	if(currentScreen)
	{
		currentScreen->SystemUpdate(timeElapsed);
	}

	popupContainer->SystemUpdate(timeElapsed);
	
	SafeRelease(prevScreen);
}
	
void UIControlSystem::Draw()
{
//	if(currentScreen && (!currentPopup || currentPopup->isTransparent))
	if (currentScreen)
	{
		currentScreen->SystemDraw(baseGeometricData);
//		currentScreen->SystemDraw(Rect(0, 0, RenderManager::Instance()->GetScreenWidth(), RenderManager::Instance()->GetScreenHeight()));
	}

	popupContainer->SystemDraw(baseGeometricData);
	
	if(frameSkip > 0)
	{
		frameSkip--;
	}
}
	
void UIControlSystem::SwitchInputToControl(int32 eventID, UIControl *targetControl)
{
	for (Vector<UIEvent>::iterator it = totalInputs.begin(); it != totalInputs.end(); it++) 
	{
		if((*it).tid == eventID)
		{
			CancelInput(&(*it));
			
			if(targetControl->IsPointInside((*it).point))
			{
				(*it).controlState = UIEvent::CONTROL_STATE_INSIDE;
				targetControl->touchesInside++;
			}
			else 
			{
				(*it).controlState = UIEvent::CONTROL_STATE_OUTSIDE;
			}
			(*it).touchLocker = targetControl;
			targetControl->currentInputID = eventID;
			if(targetControl->GetExclusiveInput())
			{
				SetExclusiveInputLocker(targetControl);
			}
			else 
			{
				SetExclusiveInputLocker(NULL);
			}

			targetControl->totalTouches++;
		}
	}
}

	
void UIControlSystem::OnInput(int32 touchType, const Vector<UIEvent> &activeInputs, const Vector<UIEvent> &allInputs, bool fromReplay/* = false*/)
{
	if(Replay::IsPlayback() && !fromReplay) return;
	if (lockInputCounter > 0)return;

	if(frameSkip <= 0)
	{
		if(Replay::IsRecord())
		{
			int32 count = (int32)activeInputs.size();
			Replay::Instance()->RecordEventsCount(count);
			for(Vector<UIEvent>::const_iterator it = activeInputs.begin(); it != activeInputs.end(); ++it) 
			{
				UIEvent ev = *it;
				RecalculatePointToVirtual(ev.physPoint, ev.point);
				Replay::Instance()->RecordEvent(&ev);
			}

			count = (int32)allInputs.size();
			Replay::Instance()->RecordEventsCount(count);
			for(Vector<UIEvent>::const_iterator it = allInputs.begin(); it != allInputs.end(); ++it) 
			{
				UIEvent ev = *it;
				RecalculatePointToVirtual(ev.physPoint, ev.point);
				Replay::Instance()->RecordEvent(&ev);
			}
		}

		//check all touches for active state
//		Logger::Debug("IN   Active touches %d", activeInputs.size());
//		Logger::Debug("IN   Total touches %d", allInputs.size());
		for (Vector<UIEvent>::iterator it = totalInputs.begin(); it != totalInputs.end(); it++) 
		{
			(*it).activeState = UIEvent::ACTIVITY_STATE_INACTIVE;
			
			for (Vector<UIEvent>::const_iterator wit = activeInputs.begin(); wit != activeInputs.end(); wit++) 
			{
				if((*it).tid == (*wit).tid)
				{
					if((*it).phase == (*wit).phase && (*it).physPoint == (*wit).physPoint)
					{
						(*it).activeState = UIEvent::ACTIVITY_STATE_ACTIVE;
					}
					else 
					{
						(*it).activeState = UIEvent::ACTIVITY_STATE_CHANGED;
					}
					
					(*it).phase = (*wit).phase;
					(*it).timestamp = (*wit).timestamp;
					(*it).physPoint = (*wit).physPoint;
					RecalculatePointToVirtual((*it).physPoint, (*it).point);
					(*it).tapCount = (*wit).tapCount;
					break;
				}
			}
			if((*it).activeState == UIEvent::ACTIVITY_STATE_INACTIVE)
			{
				for (Vector<UIEvent>::const_iterator wit = allInputs.begin(); wit != allInputs.end(); wit++) 
				{
					if((*it).tid == (*wit).tid)
					{
						if((*it).phase == (*wit).phase && (*it).point == (*wit).point)
						{
							(*it).activeState = UIEvent::ACTIVITY_STATE_ACTIVE;
						}
						else 
						{
							(*it).activeState = UIEvent::ACTIVITY_STATE_CHANGED;
						}
						
						(*it).phase = (*wit).phase;
						(*it).timestamp = (*wit).timestamp;
						(*it).physPoint = (*wit).physPoint;
						(*it).point = (*wit).point;
						RecalculatePointToVirtual((*it).physPoint, (*it).point);
						(*it).tapCount = (*wit).tapCount;
						break;
					}
				}
			}
		}
		
		//add new touches
		for (Vector<UIEvent>::const_iterator wit = activeInputs.begin(); wit != activeInputs.end(); wit++) 
		{
			bool isFind = FALSE;
			for (Vector<UIEvent>::iterator it = totalInputs.begin(); it != totalInputs.end(); it++) 
			{
				if((*it).tid == (*wit).tid)
				{
					isFind = TRUE;
				}
			}
			if(!isFind)
			{
				totalInputs.push_back((*wit));
				totalInputs.back().activeState = UIEvent::ACTIVITY_STATE_CHANGED;
				RecalculatePointToVirtual(totalInputs.back().physPoint, totalInputs.back().point);
			}
		}
		for (Vector<UIEvent>::const_iterator wit = allInputs.begin(); wit != allInputs.end(); wit++) 
		{
			bool isFind = FALSE;
			for (Vector<UIEvent>::iterator it = totalInputs.begin(); it != totalInputs.end(); it++) 
			{
				if((*it).tid == (*wit).tid)
				{
					isFind = TRUE;
				}
			}
			if(!isFind)
			{
				totalInputs.push_back((*wit));
				totalInputs.back().activeState = UIEvent::ACTIVITY_STATE_CHANGED;
				RecalculatePointToVirtual(totalInputs.back().physPoint, totalInputs.back().point);
			}
		}
		
		//removes inactive touches and cancelled touches
		for (Vector<UIEvent>::iterator it = totalInputs.begin(); it != totalInputs.end(); it++) 
		{
			if((*it).activeState == UIEvent::ACTIVITY_STATE_INACTIVE || (*it).phase == UIEvent::PHASE_CANCELLED)
			{
				CancelInput(&(*it));
				totalInputs.erase(it);
				it = totalInputs.begin();
				if(it == totalInputs.end())
				{
					break;
				}
			}
		}
		
//		Logger::Debug("Total touches %d", totalInputs.size());
//		for (Vector<UIEvent>::iterator it = totalInputs.begin(); it != totalInputs.end(); it++) 
//		{
//			Logger::Debug("		ID %d", (*it).tid);
//			Logger::Debug("		phase %d", (*it).phase);
//		}


		if(currentScreen)
		{
			for(Vector<UIEvent>::iterator it = totalInputs.begin(); it != totalInputs.end(); it++) 
			{
				if((*it).activeState == UIEvent::ACTIVITY_STATE_CHANGED)
				{
					if(!popupContainer->SystemInput(&(*it)))
					{
						currentScreen->SystemInput(&(*it));
					}
				}
				if(totalInputs.empty())
				{
					break;
				}
			}
		}
	}
}

void UIControlSystem::CancelInput(UIEvent *touch)
{
	if(touch->touchLocker)
	{
		touch->touchLocker->SystemInputCancelled(touch);
		touch->touchLocker = NULL;
	}
}
void UIControlSystem::CancelAllInputs()
{
	for (Vector<UIEvent>::iterator it = totalInputs.begin(); it != totalInputs.end(); it++) 
	{
		CancelInput(&(*it));
	}
	totalInputs.clear();
}

void UIControlSystem::CancelInputs(UIControl *control)
{
	for (Vector<UIEvent>::iterator it = totalInputs.begin(); it != totalInputs.end(); it++) 
	{
		if(it->touchLocker == control)
		{
			CancelInput(&(*it));
		}
	}
}

//void UIControlSystem::SetTransitionType(int newTransitionType)
//{
//	transitionType = newTransitionType;
//}
	
int32 UIControlSystem::LockInput()
{
	lockInputCounter++;
	if (lockInputCounter > 0)
	{
		CancelAllInputs();
	}
	return lockInputCounter;
}

int32 UIControlSystem::UnlockInput()
{
	DVASSERT(lockInputCounter != 0);

	lockInputCounter--;
	if (lockInputCounter == 0)
	{
		// VB: Done that because hottych asked to do that.
		CancelAllInputs();
	}
	return lockInputCounter;
}
	
int32 UIControlSystem::GetLockInputCounter() const
{
	return lockInputCounter;
}

const Vector<UIEvent> & UIControlSystem::GetAllInputs()
{
	return totalInputs;
}
	
void UIControlSystem::SetExclusiveInputLocker(UIControl *locker)
{
	SafeRelease(exclusiveInputLocker);
	exclusiveInputLocker = SafeRetain(locker);
}
	
UIControl *UIControlSystem::GetExclusiveInputLocker()
{
	return exclusiveInputLocker;
}
    
void UIControlSystem::ScreenSizeChanged()
{
    popupContainer->SystemScreenSizeDidChanged(Rect(Core::Instance()->GetVirtualScreenXMin()
                                                 , Core::Instance()->GetVirtualScreenYMin()
                                                 , Core::Instance()->GetVirtualScreenXMax() - Core::Instance()->GetVirtualScreenXMin()
                                                 , Core::Instance()->GetVirtualScreenYMax() - Core::Instance()->GetVirtualScreenYMin()));
}

void UIControlSystem::SetHoveredControl(UIControl *newHovered)
{
    if (hovered != newHovered) 
    {
        if (hovered) 
        {
            hovered->SystemDidRemoveHovered();
            hovered->Release();
        }
        hovered = SafeRetain(newHovered);
        if (hovered) 
        {
            hovered->SystemDidSetHovered();
        }
    }
}
    
UIControl *UIControlSystem::GetHoveredControl(UIControl *newHovered)
{
    return hovered;
}

	
const UIGeometricData &UIControlSystem::GetBaseGeometricData()
{
	return baseGeometricData;	
}
	
void UIControlSystem::SetInputScreenAreaSize(int32 width, int32 height)
{
	if(Core::Instance()->GetScreenOrientation() == Core::SCREEN_ORIENTATION_PORTRAIT || Core::Instance()->GetScreenOrientation() == Core::SCREEN_ORIENTATION_PORTRAIT_UPSIDE_DOWN)
	{
		inputWidth = width;
		inputHeight = height;
	}
	else
	{
		inputWidth = height;
		inputHeight = width;
	}
}

void UIControlSystem::CalculateScaleMultipliers()
{
	
	float32 w, h;
	w = (float32)Core::Instance()->GetVirtualScreenWidth() / (float32)inputWidth;
	h = (float32)Core::Instance()->GetVirtualScreenHeight() / (float32)inputHeight;
	inputOffset.x = inputOffset.y = 0;
	if(w > h)
	{
		scaleFactor = w;
		inputOffset.y = 0.5f * ((float32)Core::Instance()->GetVirtualScreenHeight() - (float32)inputHeight * scaleFactor);
	}
	else
	{
		scaleFactor = h;
		inputOffset.x = 0.5f * ((float32)Core::Instance()->GetVirtualScreenWidth() - (float32)inputWidth * scaleFactor);
	}
}


void UIControlSystem::RecalculatePointToVirtual(const Vector2 &physicalPoint, Vector2 &virtualPoint)
{
	if(Replay::IsPlayback())
	{
		return;
	}

	if(Core::Instance()->GetScreenOrientation() == Core::SCREEN_ORIENTATION_LANDSCAPE_LEFT)
	{
		virtualPoint.x = (inputWidth - physicalPoint.y);
		virtualPoint.y = (physicalPoint.x);
	}
	else if(Core::Instance()->GetScreenOrientation() == Core::SCREEN_ORIENTATION_LANDSCAPE_RIGHT)
	{
		virtualPoint.x = (physicalPoint.y);
		virtualPoint.y = (inputHeight - physicalPoint.x);
	}
	else
	{
		virtualPoint = physicalPoint;
	}
	
	virtualPoint *= scaleFactor;
	virtualPoint += inputOffset;
	
}

void UIControlSystem::ReplayEvents()
{
	while(Replay::Instance()->IsEvent())
	{
		int32 activeCount = Replay::Instance()->PlayEventsCount();
		Vector<UIEvent> activeInputs;
		while(activeCount--)
		{
			UIEvent ev = Replay::Instance()->PlayEvent();
			activeInputs.push_back(ev);
		}

		int32 allCount = Replay::Instance()->PlayEventsCount();
		Vector<UIEvent> allInputs;
		while(allCount--)
		{
			UIEvent ev = Replay::Instance()->PlayEvent();
			allInputs.push_back(ev);
		}

		if(activeCount || allCount)
		{
			OnInput(0, activeInputs, allInputs, true);
		}
	}
}

};
