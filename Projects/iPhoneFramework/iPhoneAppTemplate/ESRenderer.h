//
//  ESRenderer.h
//  KellyGreen
//
//  Created by Hottych on 10/27/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>

#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>

@protocol ESRenderer <NSObject>

- (void) startRendering;
- (void) endRendering;
- (BOOL) resizeFromLayer:(CAEAGLLayer *)layer;

- (void) setCurrentContext;
@end
