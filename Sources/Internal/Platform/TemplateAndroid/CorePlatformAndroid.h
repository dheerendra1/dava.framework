/*
 *  CorePlatformAndroid.h
 *  TemplateProjectAndroid
 *
 *  Created by Victor  Kleschenko on 2/18/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef __DAVAENGINE_CORE_PLATFORM_ANDROID_H__
#define __DAVAENGINE_CORE_PLATFORM_ANDROID_H__

#include "DAVAEngine.h"
#if defined(__DAVAENGINE_ANDROID__)

#include "AndroidSpecifics.h"


namespace DAVA
{

class AndroidSystemListener
{
public:
	virtual void ShowKeyboard() = 0;
	virtual void HideKeyboard() = 0;
	virtual bool DownloadHttpFile(const String & url, const String & documentsPathname) = 0;
};



class Thread;
class CoreAndroidPlatform: public Core
{
public:

	CoreAndroidPlatform();

	virtual void CreateAndroidWindow(const char8 *docPath, const char8 *assets, const char8 *logTag, AndroidSystemListener * sysListener);

	virtual void Quit();

	void RepaintView();
	void ResizeView(int32 w, int32 h);

	// called from Activity and manage a visible lifetime
	void StartVisible();
	void StopVisible();

	void StartForeground();
	void StopForeground();

	void OnCreateActivity();
	void OnDestroyActivity();

	void KeyUp(int keyCode);
	void KeyDown(int keyCode);

	void OnTouch(int action, int id, int x, int y, long time);

	void RenderRecreated();

	bool DownloadHttpFile(const String & url, const String & documentsPathname);

private:

	void QuitAction();
	void ProcessWithoutDrawing();

	void UpdateScreenMode();


private:
	DisplayMode windowedMode;
	int32 oldWidth;
	int32 oldHeight;

	bool wasCreated;
	bool renderIsActive;

	bool foreground;

	AndroidSystemListener *androidListener;
};
};
#endif // #if defined(__DAVAENGINE_ANDROID__)
#endif // __DAVAENGINE_CORE_PLATFORM_ANDROID_H__
