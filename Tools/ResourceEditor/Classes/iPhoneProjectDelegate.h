//
//  TransportControlAppDelegate.h
//  TransportControl
//
//  Created by Vitaliy  Borodovsky on 11/8/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import "Platform/TemplateiOS/HelperAppDelegate.h"
#import "AppScreens.h"
#import "DAVAEngine.h"
#import "GameCore.h"

#if defined(__DAVAENGINE_IPHONE__)

#import <UIKit/UIKit.h>

@interface iPhoneProjectDelegate : HelperAppDelegate
{
    UIWindow *window;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;

@end

#endif // #if defined(__DAVAENGINE_IPHONE__)
