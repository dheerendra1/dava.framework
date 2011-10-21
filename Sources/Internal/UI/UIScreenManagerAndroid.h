/*
 *  ScreenManagerC.h
 *  MahJongQuest
 *
 *  Created by Vitaliy  Borodovsky on 11/5/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __DAVAENGINE_ANDROID_SCREENMANAGER_C_H__
#define __DAVAENGINE_ANDROID_SCREENMANAGER_C_H__

#include "DAVAEngine.h"

namespace DAVA 
{
class UIScreenManager : public Singleton<UIScreenManager>
{
public:
	UIScreenManager();
	virtual ~UIScreenManager();
	
	void RegisterScreen(int screenId, UIScreen * screen);

	void SetFirst(int screenId);
	void SetScreen(int screenId, UIScreenTransition * transition = 0);

	UIScreen *GetScreen(int screenId);
	UIScreen *GetScreen();
	int32 GetScreenId();

    void ScreenSizeChanged();


private:
	
	void ActivateGLController();
	
	struct Screen
	{
		enum eType 
		{
			TYPE_NULL = 0,
			TYPE_SCREEN,
		};
		Screen(eType _type = TYPE_NULL, void * _value  = 0) 
		{
			type = _type;
			value = _value;
		}
		eType	type;
		void *	value;
	};
	
	Map<int, Screen>		screens;
	int						glControllerId;
	int						activeControllerId;
	int						activeScreenId;
};
};

#endif // __DAVAENGINE_ANDROID_SCREENMANAGER_C_H__