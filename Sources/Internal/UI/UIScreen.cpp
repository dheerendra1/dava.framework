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

#include "UI/UIScreen.h"
#include "Render/RenderManager.h"
#include "Render/RenderHelper.h"
#include "Platform/SystemTimer.h"

namespace DAVA 
{

List<UIScreen*> UIScreen::appScreens;
int32			UIScreen::groupIdCounter = -1;

UIScreen::UIScreen(const Rect &rect)
	:UIControl(rect)
,	groupId(groupIdCounter)
{
	// add screen to list
	appScreens.push_back(this);
	groupIdCounter --;
	isLoaded = false;
	fillBorderOrder = FILL_BORDER_AFTER_DRAW;
    fullScreenRect = rect;
}
	
UIScreen::~UIScreen()
{
	// remove screen from list
	for (List<UIScreen*>::iterator t = appScreens.begin(); t != appScreens.end(); ++t)
	{
		if (*t == this)
		{
			appScreens.erase(t);
			break;
		}
	}
}
    
void UIScreen::SystemWillAppear()
{
    UIControl::SystemWillAppear();

    if (fullScreenRect.dx != Core::Instance()->GetVirtualScreenXMax() - Core::Instance()->GetVirtualScreenXMin()
        || fullScreenRect.dy != Core::Instance()->GetVirtualScreenYMax() - Core::Instance()->GetVirtualScreenYMin())
    {
        Rect fsr;
        fsr.x = Core::Instance()->GetVirtualScreenXMin();
        fsr.y = Core::Instance()->GetVirtualScreenYMin();
        fsr.dx = Core::Instance()->GetVirtualScreenXMax() - Core::Instance()->GetVirtualScreenXMin();
        fsr.dy = Core::Instance()->GetVirtualScreenYMax() - Core::Instance()->GetVirtualScreenYMin();
        SystemScreenSizeDidChanged(fsr);
    }
}

void UIScreen::SystemScreenSizeDidChanged(const Rect &newFullScreenRect)
{
    fullScreenRect = newFullScreenRect;
    UIControl::SystemScreenSizeDidChanged(newFullScreenRect);
}

	
void UIScreen::SetFillBorderOrder(UIScreen::eFillBorderOrder fillOrder)
{
	fillBorderOrder = fillOrder;
}

	
void UIScreen::SystemDraw(const UIGeometricData &geometricData)
{
	if (fillBorderOrder == FILL_BORDER_BEFORE_DRAW)
	{
		FillScreenBorders(geometricData);
		UIControl::SystemDraw(geometricData);
	}
	else if (fillBorderOrder == FILL_BORDER_AFTER_DRAW)
	{
		UIControl::SystemDraw(geometricData);
		FillScreenBorders(geometricData);
	}
	else 
	{
		UIControl::SystemDraw(geometricData);
	}
}

void UIScreen::FillScreenBorders(const UIGeometricData &geometricData)
{
	RenderManager::Instance()->SetColor(0, 0, 0, 1.0f);
	UIGeometricData drawData;
	drawData.position = relativePosition;
	drawData.size = size;
	drawData.pivotPoint = pivotPoint;
	drawData.scale = scale;
	drawData.angle = angle;
	drawData.AddToGeometricData(geometricData);

	Rect drawRect = drawData.GetUnrotatedRect();
	if (Core::Instance()->GetVirtualScreenXMin() < 0)
	{
		RenderHelper::Instance()->FillRect(Rect(
													Core::Instance()->GetVirtualScreenXMin()
												 ,	0
												 ,	-Core::Instance()->GetVirtualScreenXMin()
												 ,	Core::Instance()->GetVirtualScreenHeight()));
		RenderHelper::Instance()->FillRect(Rect(
												 Core::Instance()->GetVirtualScreenWidth()
												 ,	0
												 ,	Core::Instance()->GetVirtualScreenXMax() - Core::Instance()->GetVirtualScreenWidth()
												 ,	Core::Instance()->GetVirtualScreenHeight()));
	}
	else 
	{
		RenderHelper::Instance()->FillRect(Rect(
													0
												 ,	Core::Instance()->GetVirtualScreenYMin()
												 ,	Core::Instance()->GetVirtualScreenWidth()+1
												 ,	-Core::Instance()->GetVirtualScreenYMin()));
		RenderHelper::Instance()->FillRect(Rect(
												 0
												 ,	Core::Instance()->GetVirtualScreenHeight()
												 ,	Core::Instance()->GetVirtualScreenWidth()+1
												 ,	Core::Instance()->GetVirtualScreenYMax() - Core::Instance()->GetVirtualScreenHeight()));
	}

	RenderManager::Instance()->ResetColor();
}


void UIScreen::LoadGroup()
{
	//Logger::Debug("load group started");
	//uint64 loadGroupStart = SystemTimer::Instance()->AbsoluteMS();
	if (groupId < 0)
	{
		if (isLoaded)return;
		
		LoadResources();
		isLoaded = true;
	}else
	{
		for (List<UIScreen*>::iterator t = appScreens.begin(); t != appScreens.end(); ++t)
		{
			UIScreen * screen = *t;
			if ((screen->groupId == groupId) && (!screen->isLoaded))
			{
				screen->LoadResources();
				screen->isLoaded = true;
			}
		}
	}
	//uint64 loadGroupEnd = SystemTimer::Instance()->AbsoluteMS();
	//Logger::Debug("load group finished: %lld", loadGroupEnd - loadGroupStart);
}

void UIScreen::UnloadGroup()
{
	if (groupId < 0)
	{
		if (!isLoaded)return;
		
		UnloadResources();
		isLoaded = false;
	}else
	{
		for (List<UIScreen*>::iterator t = appScreens.begin(); t != appScreens.end(); ++t)
		{
			UIScreen * screen = *t;
			if ((screen->groupId == groupId) && (screen->isLoaded))
			{
				screen->UnloadResources();
				screen->isLoaded = false;
			}
		}
	}	
}

bool UIScreen::IsLoaded()
{
	return isLoaded;
}
	
void UIScreen::AddToGroup(int32 _groupId)
{
	groupId = _groupId;
}

void UIScreen::RemoveFromGroup(int32 _groupId)
{
	groupId = groupIdCounter--;
}

int32 UIScreen::GetGroupId()
{
	return groupId;
}

};