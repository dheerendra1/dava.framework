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
#ifndef __DAVAENGINE_APPLICATION_CORE__
#define __DAVAENGINE_APPLICATION_CORE__

#include "Base/BaseObject.h"

namespace DAVA 
{

/**
	\ingroup core
	\brief Class that defines high-level application logic. 
	This is main class of every application or game that made on our SDK. 
	
	To create your own application you need to perform the following steps:
 
	1. Define FrameworkDidLaunched and FrameworkWillTerminate functions
	\code
	void FrameworkDidLaunched()
	{
		//	Create keyed archive for application options
		KeyedArchive * appOptions = new KeyedArchive();
	#if defined(__DAVAENGINE_IPHONE__)
		// set application base orientation (should be the same as in Info.plist)
		appOptions->SetInt("orientation", Core::SCREEN_ORIENTATION_PORTRAIT);

		// set virtual resolution you want to work in
		DAVA::Core::Instance()->SetVirtualScreenSize(320, 480);
		
		// register resources and their resolutions
		DAVA::Core::Instance()->RegisterAvailableResourceSize(320, 480, "Gfx");
	#else
		// set client area size for the windowed application
		appOptions->SetInt("width",	920);
		appOptions->SetInt("height", 690);

		appOptions->SetInt("fullscreen.width",	1024);
		appOptions->SetInt("fullscreen.height", 768);

		// this flag means that we start from windowed mode
		appOptions->SetInt("fullscreen", 0);
		
		// 32 bits per pixel
		appOptions->SetInt("bpp", 32); 

		DAVA::Core::Instance()->SetVirtualScreenSize(920, 690);
		DAVA::Core::Instance()->RegisterAvailableResourceSize(920, 690, "Gfx");
	#endif 
		
		// create our class that inherited from ApplicationCore
		GameCore * core = new GameCore();
		// set it as application core
		DAVA::Core::SetApplicationCore(core);
		// set launch options
		DAVA::Core::SetOptions(appOptions);
	}

	void FrameworkWillTerminate()
	{


	}
	\endcode
	
	2. Implement all virtual functions of ApplicationCore
	\code
	void GameCore::OnAppStarted()
	{
		RenderManager::Instance()->SetFPS(30);

		mainMenuScreen.Set(new MainMenuScreen());
		gameScreen.Set(new GameScreen());
		winScreen = newref(WinScreen);

		UIScreenManager::Instance()->RegisterScreen(SCREEN_MAIN_MENU, mainMenuScreen.Get());
		UIScreenManager::Instance()->RegisterScreen(SCREEN_GAME, gameScreen.Get());
		UIScreenManager::Instance()->RegisterScreen(SCREEN_WIN, winScreen.Get());

		UIScreenManager::Instance()->SetFirst(SCREEN_GAME);
	}

	void GameCore::OnAppFinished()
	{
		// Release RefPtr pointers to avoid memory leaks
		mainMenuScreen = 0;
		gameScreen = 0;
	}

	void GameCore::OnSuspend()
	{
	}

	void GameCore::OnResume()
	{
	}

	void GameCore::OnBackground()
	{
	}

	void GameCore::BeginFrame()
	{
		ApplicationCore::BeginFrame();
		RenderManager::Instance()->ClearWithColor(0.0f, 0.0f, 0.0f, 0.0f);
	}

	void GameCore::Update(float32 timeElapsed)
	{	
		ApplicationCore::Update(timeElapsed);
	}

	void GameCore::Draw()
	{
		ApplicationCore::Draw();
	}
	\endcode
 */
class ApplicationCore : public BaseObject
{
public:
	
	ApplicationCore();
	virtual ~ApplicationCore();

	/**
		\brief Called when application is suspended or minimized.
		Stops main loop.
	 */
	virtual void OnSuspend();
	
	/**
		\brief Called when application is resumed after suspend or minimization.
		Resumes main loop.
	 */
	virtual void OnResume();

protected:
	/**
		\brief Called immediatelly after application initialized and all singletons are created. 
		This function is second initialization function of your application. First initialization function is FrameworkDidLaunched and it actually allow you to set 
		some important things like your application resolution, set available graphics resources folders and do preliminary initialization.
		This function is more related to your game or application logic. 
		You should overload this function in your GameCore and in overloaded function you should create all application screens or at least one of the screens.
		After creation you should register all of them, and then set first launch screen.
		
		Example: 
		\code
			void GameCore::OnAppStarted()
			{
				RenderManager::Instance()->SetFPS(30);

				mainMenuScreen.Set(new MainMenuScreen());
				gameScreen.Set(new GameScreen());
				winScreen = newref(WinScreen);

				UIScreenManager::Instance()->RegisterScreen(SCREEN_MAIN_MENU, mainMenuScreen.Get());
				UIScreenManager::Instance()->RegisterScreen(SCREEN_GAME, gameScreen.Get());
				UIScreenManager::Instance()->RegisterScreen(SCREEN_WIN, winScreen.Get());

				UIScreenManager::Instance()->SetFirst(SCREEN_GAME);
			}
		\endcode
	 */
	virtual void OnAppStarted() = 0;
	/**
		\brief Called when user requested to quit from application. 
		You should put all deinitialization in this function. Here you can release all objects. 
		Framework can help you to find memory leaks but to use memory leak detection you should release all objects carefully. 
		
		We do not recommend to save game progress in this function, because on some platforms it can create problems. 
		Our recomendation to perform in-game progress saves during the game immediatelly after changes that are important. 
	 */
	virtual void OnAppFinished() = 0;

	
#ifdef __DAVAENGINE_IPHONE__
	/**
		\brief Called when application go to background on iOS platforms
	 */
	virtual void OnBackground() = 0;
#endif
	
	/**	
		\brief this function is called every frame to let you update your application. 
		Normally this function can handle high-level tasks that is common between all application screens. 
		Logic of the particular game screen should be inside that screen and it's Update function. 
	 
		To modify the frequency of Update calls you can use \ref RenderManager::Instance()->SetFPS() function
	 
		\param[in] timeElapsed time in seconds that passed from the previous frame
	 */
	virtual void Update(float32 timeElapsed);
	/**
		\brief Called when application is ready to draw the frame
		In this function you can perform draw. Normally you should draw inside your particular screen but in some cases you can utilize this function when you need to draw something that will work on every screen.
	 */
	virtual void Draw();	
	/**
		\brief Called before draw to let you prepare to the rendering
	 */
	virtual void BeginFrame();
	/**
		\brief Called after draw is finished.
	 */
	virtual void EndFrame();
	
private:

	friend class Core;
};
	
};

#endif