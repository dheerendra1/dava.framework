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

#import "UI/UIScreenManageriPhoneImpl.h"

#if defined(__DAVAENGINE_IPHONE__)

@interface ScreenManagerImpl()
- (void) notimerSetViewController: (UIViewController*)viewController;
@end


@implementation ScreenManagerImpl

static ScreenManagerImpl * staticInstance = 0;
+ (ScreenManagerImpl*) instance;
{
	if (!staticInstance)
	{
		staticInstance = [[ScreenManagerImpl alloc] init];
	}
	return staticInstance;
}

- (id) init
{
	if (self = [super init])
	{
		appWindow = [[UIApplication sharedApplication] keyWindow];
		inTransition = NO;
		lastActiveController = 0;
		NSLog(@"[ScreenTransitionManager] create appWindow:%@", appWindow);
	}
	return self;
}

// 
- (void)applicationLaunched: (UIViewController*)firstController;
{
	[self notimerSetViewController: firstController];
}

- (void)dealloc
{
	NSLog(@"[ScreenTransitionManager] release");
		
	[super dealloc];
}


- (void) notimerSetViewController: (UIViewController*)viewController
{
	//NSLog(@"Start Switching\n");
	if (inTransition)return;
	//NSLog(@"Continue Switching\n");
	
	if (nil != activeController)
	{
		//[activeController screenWillDisappear];
		[activeController.view removeFromSuperview];
		activeController = nil;
	}
	
	if (viewController)
	{
		activeController = viewController;
		//[activeController screenWillAppear];
		[appWindow addSubview:activeController.view];
	}
	
	if (applyAnimationTransition)
	{
		[UIView beginAnimations:nil context:NULL];
		[UIView setAnimationDuration:0.75];
		[UIView setAnimationTransition:transition forView:appWindow cache:YES];
		[UIView setAnimationDelegate:self];
		[UIView setAnimationDidStopSelector:@selector(animationDidStop:finished:context:)];
		[self animationDidStart: nil];
	}
	
	if (applyAnimationTransition2)
	{
		//[self recursiveRemoveAnimations: mainView];
		
		// Set up the animation
		CATransition *animation = [CATransition animation];
		[animation setDelegate: self];
		
		[animation setType: type];
		[animation setSubtype: subtype];
		[animation setStartProgress: 0.0f];
		[animation setEndProgress: 1.0f];
		
		// Set the duration and timing function of the transtion -- duration is passed in as a parameter, use ease in/ease out as the timing function
		[animation setDuration: 0.75f];
		[animation setTimingFunction:[CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut]];
		
		[[appWindow layer] addAnimation:animation forKey:@"TransferAnimation"];	
		
		applyAnimationTransition2 = FALSE;
	}
	
	if (applyAnimationTransition)
	{
		[UIView commitAnimations];
		applyAnimationTransition = FALSE;
	}
	//NSLog(@"Switched\n");
}

- (void) setViewControllerByTimer: (NSTimer*)timer
{
	UIViewController* viewController = (UIViewController*)timer.userInfo;
	[timer invalidate];
	[self notimerSetViewController: viewController];
}

- (void) setViewController: (UIViewController*)viewController
{
	[NSTimer scheduledTimerWithTimeInterval:0.03 target:self selector:@selector(setViewControllerByTimer:) userInfo:viewController repeats:NO];
}

/*- (void) startLoading
{
	[[UIApplication sharedApplication] beginIgnoringInteractionEvents];
	
	// trigger animation here
}

- (void) stopLoading
{
	// stop animation here
	
	[[UIApplication sharedApplication] endIgnoringInteractionEvents];
} */

//- (void)showPopupController: (UIViewController*)viewController
//{	
//	//NSLog(@"[AppScreens] View controller retain count: %d\n", [viewController retainCount]);
//	if (activeController)
//	{
//		UIView * popupView = optionsController.view;
//		[activeController.view addSubview:popupView];
//
//		popupView.alpha = 0.0;
//		[UIView beginAnimations:nil context:nil];
//		[UIView setAnimationDuration: 0.8];
//		popupView.alpha = 1.0;
//		[UIView commitAnimations];
//	}
//}

- (void)animationDidStart:(CAAnimation *)animation 
{
	//NSLog(@"Begin ignoring\n");
	[[UIApplication sharedApplication] beginIgnoringInteractionEvents];
	inTransition = YES;
}

- (void)animationDidStop:(CAAnimation *)animation finished:(BOOL)finished 
{
	//NSLog(@"End ignoring\n");
	inTransition = NO;
	[[UIApplication sharedApplication] endIgnoringInteractionEvents];
}

- (void)animationDidStop:(NSString *)animationID finished:(NSNumber *)finished context:(void *)context 
{	
	[self animationDidStop:nil finished:YES];
}

@end

#endif // #if defined(__DAVAENGINE_IPHONE__)

