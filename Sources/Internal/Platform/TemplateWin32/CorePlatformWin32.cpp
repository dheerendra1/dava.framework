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
#include "Platform/TemplateWin32/CorePlatformWin32.h"
#include "Platform/TemplateWin32/WindowsSpecifics.h"
#include "Platform/Thread.h"

#if defined(__DAVAENGINE_WIN32__)

#include <shellapi.h>

extern void FrameworkDidLaunched();
extern void FrameworkWillTerminate();

namespace DAVA 
{
	int Core::Run(int argc, char * argv[], AppHandle handle)
	{
		CoreWin32Platform * core = new CoreWin32Platform();
		core->CreateSingletons();
		core->CreateWin32Window(handle);
		core->Run();
		core->ReleaseSingletons();
#ifdef ENABLE_MEMORY_MANAGER
		if (DAVA::MemoryManager::Instance() != 0)
		{
			DAVA::MemoryManager::Instance()->FinalLog();
		}
#endif
		return 0;
	
	}

	/*const Vector2 & Core::GetMouseLocation();
	{
		POINT pt;
		GetCursorPos(&pt);
		
	}*/
	
	int Core::RunCmdTool(int argc, char * argv[], AppHandle handle)
	{
		CoreWin32Platform * core = new CoreWin32Platform();

		//core->CreateWin32Window(handle);
		//core->Run();
		core->EnableConsoleMode();
		core->CreateSingletons();

		LPWSTR *szArglist;
		int nArgs;
		int i;
		szArglist = ::CommandLineToArgvW(::GetCommandLineW(), &nArgs);
		if( NULL == szArglist )
		{
			Logger::Error("CommandLineToArgvW failed\n");
			return 0;
		}
		else 
		{
			for( i=0; i<nArgs; i++)
			{
				WideString w = szArglist[i];
				String nonWide = WStringToString(w);
				core->commandLine.push_back(nonWide);
				Logger::Debug("%d: %s\n", i, nonWide.c_str());
			}
		}
		// Free memory allocated for CommandLineToArgvW arguments.
		LocalFree(szArglist);




		FrameworkDidLaunched();
		FrameworkWillTerminate();
		core->ReleaseSingletons();
#ifdef ENABLE_MEMORY_MANAGER
		if (DAVA::MemoryManager::Instance() != 0)
		{
			DAVA::MemoryManager::Instance()->FinalLog();
		}
#endif
		return 0;

	}
	
	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void CoreWin32Platform::CreateWin32Window(HINSTANCE hInstance)
	{	
		windowedMode = DisplayMode(800, 600, 16, 0);
		fullscreenMode = DisplayMode(800, 600, 16, 0);
		currentMode = windowedMode;
		isFullscreen = false;

		// create the window, only if we do not use the null device
		LPCWSTR className = L"DavaFrameworkWindowsDevice";

		// Register Class

		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX); 
		wcex.style			= CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= (WNDPROC)WndProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= hInstance;
		wcex.hIcon			= 0;
		wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszMenuName	= 0;
		wcex.lpszClassName	= className;
		wcex.hIconSm		= 0;

		RegisterClassEx(&wcex);

		// calculate client size

		RECT clientSize;
		clientSize.top = 0;
		clientSize.left = 0;
		clientSize.right = currentMode.width;
		clientSize.bottom = currentMode.height;

		ULONG style = WINDOWED_STYLE;

		// Create the rendering window
		if (isFullscreen)
		{
			style  = WS_VISIBLE | WS_POPUP;
		} // End if Fullscreen


		AdjustWindowRect(&clientSize, style, FALSE);

		int32 realWidth = clientSize.right - clientSize.left;
		int32 realHeight = clientSize.bottom - clientSize.top;

		int32 windowLeft = (GetSystemMetrics(SM_CXSCREEN) - realWidth) / 2;
		int32 windowTop = (GetSystemMetrics(SM_CYSCREEN) - realHeight) / 2;

		if (isFullscreen)
		{
			windowLeft = 0;
			windowTop = 0;
		}

		// create window
		hWindow = CreateWindow( className, L"", style, windowLeft, windowTop, 
			realWidth, realHeight,	NULL, NULL, hInstance, NULL);

		ShowWindow(hWindow, SW_SHOW);
		UpdateWindow(hWindow);

		// fix ugly ATI driver bugs. Thanks to ariaci (Taken from Irrlight).
		MoveWindow(hWindow, windowLeft, windowTop, realWidth, realHeight, TRUE);
	
		RenderManager::Create(Core::RENDERER_DIRECTX9);
		RenderManager::Instance()->Create(hInstance, hWindow);

