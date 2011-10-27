//
//  FullscreenWindow.m
//  Framework
//
//  Created by Dmitry Shpakov on 10/25/11.
//  Copyright 2011 DAVA Consulting. All rights reserved.
//

#import "FullscreenWindow.h"


@implementation FullscreenWindow

- (id)init
{
    self = [super init];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (void)performMiniaturize:(id)sender
{
    NSLog(@"[FullscreenWindow] performMiniaturize");
    [self miniaturize:sender];
}

- (BOOL)canBecomeKeyWindow
{
    return YES;
}

- (void)keyDown:(NSEvent *)event
{
    // your code here
    NSLog(@"[FullscreenWindow] keyDown");

//	unichar c = [[event charactersIgnoringModifiers] characterAtIndex:0];
//	
//	if ([event modifierFlags] & NSCommandKeyMask)
//	{
//		if (c == 'm')
//		{
//			NSLog(@"[FullscreenWindow] keyDown cmd+m");
//			[self miniaturize:self];
//		}
//	}
    
    [super keyDown:event];
}

- (void)keyUp:(NSEvent *)event
{
    // your code here
    [super keyUp:event];
}

@end
