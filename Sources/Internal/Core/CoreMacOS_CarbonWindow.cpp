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
#include "Core/CoreMacOS_CarbonWindow.h"

#if 0

namespace DAVA 
{

CoreMacOS_CarbonWindow::CoreMacOS_CarbonWindow()
{
	gNumVideoModes = 0;
	gCGDisplayID = INVALID_DISPLAY;
	gGameWindowWidth = 0;
	gGameWindowHeight = 0;
	gGameWindow = 0;
	
	gDialogWindow = 0;
	
	gPlayInWindow = true;
	gDesiredColorDepth = 32;
	gDesiredRezH = 1024;
	gDesiredRezV = 768;
	
	gBundle = 0;
	gNibs = 0;
}

CoreMacOS_CarbonWindow::~CoreMacOS_CarbonWindow()
{
	
}

void CoreMacOS_CarbonWindow::GetGameNib(void)
{
	OSStatus	iErr;
	
	gBundle = CFBundleGetMainBundle();
	if (gBundle == nil)
	{
		//	DoFatalAlert("\pCannot locate Main Bundle!");
	}
	iErr = CreateNibReferenceWithCFBundle(gBundle, CFSTR("Game"), &gNibs);			// note:  we don't need the ".nib" extension in the filename
	if (iErr != noErr)
	{
		// DoFatalAlert("\pCreateNibReferenceWithCFBundle failed!");
	}
}

void CoreMacOS_CarbonWindow::CreateGameWindow(void)
{
	int			w, h;
	GDHandle 	gdevice;
	Rect		r;
	
	/* GET MAIN DISPLAY INFO */
	gdevice = GetMainDevice();
	r = (**gdevice).gdRect;						// get Rect of display
	
	/* SIZE WINDOW TO HALF SCREEN SIZE */
	
	w = r.right - r.left;						// get width/height of display
	h = r.bottom - r.top;
	
	r.left		= 0;							// set Rect to size of window that we want
	r.top		= 0;
	r.right		= gGameWindowWidth = w / 2;
	r.bottom	= gGameWindowHeight = h / 2;	
	
	
	/* CREATE THE WINDOW*/
	gGameWindow = NewCWindow(nil, &r, "\p", false, plainDBox, (WindowPtr)-1L, false, 0);
	
	
	/* MOVE WINDOW TO CENTER OF SCREEN */
	MoveWindow(gGameWindow, r.right/2, r.bottom/2, true);
	ShowWindow(gGameWindow);		
}
	
	
// 	
void CoreMacOS_CarbonWindow::CreateGLContext()
{
	GDHandle 		gdevice;
	AGLPixelFormat 	fmt;
	GLint          attribWindow[]	= {AGL_RGBA, AGL_DOUBLEBUFFER, AGL_DEPTH_SIZE, 32, AGL_ALL_RENDERERS, AGL_ACCELERATED, AGL_NO_RECOVERY, AGL_NONE};
	GLint          attribFullScreen[] = {AGL_RGBA, AGL_FULLSCREEN, AGL_DOUBLEBUFFER, AGL_DEPTH_SIZE, 32, AGL_ALL_RENDERERS, AGL_ACCELERATED, AGL_NO_RECOVERY, AGL_NONE};
	
	gdevice = GetMainDevice();						// get the main display
	
	/***********************/
	/* CHOOSE PIXEL FORMAT */
	/***********************/
	
	/* PLAY IN WINDOW */
	
	if (gPlayInWindow)
		fmt = aglChoosePixelFormat(&gdevice, 1, attribWindow);
	
	/* FULL-SCREEN */
	else
		fmt = aglChoosePixelFormat(&gdevice, 1, attribFullScreen);
	
	if ((fmt == nil) || (aglGetError() != AGL_NO_ERROR))
	{
	//	DoFatalAlert("\pCannot make a draw context!");
	}
	/**********************/
	/* CREATE AGL CONTEXT */
	/**********************/
	
	gAGLContext = aglCreateContext(fmt, nil);
	if ((gAGLContext == nil) || (aglGetError() != AGL_NO_ERROR))
	{
		//	DoFatalAlert("\paglCreateContext failed!");
	}
	
	/* PLAYING IN A WINDOW */
	
	if (gPlayInWindow)
	{
		gAGLWin = (AGLDrawable)GetWindowPort(gGameWindow);
		aglSetDrawable(gAGLContext, gAGLWin);
	}
	
	/* PLAYING FULL-SCREEN */
	else
	{
		gAGLWin = nil;
		aglEnable(gAGLContext, AGL_FS_CAPTURE_SINGLE);		
		aglSetFullScreen(gAGLContext, 0, 0, 0, 0);	
	}	
	
	/* ACTIVATE THE CONTEXT */
	aglSetCurrentContext(gAGLContext);
	
	
	/* NO LONGER NEED PIXEL FORMAT */
	aglDestroyPixelFormat(fmt);
}

/******************* DRAW OPENGL SCENE *********************/
//
// Called from the main loop once each frame to draw the scene.
//

void CoreMacOS_CarbonWindow::BeginDraw()
{
	/* MAKE OUR CONTEXT THE ACTIVE ONE */
	aglSetCurrentContext(gAGLContext);
	
	// Set proper viewport
	glViewport(0, 0, gGameWindowWidth, gGameWindowHeight);
}
	
void CoreMacOS_CarbonWindow::EndDraw()
{
	/* END RENDER & SWAP THE BUFFER TO MAKE IT VISIBLE */
	aglSwapBuffers(gAGLContext);		
}

}; // namespace DAVA

#endif // #if 0