		FrameworkDidLaunched();
		KeyedArchive * options = Core::GetOptions();

		//fullscreenMode = GetCurrentDisplayMode();
		fullscreenMode = GetCurrentDisplayMode();//FindBestMode(fullscreenMode);
		if (options)
		{
			windowedMode.width = options->GetInt("width");
			windowedMode.height = options->GetInt("height");
			windowedMode.bpp = options->GetInt("bpp");
			
			// get values from config in case if they are available
			fullscreenMode.width = options->GetInt("fullscreen.width", fullscreenMode.width);
			fullscreenMode.height = options->GetInt("fullscreen.height", fullscreenMode.height);
			fullscreenMode.bpp = windowedMode.bpp;

			fullscreenMode = FindBestMode(fullscreenMode);

			isFullscreen = (0 != options->GetInt("fullscreen"));	
			String title = options->GetString("title", "[set application title using core options property 'title']");
			WideString titleW = StringToWString(title);
			SetWindowText(hWindow, titleW.c_str());
		}

		Logger::Info("[PlatformWin32] best display fullscreen mode matched: %d x %d x %d refreshRate: %d", fullscreenMode.width, fullscreenMode.height, fullscreenMode.bpp, fullscreenMode.refreshRate);

		currentMode = windowedMode;
		if (isFullscreen)
		{
			currentMode = fullscreenMode;
		}

		clientSize.top = 0;
		clientSize.left = 0;
		clientSize.right = currentMode.width;
		clientSize.bottom = currentMode.height;

		AdjustWindowRect(&clientSize, style, FALSE);

		realWidth = clientSize.right - clientSize.left;
		realHeight = clientSize.bottom - clientSize.top;

		windowLeft = (GetSystemMetrics(SM_CXSCREEN) - realWidth) / 2;
		windowTop = (GetSystemMetrics(SM_CYSCREEN) - realHeight) / 2;
		MoveWindow(hWindow, windowLeft, windowTop, realWidth, realHeight, TRUE);
	

		RenderManager::Instance()->ChangeDisplayMode(currentMode, isFullscreen);
		RenderManager::Instance()->Init(currentMode.width, currentMode.height);
		UIControlSystem::Instance()->SetInputScreenAreaSize(currentMode.width, currentMode.height);
		Core::Instance()->SetPhysicalScreenSize(currentMode.width, currentMode.height);
	}

	void CoreWin32Platform::Run()
	{
		Core::Instance()->SystemAppStarted();

		MSG msg;
		while(1)
		{
			// process messages
			willQuit = false;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);

				if (msg.message == WM_QUIT)
					willQuit = true;
			}
			Sleep(1);
			RenderManager::Instance()->Lock();
			Core::SystemProcessFrame();
			RenderManager::Instance()->Unlock();

			if (willQuit)
			{	
				break;
			}
		}

		Core::Instance()->SystemAppFinished();
		FrameworkWillTerminate();

		RenderManager::Instance()->Release();
	}

/*	void CoreWin32Platform::InitOpenGL()
	{
		hDC = GetDC(hWindow);

		PIXELFORMATDESCRIPTOR pfd;
		ZeroMemory( &pfd, sizeof( pfd ) );
		pfd.nSize = sizeof( pfd );
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 24;
		pfd.cDepthBits = 16;
		pfd.iLayerType = PFD_MAIN_PLANE;
		int iFormat = ChoosePixelFormat(hDC, &pfd);
		SetPixelFormat(hDC, iFormat, &pfd);

		hRC = wglCreateContext(hDC);
		Thread::secondaryContext = wglCreateContext(hDC);
		Thread::currentDC = hDC;
		
		wglShareLists(Thread::secondaryContext, hRC);
		wglMakeCurrent(hDC, hRC);

		Thread * t = Thread::Create(Message());
		t->EnableCopyContext();
		t->Start();

		glewInit();
		const GLubyte * extensions = glGetString(GL_EXTENSIONS);
		Logger::Debug("[CoreWin32Platform] gl extensions: %s", (const char*)extensions);
	}*/

	void CoreWin32Platform::ReleaseOpenGL()
	{
		wglMakeCurrent(0, 0);
		wglDeleteContext(hRC);
		ReleaseDC(hWindow, hDC);		
	}
	
// 	void CoreWin32Platform::ToggleFullscreen()
// 	{
// 		
// 		
// 	}

