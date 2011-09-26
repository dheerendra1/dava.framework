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
#include "FileSystem/FileSystem.h"
#include "Base/ObjectFactory.h"
#include "Core/ApplicationCore.h"
#include "Core/Core.h"
#include "Render/RenderManager.h"
#include "Platform/SystemTimer.h"
#include "Platform/Thread.h"
#include "UI/UIScreenManager.h"
#include "UI/UIControlSystem.h"
#include "Debug/DVAssert.h"
#include "Render/2D/TextBlock.h"
#include "Debug/Replay.h"
#include "Sound/SoundSystem.h"
#include "Sound/Sound.h"


#if defined(__DAVAENGINE_IPHONE__)
#include "Input/AccelerometeriPhone.h"
#endif 

namespace DAVA 
{

#if defined(__DAVAENGINE_IPHONE__)
	static bool useAutodetectContentScaleFactor = false;
#endif 

	float Core::virtualToPhysical = 0;
	float Core::physicalToVirtual = 0;
	Vector2 Core::drawOffset;

Core::Core()
{
	globalFrameIndex = 1;
	isActive = false;
	firstRun = true;
	isConsoleMode = false;
}

Core::~Core()
{
}

static KeyedArchive * options = 0;

void Core::CreateSingletons()
{
#ifndef __DAVAENGINE_IPHONE__
//	if (!Core::Instance())
//	{
//		//Logger::Warning("[Core::Create] failed / something wrong with template or your platform code / contact framework developers");
//	}
#endif
    
    
    // check types size
    
	options = new KeyedArchive();
	new FileSystem();
	new Logger();
	if (isConsoleMode)
	{
		/*
			Disable all debug initialization messages in console mode
		 */
		Logger::Instance()->SetLogLevel(Logger::LEVEL_INFO);
	}
	Logger::Debug("[Core::Create] successfull");

	new LocalizationSystem();

	new SystemTimer();
	new Random();
	new AnimationManager();
	new FontManager();
	new UIControlSystem();
	new SoundSystem(64);
	
#ifdef __DAVAENGINE_IPHONE__
	new AccelerometeriPhoneImpl();
#endif
	
	new UIScreenManager();

#ifdef __DAVAENGINE_WIN32__
	Thread::InitMainThread();
#endif
    
    CheckDataTypeSizes();
}

// We do not create RenderManager until we know which version of render manager we want to create
void Core::CreateRenderManager()
{
    KeyedArchive * options = GetOptions();
    eRenderer renderer = (eRenderer)options->GetInt("renderer");
    
    RenderManager::Create(renderer);
}
        
void Core::ReleaseSingletons()
{
	UIScreenManager::Instance()->Release();
	UIControlSystem::Instance()->Release();
	SoundSystem::Instance()->Release();
	FontManager::Instance()->Release();
	AnimationManager::Instance()->Release();
	SystemTimer::Instance()->Release();
#ifdef __DAVAENGINE_IPHONE__
	Accelerometer::Instance()->Release();
	//SoundSystem::Instance()->Release();
#endif
	//RenderManager::Instance()->Release();
	LocalizationSystem::Instance()->Release();
	Logger::Debug("[Core::Release] successfull");
	FileSystem::Instance()->Release();
	Random::Instance()->Release();
}

void Core::SetOptions(KeyedArchive * archiveOfOptions)
{
	SafeRelease(options);
	options = SafeRetain(archiveOfOptions);
	if (options) 
	{
#if defined(__DAVAENGINE_IPHONE__)
		useAutodetectContentScaleFactor = options->GetBool("iPhone_autodetectScreenScaleFactor", false);
#endif 
	}
	
	Instance()->screenOrientation = options->GetInt("orientation", SCREEN_ORIENTATION_PORTRAIT);
	
}
    
void Core::CheckDataTypeSizes()
{
    CheckType(int8(), 8, "int8");
    CheckType(uint8(), 8, "uint8");
    CheckType(int16(), 16, "int16");
    CheckType(uint16(), 16, "uint16");
    CheckType(int32(), 32, "int32");
    CheckType(uint32(), 32, "uint32");
}

template <class T> void Core::CheckType(T t, int32 expectedSize, const char * typeString)
{
    if ((sizeof(t) * 8) != expectedSize)
    {
        Logger::Error("Size of %s is incorrect. Expected size: %d. Platform size: %d", typeString, expectedSize, sizeof(t));
    }
}

KeyedArchive * Core::GetOptions()
{
	return options;
}
	
#if defined(__DAVAENGINE_IPHONE__)
bool Core::IsAutodetectContentScaleFactor()
{
	return useAutodetectContentScaleFactor;
}
#endif 

//void Core::SetContentScaleFactor(float scaleFactor)
//{
//	Logger::Info("Sets content scale to %1.2f", scaleFactor);
//	contentScaleFactor = scaleFactor;
//	contentInverseScaleFactor = 1.0f / scaleFactor;
//}
//
//float Core::GetContentScaleFactor()
//{
//	return contentScaleFactor;
//}
//	
//float Core::GetInverseContentScaleFactor()
//{
//	return contentInverseScaleFactor;
//}

	
float32 Core::GetVirtualToPhysicalFactor()
{
	return virtualToPhysical;
}
	
float32 Core::GetPhysicalToVirtualFactor()
{
	return physicalToVirtual;
}

Core::eScreenOrientation Core::GetScreenOrientation()
{
	return (Core::eScreenOrientation)screenOrientation;
}

void Core::CalculateScaleMultipliers()
{
	needTorecalculateMultipliers = false;
	UIControlSystem::Instance()->CalculateScaleMultipliers();
	float width, height;
	if (screenOrientation == SCREEN_ORIENTATION_PORTRAIT || screenOrientation == SCREEN_ORIENTATION_PORTRAIT_UPSIDE_DOWN)
	{
		width = screenWidth;
		height = screenHeight;
	}
	else 
	{
		height = screenWidth;
		width = screenHeight;
	}
	rotatedScreenWidth = width;
	rotatedScreenHeight = height;

	float32 w, h;
	w = (float32)virtualScreenWidth / (float32)width;
	h = (float32)virtualScreenHeight / (float32)height;
	drawOffset.x = drawOffset.y = 0;
	float32 desD = 10000.0f;
	if(w > h)
	{
		physicalToVirtual = w;
		virtualToPhysical = (float32)width / (float32)virtualScreenWidth;
		drawOffset.y = 0.5f * ((float32)height - (float32)Core::Instance()->GetVirtualScreenHeight() * virtualToPhysical);
		for (int i = 0; i < (int)allowedSizes.size(); i++) 
		{
			allowedSizes[i].toVirtual = (float32)virtualScreenWidth / (float32)allowedSizes[i].width;
			allowedSizes[i].toPhysical = (float32)width / (float32)allowedSizes[i].width;
			if (fabs(allowedSizes[i].toPhysical - 1.0f) < desD) 
			{
				desD = fabsf(allowedSizes[i].toPhysical - 1.0f);
				desirableIndex = i;
			}
		}
	}
	else
	{
		physicalToVirtual = h;
		virtualToPhysical = (float32)height / (float32)virtualScreenHeight;
		drawOffset.x = 0.5f * ((float32)width - (float32)Core::Instance()->GetVirtualScreenWidth() * virtualToPhysical);
		for (int i = 0; i < (int)allowedSizes.size(); i++) 
		{
			allowedSizes[i].toVirtual = (float32)virtualScreenHeight / (float32)allowedSizes[i].height;
			allowedSizes[i].toPhysical = (float32)height / (float32)allowedSizes[i].height;
			if (fabs(allowedSizes[i].toPhysical - 1.0f) < desD) 
			{
				desD = fabsf(allowedSizes[i].toPhysical - 1.0f);
				desirableIndex = i;
			}
		}
	}
	
	drawOffset.y = floorf(drawOffset.y);
	drawOffset.x = floorf(drawOffset.x);
	
	Sprite::ValidateForSize();
	TextBlock::ScreenResolutionChanged();
	
}
	
float32 Core::GetResourceToPhysicalFactor(int32 resourceIndex)
{
	DVASSERT(resourceIndex < (int32)allowedSizes.size());
	return allowedSizes[resourceIndex].toPhysical;
}
float32 Core::GetResourceToVirtualFactor(int32 resourceIndex)
{
	DVASSERT(resourceIndex < (int32)allowedSizes.size());
	return allowedSizes[resourceIndex].toVirtual;
}
const String& Core::GetResourceFolder(int32 resourceIndex)
{
	DVASSERT(resourceIndex < (int32)allowedSizes.size());
	return allowedSizes[resourceIndex].folderName;
}
int32 Core::GetDesirableResourceIndex()
{
	return desirableIndex;
}
int32 Core::GetBaseResourceIndex()
{
	return 0;
}
	
const Vector2 &Core::GetPhysicalDrawOffset()
{
	return drawOffset;
}

	
	

void Core::SetPhysicalScreenSize(int32 width, int32 height)
{
	Logger::Info("Setting physical screen size to %dx%d", width, height);
	screenWidth = (float32)width;
	screenHeight = (float32)height;
	needTorecalculateMultipliers = true;
}
	
void Core::SetVirtualScreenSize(int32 width, int32 height)
{
	virtualScreenWidth = (float32)width;
	virtualScreenHeight = (float32)height;
	needTorecalculateMultipliers = true;
}

void Core::RegisterAvailableResourceSize(int32 width, int32 height, const String &resourcesFolderName)
{
	Core::AvailableSize newSize;
	newSize.width = width;
	newSize.height = height;
	newSize.folderName = resourcesFolderName;
	
	allowedSizes.push_back(newSize);
}
	
	
float32 Core::GetPhysicalScreenWidth()
{
	return screenWidth;
}
float32 Core::GetPhysicalScreenHeight()
{
	return screenHeight;
}
	
float32 Core::GetVirtualScreenWidth()
{
	return virtualScreenWidth;
}
float32 Core::GetVirtualScreenHeight()
{
	return virtualScreenHeight;
}
	
float32 Core::GetVirtualScreenXMin()
{
	return -(drawOffset.x * physicalToVirtual);
}
	
float32 Core::GetVirtualScreenXMax()
{
	return ((float32)(rotatedScreenWidth - drawOffset.x) * physicalToVirtual);
}
	
float32 Core::GetVirtualScreenYMin()
{
	return -(drawOffset.y * physicalToVirtual);
}
	
float32 Core::GetVirtualScreenYMax()
{
	return ((float32)(rotatedScreenHeight - drawOffset.y) * physicalToVirtual);
}
	
Core::eScreenMode Core::GetScreenMode()
{
	Logger::Debug("[Core::GetScreenMode] return screen mode MODE_UNSUPPORTED");
	return MODE_UNSUPPORTED;
}

void Core::SwitchScreenToMode(eScreenMode screenMode)
{
	Logger::Debug("[Core::SwitchScreenToMode] do not supported by platform implementation of core");
}

void Core::GetAvailableDisplayModes(List<DisplayMode> & availableModes)
{	

}
void Core::ToggleFullscreen()
{
	
}

DisplayMode Core::FindBestMode(const DisplayMode & requestedMode)
{
	List<DisplayMode> availableDisplayModes;
	GetAvailableDisplayModes(availableDisplayModes);

	DisplayMode bestMatchMode;

	bestMatchMode.refreshRate = -1;
	for (List<DisplayMode>::iterator it = availableDisplayModes.begin(); it != availableDisplayModes.end(); ++it)
	{
		DisplayMode & availableMode = *it;
		if ((availableMode.width == requestedMode.width) && (availableMode.height == requestedMode.height))
		{
			// if first mode found replace
			if (bestMatchMode.refreshRate == -1)
				bestMatchMode = availableMode;

			if (availableMode.bpp > bestMatchMode.bpp) // find best match with highest bits per pixel
			{
				bestMatchMode = availableMode;
			}
		}
	}

	if (bestMatchMode.refreshRate == -1) // haven't found any mode
	{
		for (List<DisplayMode>::iterator it = availableDisplayModes.begin(); it != availableDisplayModes.end(); ++it)
		{
			DisplayMode & availableMode = *it;

			int32 diffWidth = availableMode.width - requestedMode.width;
			int32 diffHeight = availableMode.height - requestedMode.height;
			if (diffWidth > 0 && diffHeight > 0)
			{
				int32 curDiffWidth = availableMode.width - bestMatchMode.width;
				int32 curDiffHeight = availableMode.height - bestMatchMode.height;

				if (diffWidth + diffHeight <= curDiffWidth + curDiffHeight)
				{
					bestMatchMode = availableMode;
				}
			}
		}
	}

	if (bestMatchMode.refreshRate == -1) // haven't found any mode
	{
		int maxRes = 0;
		for (List<DisplayMode>::iterator it = availableDisplayModes.begin(); it != availableDisplayModes.end(); ++it)
		{
			DisplayMode & availableMode = *it;

			//int32 diffWidth = availableMode.width ;
			//int32 diffHeight = availableMode.height - requestedMode.height;
			if (availableMode.width + availableMode.height + availableMode.bpp > maxRes)
			{
				maxRes = availableMode.width + availableMode.height + availableMode.bpp;
				bestMatchMode = availableMode;
			}
		}
	}
	return bestMatchMode;
}

DisplayMode Core::GetCurrentDisplayMode()
{
	return DisplayMode();
}

void Core::Quit()
{
	Logger::Debug("[Core::Quit] do not supported by platform implementation of core");
}
	
	
static ApplicationCore * core = 0;

void Core::SetApplicationCore(ApplicationCore * _core)
{
	core = _core;
}

ApplicationCore * Core::GetApplicationCore()
{
	return core;
}
	
	
void Core::SystemAppStarted()
{
	if (Core::Instance()->NeedToRecalculateMultipliers()) 
	{
		Core::Instance()->CalculateScaleMultipliers();
		/*  Question to Hottych: Does it really necessary here? 
            RenderManager::Instance()->SetRenderOrientation(Core::Instance()->GetScreenOrientation());
         */
	}

	if (core)core->OnAppStarted();
}
	
void Core::SystemAppFinished()
{
	if (core)core->OnAppFinished();
}


void Core::SystemProcessFrame()
{
	if (!core) return;
	if (!isActive)return;
	
	SystemTimer::Instance()->Start();

	/**
		Check if device not in lost state first / after that be
	*/
	if (!RenderManager::Instance()->IsDeviceLost())
	{
// #ifdef __DAVAENGINE_DIRECTX9__
// 		if(firstRun)
// 		{
// 			core->BeginFrame();
// 			firstRun = false;
// 		}
// #else
		core->BeginFrame();
//#endif
		RenderResource::SaveAllResourcesToSystemMem();

		// recalc frame inside begin / end frame
		if (Core::Instance()->needTorecalculateMultipliers) 
		{
			Core::Instance()->CalculateScaleMultipliers();
			RenderManager::Instance()->SetRenderOrientation(screenOrientation);
            UIScreenManager::Instance()->ScreenSizeChanged();
            UIControlSystem::Instance()->ScreenSizeChanged();
		}

		float32 frameDelta = SystemTimer::Instance()->FrameDelta();

		if(Replay::IsRecord())
		{
			Replay::Instance()->RecordFrame(frameDelta);
		}
		if(Replay::IsPlayback())
		{
			UIControlSystem::Instance()->ReplayEvents();
			frameDelta = Replay::Instance()->PlayFrameTime();
			if(Replay::IsPlayback()) //can be unset in previous string
			{
				SystemTimer::Instance()->SetFrameDelta(frameDelta);
			}
		}
		
		core->Update(frameDelta);
		core->Draw();

		core->EndFrame();
// #ifdef __DAVAENGINE_DIRECTX9__
// 		core->BeginFrame();
// #endif
	}
	globalFrameIndex++;
}

	
void Core::GoBackground()
{
#if defined(__DAVAENGINE_IPHONE__)
	if (core)
		core->OnBackground();
#endif 
}

uint32 Core::GetGlobalFrameIndex()
{
	return globalFrameIndex;
}

float32 GetScreenWidth()
{
	return Core::Instance()->GetVirtualScreenWidth();
}

float32 GetScreenHeight()
{
	return Core::Instance()->GetVirtualScreenHeight();
}
	
void Core::SetCommandLine(int argc, char *argv[])
{
	for (int k = 0; k < argc; ++k)
		commandLine.push_back(argv[k]);
}

Vector<String> & Core::GetCommandLine()
{
	return commandLine;
}
	
bool Core::IsConsoleMode()
{
	return isConsoleMode;
}
	
void Core::EnableConsoleMode()
{
	isConsoleMode = true;
}

bool Core::NeedToRecalculateMultipliers()
{
	return needTorecalculateMultipliers;
}

void Core::SetIsActive(bool _isActive)
{
	isActive = _isActive;
}


};
