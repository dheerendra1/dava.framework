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
#ifndef __DAVAENGINE_CORE_MAC_OS_CARBON_WINDOW_H__
#define __DAVAENGINE_CORE_MAC_OS_CARBON_WINDOW_H__

#include "Base/BaseTypes.h"

#if defined(__DAVAENGINE_MACOS__)

#include <Carbon/Carbon.h>
#include <AGL/agl.h>
#include <OpenGL/glext.h>
//#include <ppc_intrinsics.h>
/*
namespace DAVA 
{

#define	MAX_VIDEO_MODES		50
#define	INVALID_DISPLAY		(CGDirectDisplayID)-1

class CoreMacOS_CarbonWindow
{
public:	
	
	struct VideoModeType
	{
		int	rezH,rezV;
	};
	
		
	CoreMacOS_CarbonWindow();
	~CoreMacOS_CarbonWindow();
	
	void GetGameNib();
	void CreateGameWindow();
	
	void CreateGLContext();
	void BeginDraw();
	void EndDraw();
	
	int32				gNumVideoModes;
	VideoModeType		gVideoModeList[MAX_VIDEO_MODES];
	
	CGDirectDisplayID	gCGDisplayID;
	int32				gGameWindowWidth, gGameWindowHeight;
	WindowPtr			gGameWindow;
	
	WindowRef			gDialogWindow;
	
	Boolean	gPlayInWindow;
	int32	gDesiredColorDepth;
	int32	gDesiredRezH, gDesiredRezV;
	
	CFBundleRef gBundle;
	IBNibRef 	gNibs;
	
	
	// OpenGL stuff
	AGLDrawable			gAGLWin;
	AGLContext			gAGLContext;
	
	
	
};
}; // namespace DAVA
*/
#endif // #if defined(__DAVAENGINE_MACOS__)
#endif // __DAVAENGINE_CORE_MAC_OS_CARBON_WINDOW_H__