//
//  TransportControlAppDelegate.m
//  TransportControl
//
//  Created by Vitaliy  Borodovsky on 11/8/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import "iPhoneProjectDelegate.h"

#if defined(__DAVAENGINE_IPHONE__)

@implementation iPhoneProjectDelegate

@synthesize window;

- (void)applicationDidFinishLaunching:(UIApplication *)application 
{    
	[window makeKeyAndVisible];
	window.backgroundColor = [UIColor redColor];
	
	[super applicationDidFinishLaunching:application];
}

- (void)dealloc 
{
    [window release];
    [super dealloc];
}

@end

#endif // #if defined(__DAVAENGINE_IPHONE__)
