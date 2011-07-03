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
#include "BaseTypes.h"
#if defined(__DAVAENGINE_IPHONE__)


#import <UIKit/UIKit.h>
#import "HelperAppDelegate.h"

extern  void FrameworkWillTerminate();
extern  void FrameworkDidLaunched();


int DAVA::Core::Run(int argc, char * argv[], AppHandle handle)
{
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	DAVA::Core * core = new DAVA::Core();
	core->CreateSingletons();
	FrameworkDidLaunched();
	
	{//detecting physical screen size and initing core system with this size
		if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
		{
			// The device is an iPad.
			DAVA::UIControlSystem::Instance()->SetInputScreenAreaSize(768, 1024);
			DAVA::Core::Instance()->SetPhysicalScreenSize(768, 1024);
			
		}
		else
		{
			// The device is an iPhone or iPod touch.
			DAVA::UIControlSystem::Instance()->SetInputScreenAreaSize(320, 480);
			if (DAVA::Core::IsAutodetectContentScaleFactor()) 
			{
				if ([::UIScreen instancesRespondToSelector: @selector(scale) ]
					&& [::UIView instancesRespondToSelector: @selector(contentScaleFactor) ]) 
				{
					unsigned int scv = (unsigned int)[[::UIScreen mainScreen] scale];
					if (scv != 2) 
					{
						DAVA::Core::Instance()->SetPhysicalScreenSize(320.0f, 480.0f);
					}
					else 
					{
						DAVA::Core::Instance()->SetPhysicalScreenSize(640.0f, 960.0f);
					}
				}
				else 
				{
					DAVA::Core::Instance()->SetPhysicalScreenSize(320, 480);
				}
			}
			else 
			{
				DAVA::Core::Instance()->SetPhysicalScreenSize(320, 480);
			}
		}
		
	}
		
	int retVal = UIApplicationMain(argc, argv, nil, nil);
	
	[pool release];
	
	return retVal;		
}

#include "Core/Core.h"
#include "Core/ApplicationCore.h"
#include "Debug/MemoryManager.h"
#include "UI/UIScreenManager.h"


@implementation HelperAppDelegate

@synthesize glController;


- (void)applicationDidFinishLaunching:(UIApplication *)application 
{
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
	{
		UIWindow *wnd = application.keyWindow;
		[wnd setFrame: CGRectMake(0, 0, 768, 1024)];
	}
	else
	{
		// The device is an iPhone or iPod touch.
	}
	
	glController = [[EAGLViewController alloc] init];
	DAVA::UIScreenManager::Instance()->RegisterController(CONTROLLER_GL, glController);
	DAVA::UIScreenManager::Instance()->SetGLControllerId(CONTROLLER_GL);
	
	DAVA::Core::Instance()->SystemAppStarted();
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
	DAVA::Core::Instance()->Resume();
}

- (void)applicationWillResignActive:(UIApplication *)application
{
	DAVA::Core::Instance()->Suspend();
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
	DAVA::Core::Instance()->GoBackground();
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
	DAVA::Core::Instance()->Resume();
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	NSLog(@"Application termination started");
	DAVA::Core::Instance()->SystemAppFinished();
	NSLog(@"System release started");
	DAVA::Core::Instance()->ReleaseSingletons();

//	DAVA::Sprite::DumpSprites();
//	DAVA::Texture::DumpTextures();
#ifdef ENABLE_MEMORY_MANAGER
	if (DAVA::MemoryManager::Instance() != 0)
	{
		DAVA::MemoryManager::Instance()->FinalLog();
	}
#endif
	FrameworkWillTerminate();
	NSLog(@"Application termination finished");
}

@end
#endif 
