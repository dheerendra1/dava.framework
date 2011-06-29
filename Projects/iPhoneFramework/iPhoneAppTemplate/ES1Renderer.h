//
//  ES1Renderer.h
//  KellyGreen
//
//  Created by Hottych on 10/27/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import "ESRenderer.h"

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

@interface ES1Renderer : NSObject <ESRenderer>
{
@private
	EAGLContext *context;
	
	// The pixel dimensions of the CAEAGLLayer
	GLint backingWidth;
	GLint backingHeight;
	
	// The OpenGL names for the framebuffer and renderbuffer used to render to this view
	GLuint defaultFramebuffer, colorRenderbuffer;
	GLuint depthRenderbuffer;
}

- (void) startRendering;
- (void) endRendering;
- (BOOL) resizeFromLayer:(CAEAGLLayer *)layer;

- (void) setCurrentContext;

@end
