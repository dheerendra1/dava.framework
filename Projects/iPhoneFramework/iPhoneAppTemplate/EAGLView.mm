//
//  EAGLView.m
//  KellyGreen
//
//  Created by Hottych on 10/27/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import "EAGLView.h"

#import "ES1Renderer.h"
#import "ES2Renderer.h"

#include "DAVAEngine.h"

@implementation EAGLView

@synthesize animating;
@dynamic animationFrameInterval;

// You must implement this method
+ (Class) layerClass
{
    return [CAEAGLLayer class];
}

//The GL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id) initWithCoder: (NSCoder *)aDecoder
{    
    if ((self = [super initWithCoder:aDecoder]))
	{
        // Get the layer
		DAVA::Logger::Debug("CREATING GLVIEW");
		if (DAVA::Core::IsAutodetectContentScaleFactor()) 
		{
			if ([UIScreen instancesRespondToSelector: @selector(scale) ]) 
			{
				self.contentScaleFactor = [UIScreen mainScreen].scale;
				DAVA::Core::SetContentScaleFactor([UIScreen mainScreen].scale);
			}
		}
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        
        eaglLayer.opaque = TRUE;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
		
		
//		renderer = [[ES2Renderer alloc] init];
//		
//		if (!renderer)
		{
			renderer = [[ES1Renderer alloc] init];
			
			if (!renderer)
			{
				[self release];
				return nil;
			}
		}

		
		
        
		self.multipleTouchEnabled = YES;
		animating = FALSE;
		displayLinkSupported = FALSE;
		animationFrameInterval = 1;
		currFPS = 60;
		displayLink = nil;
		animationTimer = nil;
		
		// A system version of 3.1 or greater is required to use CADisplayLink. The NSTimer
		// class is used as fallback when it isn't available.
		//NSString *reqSysVer = @"3.1";
		//NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
		//if ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending)
		//	displayLinkSupported = TRUE;
		
    }
	
    return self;
}



- (void) drawView:(id)sender
{

	DAVA::RenderManager::Lock();
	[renderer startRendering];
	
	DAVA::Core::SystemProcessFrame();
	
	[renderer endRendering];
	DAVA::RenderManager::Unlock();
	
	if(currFPS != DAVA::RenderManager::GetFPS())
	{
		currFPS = DAVA::RenderManager::GetFPS();
		float interval = 60.0 / currFPS;
		if(interval < 1.0)
		{
			interval = 1.0;
		}
		[self setAnimationFrameInterval:(int)interval];
	}
}


- (void) layoutSubviews
{
	[renderer resizeFromLayer:(CAEAGLLayer*)self.layer];
    [self drawView:nil];
}

- (NSInteger) animationFrameInterval
{
	return animationFrameInterval;
}

- (void) setAnimationFrameInterval:(NSInteger)frameInterval
{
	// Frame interval defines how many display frames must pass between each time the
	// display link fires. The display link will only fire 30 times a second when the
	// frame internal is two on a display that refreshes 60 times a second. The default
	// frame interval setting of one will fire 60 times a second when the display refreshes
	// at 60 times a second. A frame interval setting of less than one results in undefined
	// behavior.
	if (frameInterval >= 1)
	{
		animationFrameInterval = frameInterval;
		
		if (animating)
		{
			[self stopAnimation];
			[self startAnimation];
		}
	}
}

- (void) startAnimation
{
	if (!animating)
	{
		if (displayLinkSupported)
		{
			// CADisplayLink is API new to iPhone SDK 3.1. Compiling against earlier versions will result in a warning, but can be dismissed
			// if the system version runtime check for CADisplayLink exists in -initWithCoder:. The runtime check ensures this code will
			// not be called in system versions earlier than 3.1.

			displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(drawView:)];
			[displayLink setFrameInterval:animationFrameInterval];
			[displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
		}
		else
			animationTimer = [NSTimer scheduledTimerWithTimeInterval:(NSTimeInterval)((1.0 / 60.0) * animationFrameInterval) target:self selector:@selector(drawView:) userInfo:nil repeats:TRUE];
		
		animating = TRUE;
	}
}

- (void)stopAnimation
{
	if (animating)
	{
		if (displayLinkSupported)
		{
			[displayLink invalidate];
			displayLink = nil;
		}
		else
		{
			[animationTimer invalidate];
			animationTimer = nil;
		}
		
		animating = FALSE;
	}
}

void MoveTouchsToVector(void *inTouches, DAVA::Vector<DAVA::UIEvent> *outTouches)
{
	NSArray *ar = (NSArray *)inTouches;
	for(UITouch *curTouch in ar)
	{
		DAVA::UIEvent newTouch;
		newTouch.tid = (DAVA::int32)curTouch;
		newTouch.buttonId = DAVA::UIEvent::BUTTON_1;
		CGPoint p = [curTouch locationInView: curTouch.view ];
		if(DAVA::RenderManager::GetScreenOrientation() == DAVA::RenderManager::ORIENTATION_LANDSCAPE_LEFT)
		{
			newTouch.point.x = (480 - p.y);
			newTouch.point.y = (p.x);
		}
		else if(DAVA::RenderManager::GetScreenOrientation() == DAVA::RenderManager::ORIENTATION_LANDSCAPE_RIGHT)
		{
			newTouch.point.x = (p.y);
			newTouch.point.y = (320 - p.x);
		}
		else
		{
			newTouch.point.x = p.x;
			newTouch.point.y = p.y;
		}
		newTouch.timestamp = curTouch.timestamp;
		newTouch.tapCount = curTouch.tapCount;
		
		switch(curTouch.phase)
		{
			case UITouchPhaseBegan:
				newTouch.phase = DAVA::UIEvent::PHASE_BEGAN;
				break;
			case UITouchPhaseEnded:
				newTouch.phase = DAVA::UIEvent::PHASE_ENDED;
				break;
			case UITouchPhaseMoved:
			case UITouchPhaseStationary:
				newTouch.phase = DAVA::UIEvent::PHASE_DRAG;
				break;
			case UITouchPhaseCancelled:
				newTouch.phase = DAVA::UIEvent::PHASE_CANCELLED;
				break;
				
		}
		outTouches->push_back(newTouch);
	}
}


-(void)process:(int) touchType touch:(NSArray*)active withEvent: (NSArray*)total
{
	MoveTouchsToVector(active, &activeTouches);
	MoveTouchsToVector(total, &totalTouches);
	DAVA::UIControlSystem::Instance()->OnInput(touchType, activeTouches, totalTouches);
	activeTouches.clear();
	totalTouches.clear();
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	//	LogDebug("TOUCH BEGAN");
	[self process:DAVA::UIEvent::PHASE_BEGAN touch:[touches allObjects] withEvent:[[event allTouches] allObjects]];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	//	LogDebug("TOUCH MOVED");
	[self process:DAVA::UIEvent::PHASE_DRAG touch:[touches allObjects] withEvent:[[event allTouches] allObjects]];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	//	LogDebug("TOUCH ENDED");
	[self process:DAVA::UIEvent::PHASE_ENDED touch:[touches allObjects] withEvent:[[event allTouches] allObjects]];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	[self process:DAVA::UIEvent::PHASE_CANCELLED touch:[touches allObjects] withEvent:[[event allTouches] allObjects]];
}

- (void) dealloc
{
    [renderer release];
	
    [super dealloc];
}

- (void) setCurrentContext
{
	[renderer setCurrentContext];
}


@end
