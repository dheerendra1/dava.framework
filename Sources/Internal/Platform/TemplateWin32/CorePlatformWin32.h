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
#ifndef __DAVAENGINE_CORE_PLATFORM_WIN32_H__
#define __DAVAENGINE_CORE_PLATFORM_WIN32_H__

#include "DAVAEngine.h"
#if defined(__DAVAENGINE_WIN32__)

#include "WindowsSpecifics.h"

namespace DAVA 
{
class CoreWin32Platform : public Core
{
public:
	virtual eScreenMode GetScreenMode();
	virtual void SwitchScreenToMode(eScreenMode screenMode); 
	virtual void GetAvailableDisplayModes(List<DisplayMode> & availableModes);

	virtual DisplayMode GetCurrentDisplayMode();

	virtual void Quit();

	virtual bool CreateWin32Window(HINSTANCE hInstance); //true if window created, if false, need to quit the app
	virtual void Run();

	virtual void ToggleFullscreen();

	virtual void SetIcon(int32 iconId);

	void InitOpenGL();
	void ReleaseOpenGL();

	HWND hWindow;
	HDC hDC;
	HGLRC hRC;
	HANDLE hMutex;
#if defined(__DAVAENGINE_DIRECTX9__)
	LPDIRECT3D9 d3d9;
#endif 

/*	int32 screenWidth;
	int32 screenHeight;
	int32 bpp;

	bool isFullscreen;
	bool isInFullscreenNow;	*/

	DisplayMode currentMode;
	DisplayMode fullscreenMode;
	DisplayMode windowedMode;
	bool isFullscreen;
	RECT		windowPositionBeforeFullscreen;
private:
	static const uint32 WINDOWED_STYLE = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	static const uint32 FULLSCREEN_STYLE = WS_VISIBLE | WS_POPUP;


	RECT GetWindowedRectForDisplayMode(DisplayMode & dm);
	bool willQuit;
};	
};
#endif // #if defined(__DAVAENGINE_WIN32__)
#endif // __DAVAENGINE_CORE_PLATFORM_MAC_OS_H__