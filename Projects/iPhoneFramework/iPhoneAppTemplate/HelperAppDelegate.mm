//
//  HelperAppDelegate.m
//  MahJongQuest
//
//  Created by Vitaliy  Borodovsky on 11/5/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "HelperAppDelegate.h"
#include "UIScreenManager.h"
#include "ApplicationCore.h"
#include "MemoryManager.h"

extern  void FrameworkWillTerminate();

@implementation HelperAppDelegate

- (void)applicationDidFinishLaunching:(UIApplication *)application 
{
	DAVA::Core::GetApplicationCore()->OnAppStarted();
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
	DAVA::Core::GetApplicationCore()->Resume();
}

- (void)applicationWillResignActive:(UIApplication *)application
{
	DAVA::Core::GetApplicationCore()->Suspend();
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
	DAVA::Core::GetApplicationCore()->GoBackground();
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
	DAVA::Core::GetApplicationCore()->Resume();
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	NSLog(@"Application termination started");
	DAVA::Core::GetApplicationCore()->OnAppFinished();
	NSLog(@"System release started");
	DAVA::UIScreenManager::Instance()->Release(); 
	DAVA::Core::Release();

	DAVA::Sprite::DumpSprites();
	DAVA::Texture::DumpTextures();
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
