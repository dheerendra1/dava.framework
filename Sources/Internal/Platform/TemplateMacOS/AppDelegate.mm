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
#import "AppDelegate.h"

#if defined(__DAVAENGINE_MACOS__)

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	NSLog(@"applicationDidFinishLaunching: notification");
}

- (void)orderFrontStandardAboutPanel:(id)sender
{
	// [NSApp orderFrontStandardAboutPanel:sender];
}

- (void)unhideAllApplications:(id)sender
{
	// [NSApp unhideAllApplications:sender];
}

- (void)hide:(id)sender
{
	// [NSApp hide:sender];
}

- (void)hideOtherApplications:(id)sender
{
	// [NSApp hideOtherApplications:sender];
}

- (void)terminate:(id)sender
{
	// _quit = TRUE;
}

- (void)windowWillClose:(id)sender
{
//	_quit = TRUE;
}

- (NSSize)windowWillResize:(NSWindow *)window toSize:(NSSize)proposedFrameSize
{
//	if (_device->isResizable())
//		return proposedFrameSize;
//	else
//		return [window frame].size;
	return proposedFrameSize;
}

- (void)windowDidResize:(NSNotification *)aNotification
{
//	NSWindow	*window;
//	NSRect		frame;
//
//	window = [aNotification object];
//	frame = [window frame];
//	_device->setResize((int)frame.size.width,(int)frame.size.height);
}

//- (BOOL)isQuit
//{
//	return (_quit);
//}

@end

#endif // __DAVAENGINE_MACOS__
