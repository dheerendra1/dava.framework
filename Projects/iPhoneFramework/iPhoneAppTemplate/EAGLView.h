//
//  EAGLView.h
//  KellyGreen
//
//  Created by Hottych on 10/27/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

#import "ESRenderer.h"
#include "DAVAEngine.h"


//using namespace DAVA;
// This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
// The view content is basically an EAGL surface you render your OpenGL scene into.
// Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
@interface EAGLView : UIView
{    
@private
	id <ESRenderer> renderer;
	
	BOOL animating;
	BOOL displayLinkSupported;
	NSInteger animationFrameInterval;
	// Use of the CADisplayLink class is the preferred method for controlling your animation timing.
	// CADisplayLink will link to the main display and fire every vsync when added to a given run-loop.
	// The NSTimer class is used only as fallback when running on a pre 3.1 device where CADisplayLink
	// isn't available.
	id displayLink;
    NSTimer *animationTimer;
	bool isLoading;
	bool isLoaded;
	
	DAVA::Vector<DAVA::UIEvent> totalTouches;
	DAVA::Vector<DAVA::UIEvent> activeTouches;
	
	DAVA::int32 currFPS;

}

@property (readonly, nonatomic, getter=isAnimating) BOOL animating;
@property (nonatomic) NSInteger animationFrameInterval;


- (void) startAnimation;
- (void) stopAnimation;
- (void) drawView:(id)sender;

- (void) setCurrentContext;

@end
