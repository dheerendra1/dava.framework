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
#import "MainWindowController.h"
#include "CorePlatformMacOS.h"

extern void FrameworkDidLaunched();
extern void FrameworkWillTerminate();


namespace DAVA 
{
	int Core::Run(int argc, char *argv[], AppHandle handle)
	{
		NSAutoreleasePool * globalPool = 0;
		globalPool = [[NSAutoreleasePool alloc] init];
		DAVA::CoreMacOSPlatform * core = new DAVA::CoreMacOSPlatform();
		core->SetCommandLine(argc, argv);
		core->CreateSingletons();

		[NSApplication sharedApplication];
		[NSApp setDelegate:[[[MainWindowController alloc] init] autorelease]];
		
		int retVal = NSApplicationMain(argc,  (const char **) argv);
		
		core->ReleaseSingletons();
#ifdef ENABLE_MEMORY_MANAGER
		if (DAVA::MemoryManager::Instance() != 0)
		{
			DAVA::MemoryManager::Instance()->FinalLog();
		}
#endif
		[globalPool release];
		globalPool = 0;
		return retVal;
	}
	
	int Core::RunCmdTool(int argc, char *argv[], AppHandle handle)
	{
		NSAutoreleasePool * globalPool = 0;
		globalPool = [[NSAutoreleasePool alloc] init];
		DAVA::CoreMacOSPlatform * core = new DAVA::CoreMacOSPlatform();
		core->SetCommandLine(argc, argv);
		core->EnableConsoleMode();
		core->CreateSingletons();
		
		FrameworkDidLaunched();
		FrameworkWillTerminate();
		
		core->ReleaseSingletons();
#ifdef ENABLE_MEMORY_MANAGER
		if (DAVA::MemoryManager::Instance() != 0)
		{
			DAVA::MemoryManager::Instance()->FinalLog();
		}
#endif
		[globalPool release];
		globalPool = 0;
		return 0;
	}
}

@interface MainWindowController ()
- (BOOL) isAnimating;
- (void) startAnimation;
- (void) stopAnimation;
- (void) toggleAnimation;

- (void) startAnimationTimer;
- (void) stopAnimationTimer;
- (void) animationTimerFired:(NSTimer *)timer;

- (void) switchToFullScreen;

- (void)windowWillMiniaturize:(NSNotification *)notification;
- (void)windowDidMiniaturize:(NSNotification *)notification;
- (void)windowDidDeminiaturize:(NSNotification *)notification;
@end

@implementation MainWindowController


static MainWindowController * mainWindowController = nil;

/* This code disabled for now and left for the future
 */
namespace DAVA 
{
	static Vector2 mouseLocation = Vector2();
	
	const Vector2 & Core::GetMouseLocation()
	{
		NSPoint p = [mainWindowController->mainWindow mouseLocationOutsideOfEventStream]; //[NSEvent locationInWindow]; 
		p = [mainWindowController->openGLView convertPointFromBase: p];

		mouseLocation.x = p.x;
		mouseLocation.y = Core::Instance()->GetPhysicalScreenHeight() - p.y;
		// mouseLocation.y = 
		return mouseLocation;
	}
	
}

- (id)init
{
	NSLog(@"Application init");
	if (self = [super init])
	{
		mainWindowController = self;
		openGLView = nil;
		mainWindow = nil;
		fullscreenWindow = nil;
		fullScreenContext = nil;
		isAnimating = false;
		animationTimer = nil;
		timeBefore = 0;
		stayInFullScreenMode = NO;          // this flag indicating that we want to leave full screen mode
		core = 0;		

	}
	return self;
}

- (void)dealloc
{
	NSLog(@"Application dealloc");

	[super dealloc];
}

