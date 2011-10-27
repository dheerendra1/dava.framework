//
//  FullscreenWindow.h
//  Framework
//
//  Created by Dmitry Shpakov on 10/25/11.
//  Copyright 2011 DAVA Consulting. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface FullscreenWindow : NSWindow 
{
@private
    
}
- (void)performMiniaturize:(id)sender;
- (BOOL)canBecomeKeyWindow;
- (void)keyDown:(NSEvent *)event;
- (void)keyUp:(NSEvent *)event;

@end
