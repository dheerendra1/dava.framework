/*
 *  CorePlatformAndroid.cpp
 *  TemplateProjectAndroid
 *
 *  Created by Viktor  Kleschenko on 2/18/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#include "Platform/TemplateAndroid/CorePlatformAndroid.h"
#include "Platform/TemplateAndroid/AndroidSpecifics.h"

//#include "Core/Core.h"


#if defined(__DAVAENGINE_ANDROID__)

extern void FrameworkDidLaunched();
extern void FrameworkWillTerminate();

#include "Platform/Thread.h"


namespace DAVA
{
	CoreAndroidPlatform::CoreAndroidPlatform()
		:	Core()
	{
//		Logger::Debug("[CoreAndroidPlatform::CoreAndroidPlatform]");

		wasCreated = false;
		renderIsActive = false;
		oldWidth = 0;
		oldHeight = 0;

		foreground = false;
	}

	int Core::Run(int argc, char * argv[], AppHandle handle)
	{
// 		CoreWin32Platform * core = new CoreWin32Platform();
// 		core->CreateWin32Window(handle);
		// 		core->Run();
// 		core->ReleaseSingletons();
// #ifdef ENABLE_MEMORY_MANAGER
// 		if (DAVA::MemoryManager::Instance() != 0)
// 		{
// 			DAVA::MemoryManager::Instance()->FinalLog();
// 		}
// #endif
		return 0;

	}

	void CoreAndroidPlatform::Quit()
	{
		Logger::Debug("[CoreAndroidPlatform::Quit]");
		QuitAction();
	}

	void CoreAndroidPlatform::QuitAction()
	{
		Logger::Debug("[CoreAndroidPlatform::QuitAction]");

        if(Core::Instance())
        {
            Core::Instance()->SystemAppFinished();
        }

        Logger::Debug("[CoreAndroidPlatform::QuitAction] 1");
        
		FrameworkWillTerminate();

        Logger::Debug("[CoreAndroidPlatform::QuitAction] 2");

#ifdef ENABLE_MEMORY_MANAGER
		if (DAVA::MemoryManager::Instance() != 0)
		{
            Logger::Debug("[CoreAndroidPlatform::QuitAction] 3");
            
			DAVA::MemoryManager::Instance()->FinalLog();
		}
#endif

		Logger::Debug("[CoreAndroidPlatform::QuitAction] done");
	}


	void CoreAndroidPlatform::RepaintView()
	{
// 		int64 ticks = SystemTimer::Instance()->GetTickCount();
// 		Logger::Warning("[CoreAndroidPlatform::RepaintView] %lld", ticks);

        if(renderIsActive)
        {
            DAVA::RenderManager::Instance()->Lock();
            Core::SystemProcessFrame();
            DAVA::RenderManager::Instance()->Unlock();
        }
	}

	void CoreAndroidPlatform::ResizeView(int32 w, int32 h)
	{
//		if(oldWidth != w || oldHeight != h)
		{
			oldWidth = w;
			oldHeight = h;

			windowedMode.width = w;
			windowedMode.height = h;

			UpdateScreenMode();
		}

	}

	void CoreAndroidPlatform::UpdateScreenMode()
	{
        RenderManager *instance = RenderManager::Instance();
		Logger::Debug("[CoreAndroidPlatform::UpdateScreenMode] instance = %p", instance);
        
		UIControlSystem::Instance()->SetInputScreenAreaSize(windowedMode.width, windowedMode.height);
		Core::Instance()->SetPhysicalScreenSize(windowedMode.width, windowedMode.height);

        RenderManager::Instance()->Init(windowedMode.width, windowedMode.height);

        Logger::Debug("[CoreAndroidPlatform::UpdateScreenMode] finish");
    }

	void CoreAndroidPlatform::CreateAndroidWindow(const char8 *docPath, const char8 *assets, const char8 *logTag, AndroidSystemListener * sysListener)
	{
		androidListener = sysListener;

		Core::CreateSingletons();

        Logger::SetTag(logTag);
		Logger::Debug("[CoreAndroidPlatform::CreateAndroidWindow] docpath = %s", docPath);
		Logger::Debug("[CoreAndroidPlatform::CreateAndroidWindow] assets = %s", assets);

		FileSystem::Instance()->SetPath(docPath, assets);

		//////////////////////////////////////////////////////////////////////////
		windowedMode = DisplayMode(480, 320, 16, 0);

//		Logger::Debug("[CoreAndroidPlatform::CreateAndroidWindow] before create rendered");
//        RenderManager::Create(Core::RENDERER_OPENGL_ES_1_0);
//		Logger::Debug("[CoreAndroidPlatform::CreateAndroidWindow] after create rendered");

//        FrameworkDidLaunched();
//        
//		KeyedArchive * options = Core::GetOptions();
//
//		if (options)
//		{
//			windowedMode.width = options->GetInt("width");
//			windowedMode.height = options->GetInt("height");
//			windowedMode.bpp = options->GetInt("bpp");
//		}
//
//		Logger::Debug("[CoreAndroidPlatform::CreateAndroidWindow] w = %d, h = %d", windowedMode.width, windowedMode.height);
//
//
////		RenderManager::Instance()->SetFPS(60);
////
////		UpdateScreenMode();
//
//		//////////////////////////////////////////////////////////////////////////
//		Core::Instance()->SystemAppStarted();
	}
    
    void CoreAndroidPlatform::RenderRecreated()
	{
		Logger::Debug("[CoreAndroidPlatform::RenderRecreated] start");
        
        renderIsActive = true;
        
        if(wasCreated)
		{
            RenderResource::InvalidateAllResources();   
        }
        else
        {
            wasCreated = true;
            
            Logger::Debug("[CoreAndroidPlatform::] before create rendered");
            RenderManager::Create(Core::RENDERER_OPENGL_ES_1_0);
            Logger::Debug("[CoreAndroidPlatform::] after create rendered");
            
            FrameworkDidLaunched();
            
            KeyedArchive * options = Core::GetOptions();
            
            if (options)
            {
                windowedMode.width = options->GetInt("width");
                windowedMode.height = options->GetInt("height");
                windowedMode.bpp = options->GetInt("bpp");
            }
            
            Logger::Debug("[CoreAndroidPlatform::] w = %d, h = %d", windowedMode.width, windowedMode.height);
            
            
            RenderManager::Instance()->SetFPS(60);
            
            UpdateScreenMode();
            
            //////////////////////////////////////////////////////////////////////////
            Core::Instance()->SystemAppStarted();
            
            StartForeground();
        }
        
        Logger::Debug("[CoreAndroidPlatform::RenderRecreated] end");
	}

    
    void CoreAndroidPlatform::OnCreateActivity()
	{
		Logger::Debug("[CoreAndroidPlatform::OnCreateActivity]");
	}


    void CoreAndroidPlatform::OnDestroyActivity()
	{
		Logger::Debug("[CoreAndroidPlatform::OnDestroyActivity]");
        
        renderIsActive = false;
	}


	void CoreAndroidPlatform::StartVisible()
	{
		Logger::Debug("[CoreAndroidPlatform::StartVisible]");

//		Core::Resume();
	}

	void CoreAndroidPlatform::StopVisible()
	{
		Logger::Debug("[CoreAndroidPlatform::StopVisible]");
	}

	void CoreAndroidPlatform::StartForeground()
	{
		Logger::Debug("[CoreAndroidPlatform::StartForeground] start");
		//TODO: VK: add code for handling

        if(wasCreated)
        {
            ApplicationCore * core = Core::Instance()->GetApplicationCore();
            core->OnResume();
            
            foreground = true;
        }
		Logger::Debug("[CoreAndroidPlatform::StartForeground] end");
	}

	void CoreAndroidPlatform::StopForeground()
	{
		Logger::Debug("[CoreAndroidPlatform::StopForeground]");
		//TODO: VK: add code for handling

		RenderResource::SaveAllResourcesToSystemMem();
		RenderResource::LostAllResources();

        ApplicationCore * core = Core::Instance()->GetApplicationCore();
        core->OnSuspend();

		foreground = false;

		oldWidth = 0;
		oldHeight = 0;
	}

	static Vector<DAVA::UIEvent> activeTouches;
	void CoreAndroidPlatform::KeyUp(int keyCode)
	{
		Vector<DAVA::UIEvent> touches;
		Vector<DAVA::UIEvent> emptyTouches;

		for(Vector<DAVA::UIEvent>::iterator it = activeTouches.begin(); it != activeTouches.end(); it++)
		{
			touches.push_back(*it);
		}

		DAVA::UIEvent ev;
		ev.keyChar = (char16)keyCode;
		ev.phase = DAVA::UIEvent::PHASE_KEYCHAR;
		ev.tapCount = 1;
		ev.tid = (int32)keyCode;

		touches.push_back(ev);

		UIControlSystem::Instance()->OnInput(0, emptyTouches, touches);
		touches.pop_back();
		UIControlSystem::Instance()->OnInput(0, emptyTouches, touches);
	}

	void CoreAndroidPlatform::KeyDown(int keyCode)
	{
	}

	int32 MoveTouchsToVector(int action, int id, int x, int y, long time, Vector<UIEvent> *outTouches)
	{
		int tid = id;//DAVA::UIEvent::BUTTON_1;
		int32 phase = DAVA::UIEvent::PHASE_DRAG;

		switch(action)
		{
		case 5:  //ACTION_POINTER_1_DOWN-deprecated
		case 261: //ACTION_POINTER_2_DOWN-deprecated
		case 0: //public static final int ACTION_DOWN = 0;

			phase = DAVA::UIEvent::PHASE_BEGAN;
			break;

		case 262: //ACTION_POINTER_2_UP -deprecated
		case 6: //ACTION_POINTER_1_UP - deprecated
		case 1: //public static final int ACTION_UP = 1;
			phase = DAVA::UIEvent::PHASE_ENDED;
			break;
		case 2: //public static final int ACTION_MOVE = 2;
			phase = DAVA::UIEvent::PHASE_DRAG;
			break;
		case 3: //public static final int ACTION_CANCEL = 3;
			phase = DAVA::UIEvent::PHASE_CANCELLED;
			break;
		case 4: //public static final int ACTION_OUTSIDE = 4;
			break;
		}


		if(phase == UIEvent::PHASE_DRAG)
		{
			for(Vector<DAVA::UIEvent>::iterator it = activeTouches.begin(); it != activeTouches.end(); it++)
			{
				it->physPoint.x = x;//p.x;
				it->physPoint.y = y;//p.y;
				it->phase = phase;
			}
		}

		bool isFind = false;
		for(Vector<DAVA::UIEvent>::iterator it = activeTouches.begin(); it != activeTouches.end(); it++)
		{
			if(it->tid == tid)
			{
				isFind = true;

				it->physPoint.x = x;
				it->physPoint.y = y;
				it->phase = phase;

				break;
			}
		}

		if(!isFind)
		{
			UIEvent newTouch;
			newTouch.tid = tid;
			newTouch.physPoint.x = x;
			newTouch.physPoint.y = y;
			newTouch.phase = phase;

//			Logger::Debug("[MoveTouchsToVector] x is %d, y is %d", x, y);

			activeTouches.push_back(newTouch);
		}

		for(Vector<DAVA::UIEvent>::iterator it = activeTouches.begin(); it != activeTouches.end(); it++)
		{
			outTouches->push_back(*it);
		}

		if(phase == UIEvent::PHASE_ENDED || phase == UIEvent::PHASE_DRAG)
		{
			for(Vector<DAVA::UIEvent>::iterator it = activeTouches.begin(); it != activeTouches.end(); it++)
			{
				if(it->tid == tid)
				{
					activeTouches.erase(it);
					break;
				}
			}
		}
		return phase;
	}

	void CoreAndroidPlatform::OnTouch(int action, int id, int x, int y, long time)
	{
//		Logger::Debug("[CoreAndroidPlatform::OnTouch] action is %d, x is %d, y is %d, time is %d", action, x, y, time);

		Vector<DAVA::UIEvent> touches;
		Vector<DAVA::UIEvent> emptyTouches;

		int32 touchPhase = MoveTouchsToVector(action, id, x, y, time, &touches);

		UIControlSystem::Instance()->OnInput(touchPhase, emptyTouches, touches);

		touches.clear();
	}


	bool CoreAndroidPlatform::DownloadHttpFile(const String & url, const String & documentsPathname)
	{
		if(androidListener)
		{
			String path = FileSystem::Instance()->SystemPathForFrameworkPath(documentsPathname);
			return androidListener->DownloadHttpFile(url, path);
		}

		return false;
	}
}
#endif // #if defined(__DAVAENGINE_ANDROID__)