/*	void CoreWin32Platform::SwitchToFullScreen() 
	{
// 		DEVMODE dmScreenSettings;
// 
// 		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
// 		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
// 		dmScreenSettings.dmPelsWidth = screenWidth;
// 		dmScreenSettings.dmPelsHeight = screenHeight;
// 		dmScreenSettings.dmBitsPerPel = 32;
// 		dmScreenSettings.dmFields = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
// 
// 		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		LONG dwExStyle = GetWindowLong(hWindow, GWL_EXSTYLE);
		LONG dwStyle = GetWindowLong(hWindow, GWL_STYLE);

		//dwExStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
		//dwStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);

		SetWindowLong(hWindow, GWL_EXSTYLE, dwExStyle);
		SetWindowLong(hWindow, GWL_STYLE, dwStyle);

		SetWindowPos(hWindow, 0, 0, 0, screenWidth, screenHeight, SWP_NOZORDER | SWP_FRAMECHANGED);
		isInFullscreenNow = true;
	}

	void CoreWin32Platform::CloseFullScreen() 
	{
//		ChangeDisplaySettings(NULL, 0);
		ShowCursor(TRUE);

		LONG dwExStyle = GetWindowLong(hWindow, GWL_EXSTYLE);
		LONG dwStyle = GetWindowLong(hWindow, GWL_STYLE);

		//	dwExStyle |= (WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
		//	dwStyle |= (WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
		

		dwExStyle |= WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle |= WS_OVERLAPPEDWINDOW;

		SetWindowLong(hWindow, GWL_EXSTYLE, dwExStyle);
		SetWindowLong(hWindow, GWL_STYLE, dwStyle);

		SetWindowPos(hWindow, HWND_TOP, 0, 0, screenWidth, screenHeight, SWP_FRAMECHANGED);
		isInFullscreenNow = false;
	}
*/
	RECT CoreWin32Platform::GetWindowedRectForDisplayMode(DisplayMode & dm)
	{
		RECT clientSize;
		clientSize.top = 0;
		clientSize.left = 0;
		clientSize.right = dm.width;
		clientSize.bottom = dm.height;

		AdjustWindowRect(&clientSize, GetWindowLong(hWindow, GWL_STYLE), FALSE);

		return clientSize;
	}

	void CoreWin32Platform::ToggleFullscreen()
	{
		// Setup styles based on windowed / fullscreen mode
		isFullscreen = !isFullscreen;

		if ( isFullscreen )
		{
			currentMode = fullscreenMode;
			GetWindowRect(hWindow, &windowPositionBeforeFullscreen);

			SetMenu( hWindow, NULL );
			SetWindowLong( hWindow, GWL_STYLE, FULLSCREEN_STYLE );
			SetWindowPos( hWindow, NULL, 0, 0, currentMode.width, currentMode.height, SWP_NOZORDER );
		} 
		else
		{
			SetWindowLong( hWindow, GWL_STYLE, WINDOWED_STYLE );

			currentMode = windowedMode;
			RECT windowedRect = GetWindowedRectForDisplayMode(currentMode);
	
			SetWindowPos( hWindow, HWND_NOTOPMOST, windowPositionBeforeFullscreen.left, windowPositionBeforeFullscreen.top, windowedRect.right - windowedRect.left, windowedRect.bottom - windowedRect.top, SWP_NOACTIVATE | SWP_SHOWWINDOW );
		}
		
		Logger::Debug("[RenderManagerDX9] toggle mode: %d x %d isFullscreen: %d", currentMode.width, currentMode.height, isFullscreen);

		RenderManager::Instance()->ChangeDisplayMode(currentMode, isFullscreen);
		RenderManager::Instance()->Init(currentMode.width, currentMode.height);
		UIControlSystem::Instance()->SetInputScreenAreaSize(currentMode.width, currentMode.height);
		Core::Instance()->SetPhysicalScreenSize(currentMode.width, currentMode.height);
	}

	Core::eScreenMode CoreWin32Platform::GetScreenMode()
	{
		if (isFullscreen)return Core::MODE_FULLSCREEN;
		else return Core::MODE_WINDOWED;
	}

	void CoreWin32Platform::SwitchScreenToMode(eScreenMode screenMode)
	{
		if (GetScreenMode() != screenMode) // check if we try to switch mode
		{
			if (screenMode == Core::MODE_FULLSCREEN)
			{
				ToggleFullscreen();
			}else if (screenMode == Core::MODE_WINDOWED)
			{
				ToggleFullscreen();
			}
		}else
		{
		}
	}

	void CoreWin32Platform::GetAvailableDisplayModes(List<DisplayMode> & availableDisplayModes)
	{
#if defined(__DAVAENGINE_DIRECTX9__)
		if (RenderManager::Instance()->GetD3D() == 0)
		{
			// 
			Logger::Error("[PlatformWin32] can't get available display modes while RenderManager not initialized");
			return;
		}
		LPDIRECT3D9 direct3D = RenderManager::Instance()->GetD3D();
		availableDisplayModes.clear();

		D3DFORMAT formats[] = {D3DFMT_R5G6B5, D3DFMT_X8R8G8B8}; 

		for (int format = 0; format < sizeof(formats) / sizeof(D3DFORMAT); ++format)
		{
			for (uint32 mode = 0; mode < direct3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, formats[format]); ++mode)
			{
				D3DDISPLAYMODE displayMode;
				HRESULT hr = direct3D->EnumAdapterModes(D3DADAPTER_DEFAULT, formats[format], mode, &displayMode);	
				if (!FAILED(hr))
				{
					DisplayMode mode;
					mode.width = displayMode.Width;
					mode.height = displayMode.Height;
					if (displayMode.Format == D3DFMT_R5G6B5)mode.bpp = 16;
					else if (displayMode.Format == D3DFMT_X8R8G8B8) mode.bpp = 32;
					else if (displayMode.Format == D3DFMT_R8G8B8) mode.bpp = 24;
					mode.refreshRate = displayMode.RefreshRate;
					availableDisplayModes.push_back(mode);

					Logger::Debug("[RenderManagerDX9::GetAvailableDisplayModes] mode found: %d x %d x %d", 
						mode.width,
						mode.height,
						mode.bpp);
				}
			}
		}
