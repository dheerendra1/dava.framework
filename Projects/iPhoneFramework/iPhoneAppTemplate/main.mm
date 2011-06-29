//
//  main.m
//  MahJongQuest
//
//  Created by Vitaliy  Borodovsky on 10/6/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import <UIKit/UIKit.h>
#include "DAVAEngine.h"

using namespace DAVA;

extern void FrameworkDidLaunched();

int main(int argc, char *argv[]) {
    
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	Core::Create();
	FrameworkDidLaunched();

	//TODO: fix this ugly realisation. Orientation need to be geted from the info.plist 
	// and must be inited as part of the core 		
	// DAVA::Core::Init(DAVA::RenderManager::ORIENTATION_PORTRAIT, 320, 480); for example
	// orientation is not a part of the rendering - orientation is part of an application at all
	KeyedArchive * options = DAVA::Core::GetOptions();
	String orientation = options->GetString("orientation", "portrait");
	if (orientation == "portrait")
	{
		DAVA::RenderManager::Init(DAVA::RenderManager::ORIENTATION_PORTRAIT, 320, 480);
	}
	else if (orientation == "landscape_left")
	{
		DAVA::RenderManager::Init(DAVA::RenderManager::ORIENTATION_LANDSCAPE_LEFT, 320, 480);
	}
	else if (orientation == "landscape_right")
	{
		DAVA::RenderManager::Init(DAVA::RenderManager::ORIENTATION_LANDSCAPE_RIGHT, 320, 480);
	}
	else if (orientation == "portrait_upside_down")
	{
		DAVA::RenderManager::Init(DAVA::RenderManager::ORIENTATION_PORTRAIT_UPSIDE_DOWN, 320, 480);
	}
	
	int retVal = UIApplicationMain(argc, argv, nil, nil);
	
	[pool release];
    return retVal;
}
