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
#include "Base/BaseTypes.h"
#if defined(__DAVAENGINE_IPHONE__)

#include "UI/UIScreenManager.h"
#include "UI/UIScreenManageriPhoneImpl.h"
#include "BaseObject.h"
//#include "EAGLViewController.h"
//#include "EAGLView.h"

@class EAGLViewController;
@class EAGLView;

namespace DAVA 
{
	
UIScreenManager::UIScreenManager()
{
	glControllerId = -1;
	activeControllerId = -1;
	activeScreenId = -1;
}

UIScreenManager::~UIScreenManager()
{
	Vector<Screen> releaseBuf;
	for(Map<int, Screen>::const_iterator it = screens.begin(); it != screens.end(); it++)
	{
		if(it->second.type == Screen::TYPE_SCREEN)
		{
			((UIScreen*)it->second.value)->UnloadGroup();
			//it->second.type == Screen::TYPE_NULL;
			releaseBuf.push_back(it->second);
		}
	}
	for(Vector<Screen>::const_iterator it = releaseBuf.begin(); it != releaseBuf.end(); it++)
	{
		((UIScreen*)it->value)->Release();
	}
	
}
void UIScreenManager::SetGLControllerId(int _glControllerId)
{
	glControllerId = _glControllerId;
}
	
	
void UIScreenManager::ActivateGLController()
{
	
}

void UIScreenManager::ScreenSizeChanged()
{
    GetScreen()->SystemScreenSizeDidChanged(Rect(Core::Instance()->GetVirtualScreenXMin()
                                                 , Core::Instance()->GetVirtualScreenYMin()
                                                 , Core::Instance()->GetVirtualScreenXMax() - Core::Instance()->GetVirtualScreenXMin()
                                                 , Core::Instance()->GetVirtualScreenYMax() - Core::Instance()->GetVirtualScreenYMin()));
}


void UIScreenManager::SetFirst(int screenId)
{
	Screen & screen = screens[screenId];
	if (screen.type == Screen::TYPE_CONTROLLER)
	{
		[[ScreenManagerImpl instance] applicationLaunched:(UIViewController*)screen.value];	
		activeControllerId = screenId;
	}else if (screen.type == Screen::TYPE_SCREEN)
	{
		Screen & glController = screens[glControllerId];
		UIViewController * controller = (UIViewController *)glController.value;
		if (controller)
		{
			[[ScreenManagerImpl instance] applicationLaunched:(UIViewController*)controller];	
			activeControllerId = glControllerId;
		}else
		{
			Logger::Error("ScreenManager::SetFirst no gl controller registered (use SetGLControllerId)");
		}
		activeScreenId = screenId;
		UIControlSystem::Instance()->SetScreen((UIScreen*)screen.value);
	}else
	{
		Logger::Error("ScreenManager::SetFirst wrong type of screen");
	}
}

	
	
void UIScreenManager::SetScreen(int screenId, UIScreenTransition * transition)
{
	Screen & screen = screens[screenId];
	if (screen.type == Screen::TYPE_CONTROLLER)
	{
		UIViewController * controller = (UIViewController *)screen.value;
		if (controller)
		{
			[[ScreenManagerImpl instance] setViewController:controller];
		}
		activeControllerId = screenId;
		UIControlSystem::Instance()->SetScreen(0, 0);
		UIControlSystem::Instance()->ProcessScreenLogic();
		
		activeScreenId = -1;
	}else if (screen.type == Screen::TYPE_SCREEN)
	{
		// Set GL Controller first
		if (activeControllerId != glControllerId)
		{
			Screen & glController = screens[glControllerId];
			UIViewController * controller = (UIViewController *)glController.value;
			if (controller)
			{
				[[ScreenManagerImpl instance] setViewController:controller];
			}
			activeControllerId = glControllerId;
		}
		activeScreenId = screenId;
		
		UIControlSystem::Instance()->SetScreen((UIScreen*)screen.value, transition);
	}
}

void UIScreenManager::RegisterController(int controllerId, void * controller)
{
	screens[controllerId] = Screen(Screen::TYPE_CONTROLLER, controller);
}

void UIScreenManager::RegisterScreen(int screenId, UIScreen * screen)
{
	screens[screenId] = Screen(Screen::TYPE_SCREEN, SafeRetain(screen));
}
	
UIScreen *UIScreenManager::GetScreen(int screenId)
{
	Screen & screen = screens[screenId];
	if (screen.type == Screen::TYPE_CONTROLLER)
	{
		return NULL;
	}
	else if (screen.type == Screen::TYPE_SCREEN)
	{
		return (UIScreen*)screen.value;
	}
	return NULL;
}
UIScreen *UIScreenManager::GetScreen()
{
	return GetScreen(activeScreenId);
}
	
int UIScreenManager::GetScreenId()
{
	return activeScreenId;
}

void *UIScreenManager::GetController(int controllerId)
{
	Screen & screen = screens[controllerId];
	if (screen.type == Screen::TYPE_SCREEN)
	{
		return NULL;
	}
	else if (screen.type == Screen::TYPE_CONTROLLER)
	{
		return (void*)screen.value;
	}
	return NULL;
	
}
	
void *UIScreenManager::GetController()
{
	return GetController(activeControllerId);
}
	
int UIScreenManager::GetControllerId()
{
	return activeControllerId;
}
	
	
void UIScreenManager::StopGLAnimation()
{
	Screen & glController = screens[glControllerId];
	UIViewController * controller = (UIViewController *)glController.value;
	EAGLView * view = (EAGLView *)controller.view;
    [view performSelector: @selector(stopAnimation)];
}

void UIScreenManager::StartGLAnimation()
{
	Screen & glController = screens[glControllerId];
	UIViewController * controller = (UIViewController *)glController.value;
	EAGLView * view = (EAGLView *)controller.view;
    [view performSelector: @selector(startAnimation)];
}

	
}

#endif // #if defined(__DAVAENGINE_IPHONE__)