-(void)createWindows
{
	NSLog(@"Awake from NIB");
	//[NSCursor hide]; 
	DisplayMode fullscreenMode = Core::Instance()->GetCurrentDisplayMode();
	
	// launch framework and setup all preferences
	FrameworkDidLaunched();
    RenderManager::Create(Core::RENDERER_OPENGL);
    
	
	//Core::Instance()->Creat();
    
		// do all ground work & setup window itself according to value specified by user
	KeyedArchive * options = DAVA::Core::GetOptions();
	int32 width = options->GetInt("width", 800);
	int32 height = options->GetInt("height", 600);
	
	String title = options->GetString("title", "[set application title using core options property 'title']");
	
	mainWindow = [[NSWindow alloc] initWithContentRect:NSMakeRect((fullscreenMode.width - width) / 2, 
																  (fullscreenMode.height - height) / 2, width, height) 
											 styleMask:NSTitledWindowMask+NSMiniaturizableWindowMask+NSClosableWindowMask
											backing:NSBackingStoreBuffered defer:FALSE];
	[mainWindow setDelegate:self];
	openGLView = [[OpenGLView alloc]initWithFrame: NSMakeRect(0, 0, width, height)];
	[mainWindow setContentView: openGLView];

	NSRect rect;
	rect.origin.x = 0;
	rect.origin.y = 0;
	rect.size.width = width;
	rect.size.height = height;

	[mainWindow setContentSize:rect.size];
	[openGLView setFrame: rect];
	
	core = Core::GetApplicationCore();

	// start animation
	isAnimating = NO;
	
	currFPS = RenderManager::Instance()->GetFPS();
    [self startAnimation];

	// make window main
	[mainWindow makeKeyAndOrderFront:nil];
	//[mainWindow setLevel: NSMainMenuWindowLevel + 1];
	[mainWindow setTitle:[NSString stringWithFormat:@"%s", title.c_str()]];
	[mainWindow setAcceptsMouseMovedEvents:YES];

//	if ([mainWindow isMainWindow])
//	{
//		NSLog(@"****** Our window is MAIN window");
//	}
}


- (NSWindow *) beginFullScreen
{ 
//	fullscreenWindow = [ [NSWindow alloc]
//			  initWithContentRect:[[NSScreen mainScreen] frame]
//			  styleMask:NSBorderlessWindowMask
//			  backing:NSBackingStoreBuffered
//			  defer:NO];
    
    fullscreenWindow = [ [NSFullScreenWindow alloc]
                        initWithContentRect:[[NSScreen mainScreen] frame]
                        styleMask:NSBorderlessWindowMask
                        backing:NSBackingStoreBuffered
                        defer:NO];
    
	[NSMenu setMenuBarVisible:NO];
	[fullscreenWindow setHasShadow:NO];
	[fullscreenWindow setContentView:[mainWindow contentView]];
	
	[fullscreenWindow makeKeyAndOrderFront: fullscreenWindow];
	[mainWindow orderOut:nil];
	return fullscreenWindow;
}

- (void)endFullScreen
{ 
	[NSMenu setMenuBarVisible:YES];
	[mainWindow setContentView:[fullscreenWindow contentView]];
	[mainWindow makeKeyAndOrderFront:nil];
	[fullscreenWindow orderOut:nil];
	[fullscreenWindow release];
}

// some macros to make code more readable.
#define GetModeWidth(mode) GetDictionaryLong((mode), kCGDisplayWidth)
#define GetModeHeight(mode) GetDictionaryLong((mode), kCGDisplayHeight)
#define GetModeRefreshRate(mode) GetDictionaryLong((mode), kCGDisplayRefreshRate)
#define GetModeBitsPerPixel(mode) GetDictionaryLong((mode), kCGDisplayBitsPerPixel)

//------------------------------------------------------------------------------------------
long GetDictionaryLong(CFDictionaryRef theDict, const void* key) 
{
	// get a long from the dictionary
	long value = 0;
	CFNumberRef numRef;
	numRef = (CFNumberRef)CFDictionaryGetValue(theDict, key); 
	if (numRef != NULL)
		CFNumberGetValue(numRef, kCFNumberLongType, &value); 	
	return value;
}

- (void)windowWillMiniaturize:(NSNotification *)notification
{
//    NSLog(@"[MainWindowController] windowWillMiniaturize");
//    if(core)
//    {
//        core->OnSuspend();
//    }
//    else 
//    {
//        Core::Instance()->SetIsActive(false);
//    }
}

- (void)windowDidMiniaturize:(NSNotification *)notification
{
    NSLog(@"[MainWindowController] windowDidMiniaturize");
    
    SoundSystem::Instance()->Suspend();
    Core::Instance()->SetIsActive(false);
    
//    if(core)
//    {
//        core->OnSuspend();
//    }
//    else 
//    {
//        Core::Instance()->SetIsActive(false);
//    }
}

- (void)windowDidDeminiaturize:(NSNotification *)notification
{
    NSLog(@"[MainWindowController] windowDidDeminiaturize");
    
    SoundSystem::Instance()->Resume();
    Core::Instance()->SetIsActive(true);
    
//    if(core)
//    {
//        core->OnResume();
//    }
//    else 
//    {
//        Core::Instance()->SetIsActive(true);
//    }
}

