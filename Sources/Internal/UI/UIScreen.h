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
        * Created by Alexey 'Hottych' Prosin
=====================================================================================*/

#ifndef __DAVAENGINE_UI_SCREEN_H__
#define __DAVAENGINE_UI_SCREEN_H__

#include "Base/BaseTypes.h"
#include "UI/UIControl.h"
#include "Render/RenderManager.h"
#include "Utils/Utils.h"
#include "Core/Core.h"

namespace DAVA 
{
	
class UIScreenTransition;
class UIControlSystem;	
class UIScreen : public UIControl 
{
public:
	enum eFillBorderOrder 
	{
			FILL_BORDER_BEFORE_DRAW = 0
		,	FILL_BORDER_AFTER_DRAW
		,	FILL_BORDER_NONE
	};
	
	UIScreen(const Rect &rect = Rect(0.0f,0.0f,(float32)GetScreenWidth(),(float32)GetScreenHeight()));
	virtual ~UIScreen();

	/**
	 \brief Sets the fill border ordrer. Borders fills Only when they are present on the screen. You can change filling type by overloading FillScreenBorders method.
	 
	 \param fillOrder Sets the borders filling order.
	 */	
	void SetFillBorderOrder(UIScreen::eFillBorderOrder fillOrder);

	/* 
		This is block of functions used by transition
	 */
	virtual void LoadGroup();
	virtual void UnloadGroup();
	virtual bool IsLoaded();
	
	virtual void AddToGroup(int32 groupId);
	virtual void RemoveFromGroup(int32 groupId);

	virtual	int32 GetGroupId();
	virtual void SystemDraw(const UIGeometricData &geometricData);// Internal method used by ControlSystem
    
    virtual void SystemWillAppear();
    virtual void SystemScreenSizeDidChanged(const Rect &newFullScreenSize);

protected:
	virtual void LoadResources() {};
	virtual void UnloadResources() {};
	
	/**
	 \brief Fills borders thats appears in non proportional screen scaling.
	 
	 \param geometricData Base geometric data. This parameter is'n used in the default realisation.
	 */	
	virtual void FillScreenBorders(const UIGeometricData &geometricData);


	int32 groupId;
private:
	bool  isLoaded;
	static List<UIScreen*>	appScreens;
	static int32			groupIdCounter;
	eFillBorderOrder fillBorderOrder;
    Rect fullScreenRect;
};

	
};

#endif