#endif 
	}

	DisplayMode CoreWin32Platform::GetCurrentDisplayMode()
	{
#if defined(__DAVAENGINE_DIRECTX9__)
		LPDIRECT3D9 direct3D = RenderManager::Instance()->GetD3D();
		D3DDISPLAYMODE displayMode;
		HRESULT hr = direct3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode);
		if (!FAILED(hr))
		{
			DisplayMode mode;
			mode.width = displayMode.Width;
			mode.height = displayMode.Height;
			if (displayMode.Format == D3DFMT_R5G6B5)mode.bpp = 16;
			else if (displayMode.Format == D3DFMT_X8R8G8B8) mode.bpp = 32;
			else if (displayMode.Format == D3DFMT_R8G8B8) mode.bpp = 24;
			mode.refreshRate = displayMode.RefreshRate;
			return mode;
		}
		return DisplayMode();
#endif
		return DisplayMode();
	}


	void CoreWin32Platform::Quit()
	{
		willQuit = true;
	}

	void CoreWin32Platform::SetIcon(int32 iconId)
	{
		HINSTANCE hInst= GetModuleHandle(0);
		HICON smallIcon = static_cast<HICON>(LoadImage(hInst,
			MAKEINTRESOURCE(iconId),
			IMAGE_ICON,
			0,
			0,
			LR_DEFAULTSIZE));
		SendMessage(hWindow, WM_SETICON, ICON_SMALL, (LPARAM)smallIcon);
	}

	static Vector<DAVA::UIEvent> activeTouches;
	int32 MoveTouchsToVector(UINT message, WPARAM wParam, LPARAM lParam, Vector<UIEvent> *outTouches)
	{
		
		int button = 0;
		//	case WM_LBUTTONDOWN:
		//	case WM_RBUTTONDOWN:
		//	case WM_MBUTTONDOWN:
		//	case WM_XBUTTONDOWN:
		//		
		//	case WM_LBUTTONUP:
		//	case WM_RBUTTONUP:
		//	case WM_MBUTTONUP:
		//	case WM_XBUTTONUP:
		//		
		//	case WM_MOUSEMOVE:
		if(message == WM_LBUTTONDOWN || message == WM_LBUTTONUP || message == WM_MOUSEMOVE)
		{
			button = 1;
		}
		else if(message == WM_RBUTTONDOWN || message == WM_RBUTTONUP)
		{
			button = 2;
		}
		else if(message == WM_MBUTTONDOWN || message == WM_MBUTTONUP)
		{
			button = 3;
		}
// 		else if(message == WM_XBUTTONDOWN || message == WM_XBUTTONUP)
// 		{
// 			if(HIWORD(wParam) == XBUTTON1)
// 			{
// 				button = 4;
// 			}
// 			else 
// 			{
// 				button = 5;
// 			}
// 
// 		}

		//	NSLog(@"Event button %d", button);

		int phase = UIEvent::PHASE_MOVE;
		if(message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN || message == WM_MBUTTONDOWN /*|| message == WM_XBUTTONDOWN*/)
		{
			phase = UIEvent::PHASE_BEGAN;
			//		NSLog(@"Event phase PHASE_BEGAN");
		}
		else if(message == WM_LBUTTONUP || message == WM_RBUTTONUP || message == WM_MBUTTONUP /*|| message == WM_XBUTTONUP*/)
		{
			phase = UIEvent::PHASE_ENDED;
			//		NSLog(@"Event phase PHASE_ENDED");
		}
		else if(LOWORD(wParam)&MK_LBUTTON || LOWORD(wParam)&MK_RBUTTON || LOWORD(wParam)&MK_MBUTTON /*|| LOWORD(wParam)&MK_XBUTTON1 || LOWORD(wParam)&MK_XBUTTON2*/)
		{
			phase = UIEvent::PHASE_DRAG;
		}

		if(phase == UIEvent::PHASE_DRAG)
		{
			for(Vector<DAVA::UIEvent>::iterator it = activeTouches.begin(); it != activeTouches.end(); it++)
			{
				Vector2 p((float32)GET_X_LPARAM(lParam), (float32)GET_Y_LPARAM(lParam));

				it->physPoint.x = p.x;
				it->physPoint.y = p.y;
				it->phase = phase;
			}
		}

		bool isFind = false;
		for(Vector<DAVA::UIEvent>::iterator it = activeTouches.begin(); it != activeTouches.end(); it++)
		{
			if(it->tid == button)
			{
				isFind = true;

				Vector2 p((float32)GET_X_LPARAM(lParam), (float32)GET_Y_LPARAM(lParam));

				it->physPoint.x = p.x;
				it->physPoint.y = p.y;
				it->phase = phase;
				//				it->timestamp = curEvent.timestamp;
				//				it->tapCount = curEvent.clickCount;
				it->phase = phase;

				break;
			}
		}

		if(!isFind)
		{
			UIEvent newTouch;
			newTouch.tid = button;
			Vector2 p((float32)GET_X_LPARAM(lParam), (float32)GET_Y_LPARAM(lParam));
			newTouch.physPoint.x = p.x;
			newTouch.physPoint.y = p.y;
			//			newTouch.timestamp = curEvent.timestamp;
			//			newTouch.tapCount = curEvent.clickCount;
			newTouch.phase = phase;
			activeTouches.push_back(newTouch);
		}

		for(Vector<DAVA::UIEvent>::iterator it = activeTouches.begin(); it != activeTouches.end(); it++)
		{
			outTouches->push_back(*it);
		}

		if(phase == UIEvent::PHASE_ENDED || phase == UIEvent::PHASE_MOVE)
		{
			for(Vector<DAVA::UIEvent>::iterator it = activeTouches.begin(); it != activeTouches.end(); it++)
			{
				if(it->tid == button)
				{
					activeTouches.erase(it);
					break;
				}
			}
		}
		return phase;
	}


	//static bool mouseShow = false;
	static bool mouseCursorShown = true;

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A
#endif
#ifndef WHEEL_DELTA                     
#define WHEEL_DELTA 120
#endif

		//Event event;
		//event.MouseEvent.RelativeX = 0;
		//event.MouseEvent.RelativeY = 0;

		switch (message) 
		{
			//case WM_PAINT:
			case WM_ERASEBKGND:
				return 0;

			/*case WM_PAINT:
			{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			}
			return 0;

			case WM_ERASEBKGND:
			return 0;

			case WM_SETCURSOR: 
			envm = getEnvMapperFromHWnd(hWnd);
			if (envm &&	!envm->irrDev->getWin32CursorControl()->isVisible())
			{
			SetCursor(NULL); 
			return 0; 
			}
			break;

			case WM_MOUSEWHEEL: 
			event.EventType = irr::EET_MOUSE_INPUT_EVENT; 
			event.MouseInput.Wheel = (float)((short)HIWORD(wParam)) / (float)WHEEL_DELTA;
			event.MouseInput.Event = irr::EMIE_MOUSE_WHEEL;
			event.MouseInput.X = LOWORD(lParam); 
			event.MouseInput.Y = HIWORD(lParam); 

			dev = getDeviceFromHWnd(hWnd); 
			if (dev) 
			dev->postEventFromUser(event); 
			break;
			*/
			/*
			case WM_LBUTTONDOWN:
				event.EventType = irr::EET_MOUSE_INPUT_EVENT;
				event.MouseInput.Event = irr::EMIE_LMOUSE_PRESSED_DOWN;
				event.MouseInput.X = LOWORD(lParam);
				event.MouseInput.Y = HIWORD(lParam);
				dev = getDeviceFromHWnd(hWnd);
				if (dev)
					dev->postEventFromUser(event);
			return 0;

			case WM_LBUTTONUP:
			event.EventType = irr::EET_MOUSE_INPUT_EVENT;
			event.MouseInput.Event = irr::EMIE_LMOUSE_LEFT_UP;
			event.MouseInput.X = LOWORD(lParam);
			event.MouseInput.Y = HIWORD(lParam);
			dev = getDeviceFromHWnd(hWnd);
			if (dev)
			dev->postEventFromUser(event);
			return 0;

			case WM_RBUTTONDOWN:
			event.EventType = irr::EET_MOUSE_INPUT_EVENT;
			event.MouseInput.Event = irr::EMIE_RMOUSE_PRESSED_DOWN;
			event.MouseInput.X = LOWORD(lParam);
			event.MouseInput.Y = HIWORD(lParam);
			dev = getDeviceFromHWnd(hWnd);
			if (dev)
			dev->postEventFromUser(event);
			return 0;

			case WM_RBUTTONUP:
			event.EventType = irr::EET_MOUSE_INPUT_EVENT;
			event.MouseInput.Event = irr::EMIE_RMOUSE_LEFT_UP;
			event.MouseInput.X = LOWORD(lParam);
			event.MouseInput.Y = HIWORD(lParam);
			dev = getDeviceFromHWnd(hWnd);
			if (dev)
			dev->postEventFromUser(event);
			return 0;

			case WM_MBUTTONDOWN:
			event.EventType = irr::EET_MOUSE_INPUT_EVENT;
			event.MouseInput.Event = irr::EMIE_MMOUSE_PRESSED_DOWN;
			event.MouseInput.X = LOWORD(lParam);
			event.MouseInput.Y = HIWORD(lParam);
			dev = getDeviceFromHWnd(hWnd);
			if (dev)
			dev->postEventFromUser(event);
			return 0;

			case WM_MBUTTONUP:
			event.EventType = irr::EET_MOUSE_INPUT_EVENT;
			event.MouseInput.Event = irr::EMIE_MMOUSE_LEFT_UP;
			event.MouseInput.X = LOWORD(lParam);
			event.MouseInput.Y = HIWORD(lParam);
			dev = getDeviceFromHWnd(hWnd);
			if (dev)
			dev->postEventFromUser(event);
			return 0;

			case WM_MOUSEMOVE:
			event.EventType = irr::EET_MOUSE_INPUT_EVENT;
			event.MouseInput.Event = irr::EMIE_MOUSE_MOVED;
			event.MouseInput.X = LOWORD(lParam);
			event.MouseInput.Y = HIWORD(lParam);
			dev = getDeviceFromHWnd(hWnd);
			if (dev)
			dev->postEventFromUser(event);
			return 0;

			case WM_KEYDOWN:
			{
			event.EventType = irr::EET_KEY_INPUT_EVENT;
			event.KeyInput.Key = (irr::EKEY_CODE)wParam;
			event.KeyInput.PressedDown = true;
			dev = getDeviceFromHWnd(hWnd);

			BYTE allKeys[256];
			WORD KeyAsc=0;
			GetKeyboardState(allKeys);
			ToAscii(wParam,lParam,allKeys,&KeyAsc,0);

			event.KeyInput.Shift = ((allKeys[VK_SHIFT] & 0x80)!=0);
			event.KeyInput.Control = ((allKeys[VK_CONTROL] & 0x80)!=0);
			event.KeyInput.Char = KeyAsc; //KeyAsc >= 0 ? KeyAsc : 0;

			if (dev)
			dev->postEventFromUser(event);

			return 0;
			}
			case WM_KEYUP:
			{
			event.EventType = irr::EET_KEY_INPUT_EVENT;
			event.KeyInput.Key = (irr::EKEY_CODE)wParam;
			event.KeyInput.PressedDown = false;
			dev = getDeviceFromHWnd(hWnd);

			BYTE allKeys[256];
			WORD KeyAsc=0;
			GetKeyboardState(allKeys);
			ToAscii(wParam,lParam,allKeys,&KeyAsc,0);

			event.KeyInput.Shift = ((allKeys[VK_SHIFT] & 0x80)!=0);
			event.KeyInput.Control = ((allKeys[VK_CONTROL] & 0x80)!=0);
			event.KeyInput.Char = KeyAsc; //KeyAsc >= 0 ? KeyAsc : 0;

			if (dev)
			dev->postEventFromUser(event);

			return 0;
			}

			case WM_SIZE:
			{
			// resize
			dev = getDeviceFromHWnd(hWnd);
			if (dev)
			dev->OnResized();
			}
			return 0;
			*/
		case WM_KEYDOWN:
			{
				BYTE allKeys[256];
				GetKeyboardState(allKeys);
	
				if ((allKeys[VK_MENU] & 0x80)
					&& (allKeys[VK_TAB] & 0x80))
				{
					ShowWindow(hWnd, SW_MINIMIZE);
				}

			};
			break;
		case WM_CHAR:
			{
				//Logger::Debug("wm_char");
				Vector<DAVA::UIEvent> touches;
				Vector<DAVA::UIEvent> emptyTouches;

				for(Vector<DAVA::UIEvent>::iterator it = activeTouches.begin(); it != activeTouches.end(); it++)
				{
					touches.push_back(*it);
				}

				DAVA::UIEvent ev;
				ev.keyChar = (char16)wParam;
				ev.phase = DAVA::UIEvent::PHASE_KEYCHAR;
				ev.tapCount = 1;
				ev.tid = (int32)wParam;

				touches.push_back(ev);

				//if ((ev.keyChar == 'f') /*&& (GetKeyState(VK_LSHIFT) & 0x8000)*/)
				//{
				//	Core::Instance()->ToggleFullscreen();
				//}

				UIControlSystem::Instance()->OnInput(0, emptyTouches, touches);
				touches.pop_back();
				UIControlSystem::Instance()->OnInput(0, emptyTouches, touches);
			}
			break;


		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
//		case WM_XBUTTONDOWN:

		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
//		case WM_XBUTTONUP:

		case WM_MOUSEMOVE:
			{
			
				//Logger::Debug("ms: %d %d", GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

				Vector<DAVA::UIEvent> touches;
				Vector<DAVA::UIEvent> emptyTouches;

				int32 touchPhase = MoveTouchsToVector(message, wParam, lParam, &touches);

				UIControlSystem::Instance()->OnInput(touchPhase, emptyTouches, touches);

				touches.clear();
			}
// 			if (mouseShow)
// 			{
// 				ShowCursor(false);
// 				mouseShow = false;
// 			}
			if (RenderManager::Instance()->GetCursor() != 0 && mouseCursorShown)
			{
				ShowCursor(false);
				mouseCursorShown = false;
			}
			if (RenderManager::Instance()->GetCursor() == 0 && !mouseCursorShown)			
			{
				ShowCursor(false);
				mouseCursorShown = false;
			}
			break;

		case WM_NCMOUSEMOVE:
			if (!mouseCursorShown)
			{	
				ShowCursor(true);
				mouseCursorShown = true;
			}
			break;

		case WM_NCMOUSELEAVE:
			//ShowCursor(false);
			break;

			//case WM_KEYDOWN:
			//	{
			//		event.Type = EET_KEYBOARDEVENT;
			//		event.KeyboardEvent.Key = (Log::eKeyCode)wParam;
			//		event.KeyboardEvent.Type = EKET_KEYDOWN;

			//		BYTE allKeys[256];
			//		WORD KeyAsc=0;
			//		GetKeyboardState(allKeys);
			//		ToAscii((UINT )wParam,(UINT )lParam,allKeys,&KeyAsc,0);

			//		//event.KeyboardEvent.Shift = ((allKeys[VK_SHIFT] & 0x80)!=0);
			//		//event.KeyboardEvent.Control = ((allKeys[VK_CONTROL] & 0x80)!=0);
			//		//event.KeyboardEvent.Char = KeyAsc; //KeyAsc >= 0 ? KeyAsc : 0;

			//		if (Global::engineDevice)
			//			((WindowsEngineSystem *)Global::engineDevice)->PostEventFromUser(event);

			//		return 0;
			//	}
			//case WM_KEYUP:
			//	{
			//		event.Type = EET_KEYBOARDEVENT;
			//		event.KeyboardEvent.Key = (Log::eKeyCode)wParam;
			//		event.KeyboardEvent.Type = EKET_KEYUP;

			//		BYTE allKeys[256];
			//		WORD KeyAsc=0;
			//		GetKeyboardState(allKeys);
			//		ToAscii((UINT )wParam,(UINT )lParam,allKeys,&KeyAsc,0);

			//		//event.KeyInput.Shift = ((allKeys[VK_SHIFT] & 0x80)!=0);
			//		//event.KeyInput.Control = ((allKeys[VK_CONTROL] & 0x80)!=0);
			//		//event.KeyInput.Char = KeyAsc; //KeyAsc >= 0 ? KeyAsc : 0;

			//		if (Global::engineDevice)
			//			((WindowsEngineSystem *)Global::engineDevice)->PostEventFromUser(event);

			//		return 0;
			//	}

			//case WM_MBUTTONDOWN:
			//	event.Type = Log::EET_MOUSEEVENT;
			//	event.MouseEvent.Type = Log::EMET_MBUTTONDOWN;
			//	event.MouseEvent.X = LOWORD(lParam);
			//	event.MouseEvent.Y = HIWORD(lParam);
			//	if (Global::engineDevice)
			//		((WindowsEngineSystem *)Global::engineDevice)->PostEventFromUser(event);
			//	
			//	return 0;

			//case WM_MBUTTONUP:
			//	event.Type = Log::EET_MOUSEEVENT;
			//	event.MouseEvent.Type = Log::EMET_MBUTTONUP;
			//	event.MouseEvent.X = LOWORD(lParam);
			//	event.MouseEvent.Y = HIWORD(lParam);

			//	if (Global::engineDevice)
			//		((WindowsEngineSystem *)Global::engineDevice)->PostEventFromUser(event);
			//	return 0;

			//case WM_RBUTTONDOWN:
			//	event.Type = Log::EET_MOUSEEVENT;
			//	event.MouseEvent.Type = Log::EMET_RBUTTONDOWN;
			//	event.MouseEvent.X = LOWORD(lParam);
			//	event.MouseEvent.Y = HIWORD(lParam);
			//	if (Global::engineDevice)
			//		((WindowsEngineSystem *)Global::engineDevice)->PostEventFromUser(event);
			//	
			//	return 0;

			//case WM_RBUTTONUP:
			//	event.Type = Log::EET_MOUSEEVENT;
			//	event.MouseEvent.Type = Log::EMET_RBUTTONUP;
			//	event.MouseEvent.X = LOWORD(lParam);
			//	event.MouseEvent.Y = HIWORD(lParam);
			//	
			//	if (Global::engineDevice)
			//		((WindowsEngineSystem *)Global::engineDevice)->PostEventFromUser(event);
			//	return 0;

			//case WM_LBUTTONDOWN:
			//	event.Type = Log::EET_MOUSEEVENT;
			//	event.MouseEvent.Type = Log::EMET_LBUTTONDOWN;
			//	event.MouseEvent.X = LOWORD(lParam);
			//	event.MouseEvent.Y = HIWORD(lParam);
			//	if (Global::engineDevice)
			//		((WindowsEngineSystem *)Global::engineDevice)->PostEventFromUser(event);
			//	
			//	return 0;

			//case WM_LBUTTONUP:
			//	event.Type = Log::EET_MOUSEEVENT;
			//	event.MouseEvent.Type = Log::EMET_LBUTTONUP;
			//	event.MouseEvent.X = LOWORD(lParam);
			//	event.MouseEvent.Y = HIWORD(lParam);


			//	if (Global::engineDevice)
			//		((WindowsEngineSystem *)Global::engineDevice)->PostEventFromUser(event);
			//	return 0;
			//	
			//case WM_MOUSEMOVE:
			//	event.Type = Log::EET_MOUSEEVENT;
			//	event.MouseEvent.Type = Log::EMET_MOUSEMOVE;
			//	event.MouseEvent.X = LOWORD(lParam);
			//	event.MouseEvent.Y = HIWORD(lParam);
			//	event.MouseEvent.RelativeX = event.MouseEvent.X - MousePreviousX;
			//	event.MouseEvent.RelativeY = event.MouseEvent.Y - MousePreviousY;
			//	
			//	MousePreviousX = event.MouseEvent.X;
			//	MousePreviousY = event.MouseEvent.Y;

			//	if (Global::engineDevice)
			//		((WindowsEngineSystem *)Global::engineDevice)->PostEventFromUser(event);
			//	return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_ACTIVATE:
			{
				KeyedArchive* options = DAVA::Core::GetOptions();
				if(options->GetBool("suspendOnDeactivate",true))
				{
					WORD loWord = LOWORD(wParam);
					WORD hiWord = HIWORD(wParam);
					if(!loWord || hiWord)
					{
						Logger::Debug("[PlatformWin32] deactivate application");
						RenderResource::SaveAllResourcesToSystemMem();
						Core::Instance()->Suspend();
					}
					else
					{
						Logger::Debug("[PlatformWin32] activate application");
						Core::Instance()->Resume();
					}
				}
			};
			break;
		case WM_SYSCOMMAND:
			// prevent screensaver or monitor powersave mode from starting
			if (wParam == SC_SCREENSAVE ||
				wParam == SC_MONITORPOWER)
				return 0;
			break;
		}

		return DefWindowProc(hWnd, message, wParam, lParam);
	}


	
}
#endif // #if defined(__DAVAENGINE_WIN32__)