// Action method wired up to fire when the user clicks the "Go FullScreen" button.  We remain in this method until the user exits FullScreen mode.
- (void) switchToFullScreen
{
    [self beginFullScreen];
	CGDisplayErr err;
 	
	// Take control of the display where we're about to go FullScreen.
  
    err = CGDisplayCapture (kCGDirectMainDisplay);
    if (err != CGDisplayNoErr) 
	{
        return;
    }
//	
//	KeyedArchive * options = DAVA::Core::GetOptions();
//	int32 width = options->GetInt("fullscreen.width", 800);
//	int32 height = options->GetInt("fullscreen.height", 600);
//	int32 bpp = options->GetInt("bpp", 32);

	DisplayMode fullscreenMode = Core::Instance()->GetCurrentDisplayMode();
	
	int32 width = fullscreenMode.width;
	int32 height = fullscreenMode.height;
	int32 bpp = fullscreenMode.bpp;
	
	CFDictionaryRef displayMode;
	boolean_t exactMatch;
	
	displayMode = CGDisplayBestModeForParameters (kCGDirectMainDisplay, bpp, width, height, &exactMatch);

	if (exactMatch)
	{
		CGDisplaySwitchToMode (kCGDirectMainDisplay,  displayMode);
		Logger::Debug("[CoreMacOSPlatform] switch to %d x %d x %d fullscreen mode", width, height, bpp);
	}else 
	{
		CGDisplaySwitchToMode (kCGDirectMainDisplay,  displayMode);
		Logger::Debug("[CoreMacOSPlatform] switch to closes mode to %d x %d x %d fullscreen mode", width, height, bpp);
	}

    // Pixel Format Attributes for the FullScreen NSOpenGLContext
    NSOpenGLPixelFormatAttribute attrs[] = 
	{
		
        // Specify that we want a full-screen OpenGL context.
        NSOpenGLPFAFullScreen,
		NSOpenGLPFADoubleBuffer,
		
        // We may be on a multi-display system (and each screen may be driven by a different renderer), so we need to specify which screen we want to take over.  For this demo, we'll specify the main screen.
        NSOpenGLPFAScreenMask, CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay),
		
        // Attributes Common to FullScreen and non-FullScreen
        NSOpenGLPFAColorSize, CGDisplayBitsPerPixel(kCGDirectMainDisplay), //24,
        NSOpenGLPFADepthSize, 16,
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFAAccelerated,
        0
    };
    GLint rendererID;
	
    // Create the FullScreen NSOpenGLContext with the attributes listed above.
    NSOpenGLPixelFormat *pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
    
    // Just as a diagnostic, report the renderer ID that this pixel format binds to.  CGLRenderers.h contains a list of known renderers and their corresponding RendererID codes.
    [pixelFormat getValues:&rendererID forAttribute:NSOpenGLPFARendererID forVirtualScreen:0];
    NSLog(@"[CoreMacOSPlatform] fullScreen pixelFormat rendererID = %08x %@", (unsigned)rendererID, openGLView);
	
    // Create an NSOpenGLContext with the FullScreen pixel format.  By specifying the non-FullScreen context as our "shareContext", we automatically inherit all of the textures, display lists, and other OpenGL objects it has defined.
    fullScreenContext = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:[openGLView openGLContext]];
    [pixelFormat release];
    pixelFormat = nil;
	
    if (fullScreenContext == nil) 
	{
        NSLog(@"[CoreMacOSPlatform] failed to create fullScreenContext");
        return;
    }
	
    // Pause animation in the OpenGL view.  While we're in full-screen mode, we'll drive the animation actively instead of using a timer callback.
    if ([self isAnimating]) 
	{
        [self stopAnimationTimer];
    }
	
	// enable vsync
	GLint swapInt = 1;
    [fullScreenContext setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
	
  	
	//if ([[mainWindow screen] isEqual:[[NSScreen screens] objectAtIndex:0]])
	//[NSMenu setMenuBarVisible: NO];
	//CGDisplayHideCursor(kCGDirectMainDisplay);
	//[self becomeFirstResponder];
	
    // Enter FullScreen mode and make our FullScreen context the active context for OpenGL commands.
	//NSLog(@"[CoreMacOSPlatform] failed to create fullScreenContext");
	NSLog(@"[CoreMacOSPlatform] setFullscreen (before)");
	[fullScreenContext setFullScreen];
	NSLog(@"[CoreMacOSPlatform] makeCurrentContext (before)");
    [fullScreenContext makeCurrentContext];
	
	
	
	
	CGLContextObj cglContext;
    long oldSwapInterval;
    long newSwapInterval;
	
    // Save the current swap interval so we can restore it later, and then set the new swap interval to lock us to the display's refresh rate.
    cglContext = CGLGetCurrentContext();
    CGLGetParameter(cglContext, kCGLCPSwapInterval, (GLint*)&oldSwapInterval);
    newSwapInterval = 1;
    CGLSetParameter(cglContext, kCGLCPSwapInterval, (GLint*)&newSwapInterval);
	
    // Tell the scene the dimensions of the area it's going to render to, so it can set up an appropriate viewport and viewing transformation.
//	int32 oldWidth = RenderManager::Instance()->GetScreenWidth();
//	int32 oldHeight = RenderManager::Instance()->GetScreenHeight();	
//	RenderManager::Instance()->Init(RenderManager::ORIENTATION_PORTRAIT, CGDisplayPixelsWide(kCGDirectMainDisplay), CGDisplayPixelsHigh(kCGDirectMainDisplay));
	
	DisplayMode currentMode;
	currentMode.width = GetModeWidth(displayMode);
	currentMode.height = GetModeHeight(displayMode);
	
	NSLog(@"[CoreMacOSPlatform] init internal renderer: %d x %d", currentMode.width, currentMode.height);

	RenderManager::Instance()->Init(currentMode.width, currentMode.height);
	UIControlSystem::Instance()->SetInputScreenAreaSize(currentMode.width, currentMode.height);
	Core::Instance()->SetPhysicalScreenSize(currentMode.width, currentMode.height);
	
	
	RENDER_VERIFY(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    RENDER_VERIFY(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    [fullScreenContext flushBuffer];
	
    // Now that we've got the screen, we enter a loop in which we alternately process input events and computer and render the next frame of our animation.  The shift here is from a model in which we passively receive events handed to us by the AppKit to one in which we are actively driving event processing.
    timeBefore = CFAbsoluteTimeGetCurrent();
    stayInFullScreenMode = YES;
    while (stayInFullScreenMode) 
	{
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
		
	
        // Check for and process input events.
        NSEvent *event;
        while (event = [NSApp nextEventMatchingMask:NSAnyEventMask untilDate:[NSDate distantPast] inMode:NSDefaultRunLoopMode dequeue:YES]) 
		{
            switch ([event type]) 
			{
                case NSLeftMouseDown:
                    [self mouseDown:event];
                    break;
					
                case NSLeftMouseUp:
                    [self mouseUp:event];
                    break;
					
                case NSLeftMouseDragged:
                    [self mouseDragged:event];
                    break;
					
                case NSRightMouseDown:
                    [self rightMouseDown:event];
                    break;
					
                case NSRightMouseUp:
                    [self rightMouseUp:event];
                    break;
					
                case NSRightMouseDragged:
                    [self rightMouseDragged:event];
                    break;
					
                case NSOtherMouseDown:
                    [self rightMouseDown:event];
                    break;
					
                case NSOtherMouseUp:
                    [self rightMouseUp:event];
                    break;
					
                case NSOtherMouseDragged:
                    [self rightMouseDragged:event];
                    break;
					
                case NSMouseMoved:
                    [self mouseMoved:event];
                    break;
					
                case NSKeyDown:
                    //[self keyDown:event];
                    [NSApp sendEvent:event];
                    //[fullscreenWindow keyDown:event];
                    break;
					
                default:
                    [NSApp sendEvent:event];
                    break;
            }
        }
		DAVA::Cursor * activeCursor = RenderManager::Instance()->GetCursor();
		if (activeCursor)
		{
			NSCursor * cursor = (NSCursor*)activeCursor->GetMacOSXCursor();
			[cursor set];
		}

		DAVA::RenderManager::Instance()->Lock();
		Core::Instance()->SystemProcessFrame();
		
		int err = glGetError();
		if (err != GL_NO_ERROR)
		{
			NSLog(@"GL error: %d", err);
		}
		[fullScreenContext flushBuffer];
		
		DAVA::RenderManager::Instance()->Unlock();
		
		// NSLog(@"GL fullscreen frame");
        // Clean up any autoreleased objects that were created this time through the loop.
        [pool release];
    }
    
    // Clear the front and back framebuffers before switching out of FullScreen mode.  (This is not strictly necessary, but avoids an untidy flash of garbage.)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    [fullScreenContext flushBuffer];
	
    // Restore the previously set swap interval.
    CGLSetParameter(cglContext, kCGLCPSwapInterval, (GLint*)&oldSwapInterval);
	
	//[NSMenu setMenuBarVisible: YES];
	//CGDisplayShowCursor(kCGDirectMainDisplay);
	//SetSystemUIMode(kUIModeNormal, 0);

	//if ([[mainWindow screen] isEqual:[[NSScreen screens] objectAtIndex:0]])
	//	{
	//		[NSMenu setMenuBarVisible:YES];
	//	}
	
	
    // Exit fullscreen mode and release our FullScreen NSOpenGLContext.
    [NSOpenGLContext clearCurrentContext];
    [fullScreenContext clearDrawable];
    [fullScreenContext release];
    fullScreenContext = nil;
	
    // Release control of the display.
    CGReleaseAllDisplays();
	
    // Mark our view as needing drawing.  (The animation has advanced while we were in FullScreen mode, so its current contents are stale.)
//  	RenderManager::Instance()->Init(RenderManager::ORIENTATION_PORTRAIT, oldWidth, oldHeight);
	[openGLView setNeedsDisplay:YES];
	[openGLView enableTrackingArea];

    // Resume animation timer firings.
    if ([self isAnimating]) 
	{
        [self startAnimationTimer];
    }
	[self endFullScreen];
}

- (void) keyDown:(NSEvent *)event
{
	[openGLView keyDown:event];

//	unichar c = [[event charactersIgnoringModifiers] characterAtIndex:0];
//	
//	if ([event modifierFlags] & NSCommandKeyMask)
//	{
//		if (c == 'm')
//		{
//			NSLog(@"[CoreMacOSPlatform] keyDown cmd+m");
//			[fullscreenWindow miniaturize:fullscreenWindow];
//		}
//	}
    
//	// TODO: remove that from template
//	switch (c) 
//	{
//		case 27:
//            stayInFullScreenMode = NO;
//            break;
//		default:
//            break;
//    }	
}

- (void)mouseDown:(NSEvent *)theEvent
{
	[openGLView mouseDown:theEvent];
}

- (void)mouseMoved:(NSEvent *)theEvent
{
	//NSLog(@"[CoreMacOSPlatform] mouse moved fullscreen");	
	[openGLView mouseMoved:theEvent];
}

- (void)mouseUp:(NSEvent *)theEvent
{
	[openGLView mouseUp:theEvent];
}

- (void)mouseDragged:(NSEvent *)theEvent
{
	[openGLView mouseDragged:theEvent];
}

- (void)mouseEntered:(NSEvent *)theEvent
{
	NSLog(@"mouse ENTERED");
}
- (void)mouseExited:(NSEvent *)theEvent
{
	NSLog(@"mouse EXITED");
}
- (void)rightMouseDown:(NSEvent *)theEvent
{
	[openGLView rightMouseDown:theEvent];
}

- (void)rightMouseDragged:(NSEvent *)theEvent
{
	[openGLView rightMouseDragged:theEvent];
}

- (void)rightMouseUp:(NSEvent *)theEvent
{
	[openGLView rightMouseUp:theEvent];
}

- (void)otherMouseDown:(NSEvent *)theEvent
{
	[openGLView otherMouseDown:theEvent];
}

- (void)otherMouseDragged:(NSEvent *)theEvent
{
	[openGLView otherMouseDragged:theEvent];
}

- (void)otherMouseUp:(NSEvent *)theEvent
{
	[openGLView otherMouseUp:theEvent];
}


- (BOOL) isInFullScreenMode
{
    return fullScreenContext != nil;
}

- (BOOL) isAnimating
{
    return isAnimating;
}

- (void) startAnimation
{
    if (!isAnimating) 
	{
        isAnimating = YES;
        if (![self isInFullScreenMode]) 
		{
            [self startAnimationTimer];
        }
    }
}

- (void) stopAnimation
{
    if (isAnimating) 
	{
        if (animationTimer != nil) 
		{
            [self stopAnimationTimer];
        }
        isAnimating = NO;
    }
}

- (void) toggleAnimation
{
    if ([self isAnimating]) 
	{
        [self stopAnimation];
    } else {
        [self startAnimation];
    }
}

- (void) startAnimationTimer
{
    if (animationTimer == nil) 
	{
        animationTimer = [[NSTimer scheduledTimerWithTimeInterval:1.0f / currFPS target:self selector:@selector(animationTimerFired:) userInfo:nil repeats:YES] retain];
    }
}

- (void) stopAnimationTimer
{
    if (animationTimer != nil) 
	{
        [animationTimer invalidate];
        [animationTimer release];
        animationTimer = nil;
    }
}

- (void) animationTimerFired:(NSTimer *)timer
{
//	NSLog(@"anim timer fired: %@", openGLView);
    [openGLView setNeedsDisplay:YES];
	if (currFPS != RenderManager::Instance()->GetFPS())
	{
		currFPS = RenderManager::Instance()->GetFPS();
		[self stopAnimationTimer];
		[self startAnimationTimer];
	}
}

- (void) switchFullscreenTimerFired:(NSTimer *)timer
{
	[openGLView disableTrackingArea];
	[self switchToFullScreen];
	[timer invalidate];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	NSLog(@"[CoreMacOSPlatform] Application did finish launching");	
}

- (void)applicationWillFinishLaunching:(NSNotification *)aNotification
{
//	[NSMenu setMenuBarVisible: NO];
//	[NSMenu setMenuBarVisible: YES];
	[self createWindows];
	NSLog(@"[CoreMacOSPlatform] Application will finish launching: %@", [[NSBundle mainBundle] bundlePath]);
	Core::Instance()->SystemAppStarted();
}

- (void)applicationWillBecomeActive:(NSNotification *)aNotification
{
	NSLog(@"[CoreMacOSPlatform] Application will become active");
}

- (void)applicationDidBecomeActive:(NSNotification *)aNotification
{
	NSLog(@"[CoreMacOSPlatform] Application did become active");
    if(core)
    {
        core->OnResume();
    }
    else 
    {
        Core::Instance()->SetIsActive(true);
    }    
    DAVA::Cursor * activeCursor = RenderManager::Instance()->GetCursor();
    if (activeCursor)
    {
        NSCursor * cursor = (NSCursor*)activeCursor->GetMacOSXCursor();
        [cursor set];
    }
}

- (void)applicationDidResignActive:(NSNotification *)aNotification
{
    if(core)
    {
        core->OnSuspend();
    }
    else 
    {
        Core::Instance()->SetIsActive(false);
    }
	NSLog(@"[CoreMacOSPlatform] Application did resign active");
}

- (void)applicationDidChangeScreenParameters:(NSNotification *)aNotification
{
	NSLog(@"[CoreMacOSPlatform] Application did change screen params");
}

- (void)applicationDidHide:(NSNotification *)aNotification
{
	NSLog(@"[CoreMacOSPlatform] Application did hide");
}

- (void)applicationDidUnhide:(NSNotification *)aNotification
{
	NSLog(@"[CoreMacOSPlatform] Application did unhide");
}

- (void)windowWillClose:(NSNotification *)notification
{
	[[NSApplication sharedApplication] terminate: nil];
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
	Core::Instance()->SystemAppFinished();
	
	FrameworkWillTerminate();

	NSLog(@"[CoreMacOSPlatform] Application terminate");
	return NSTerminateNow;
}
@end

namespace DAVA 
{
	
Core::eScreenMode CoreMacOSPlatform::GetScreenMode()
{
        if ([mainWindowController isInFullScreenMode])return Core::MODE_FULLSCREEN;
	else return Core::MODE_WINDOWED;
}
	
void CoreMacOSPlatform::ToggleFullscreen()
{
	if (GetScreenMode() == Core::MODE_FULLSCREEN) // check if we try to switch mode
	{
		SwitchScreenToMode(Core::MODE_WINDOWED);
	}else {
		SwitchScreenToMode(Core::MODE_FULLSCREEN);
	}
}

void CoreMacOSPlatform::SwitchScreenToMode(eScreenMode screenMode)
{
	if (GetScreenMode() != screenMode) // check if we try to switch mode
	{
		if (screenMode == Core::MODE_FULLSCREEN)
		{
			[NSTimer scheduledTimerWithTimeInterval:0.01 target:mainWindowController selector:@selector(switchFullscreenTimerFired:) userInfo:nil repeats:NO];
		}else if (screenMode == Core::MODE_WINDOWED)
		{
			mainWindowController->stayInFullScreenMode = NO;
		}
	}else
	{
		Logger::Debug("[CoreMacOSPlatform::SwitchScreenToMode] Current screen mode is the same as previous. Do nothing");
	}
}

void CoreMacOSPlatform::Quit()
{
	[[NSApplication sharedApplication] terminate: nil];
}
	
};
