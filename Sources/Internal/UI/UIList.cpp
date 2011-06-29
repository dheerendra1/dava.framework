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

#include "UI/UIList.h"
#include "Debug/DVAssert.h"
#include "Platform/SystemTimer.h"
#include "UI/UIControlSystem.h"
#include "Base/ObjectFactory.h"

namespace DAVA 
{
	REGISTER_CLASS(UIList);
	
	
UIList::UIList(const Rect &rect, eListOrientation requiredOrientation, bool rectInAbsoluteCoordinates/* = FALSE*/)
	:	UIControl(rect, rectInAbsoluteCoordinates)
	,	delegate(NULL)
	,	orientation(requiredOrientation)
{
		InitAfterYaml();
}
		
UIList::UIList() : delegate(NULL), orientation(ORIENTATION_VERTICAL)
{
		
}
		
void UIList::InitAfterYaml()
{
	inputEnabled = TRUE;
	clipContents = TRUE;
	Rect r = GetRect();
	r.x = 0;
	r.y = 0;
	scrollContainer = new UIControl(r);
	AddControl(scrollContainer);
	
	oldPos = 0;
	newPos = 0;
	
	mainTouch = 0;
	
	touchHoldSize = 15;

	lockTouch = FALSE;
	
	needRefresh = FALSE;
	
	scroll = new ScrollHelper();
}

UIList::~UIList()
{
	SafeRelease(scrollContainer);
	SafeRelease(scroll);
	
	for (Map<String,Vector<UIListCell*>*>::iterator mit = cellStore.begin() ; mit != cellStore.end(); mit++) 
	{
		for (Vector<UIListCell*>::iterator it = mit->second->begin(); it != mit->second->end(); it++) 
		{
			SafeRelease(*it);
		}
		delete mit->second;
		mit->second = NULL;
	}
}

void UIList::ScrollTo(float delta)
{
	scroll->Impulse(delta * -4.8f);
}


void UIList::SetRect(const Rect &rect, bool rectInAbsoluteCoordinates/* = FALSE*/)
{
	if(delegate)
	{
		if(orientation == ORIENTATION_HORIZONTAL)
		{
			scroll->SetViewSize(rect.dx);
		}
		else 
		{
			scroll->SetViewSize(rect.dy);
		}
	}
	UIControl::SetRect(rect, rectInAbsoluteCoordinates);
}

void UIList::SetDelegate(UIListDelegate *newDelegate)
{
	delegate = newDelegate;
}

void UIList::FullRefresh()
{
	scrollContainer->RemoveAllControls();
	if(!delegate)
	{
		return;
	}
	
	needRefresh = FALSE;
	
	addPos = 0;
	int32 scrollAdd;
	int32 maxSize;
	if(orientation == ORIENTATION_HORIZONTAL)
	{
		scrollAdd = (int32)scrollContainer->GetRect().x;
		maxSize = (int32)GetRect().dx;
	}
	else 
	{
		scrollAdd = (int32)scrollContainer->GetRect().y;
		maxSize = (int32)GetRect().dy;
	}
	
	scroll->SetViewSize((float32)maxSize);
	
	int sz = 0;
    int32 elCnt = delegate->ElementsCount(this);
	for (int index = 0; index < elCnt; index++) 
	{
		int32 curPos = addPos + scrollAdd;
		int size = 0;
		if(orientation == ORIENTATION_HORIZONTAL)
		{
			size = delegate->CellWidth(this, index);
		}
		else 
		{
			size = delegate->CellHeight(this, index);
		}
		
		sz += size;
		if(curPos + size > -maxSize)
		{
			AddCellAtPos(delegate->CellAtIndex(this, index), addPos, size, index);
		}
		
		addPos += size;
		if(addPos + scrollAdd > maxSize * 2)
		{
			break;
		}
	}
	
	scroll->SetElementSize((float32)sz);
}

void UIList::RefreshList()
{
	needRefresh = TRUE;
}

void UIList::Update(float32 timeElapsed)
{
	if(!delegate)
	{
		return;
	}
	
	if(needRefresh)
	{
		FullRefresh();
	}
	
	float d = newPos - oldPos;
	oldPos = newPos;
	Rect r = scrollContainer->GetRect();
	if(orientation == ORIENTATION_HORIZONTAL)
	{
		r.x = scroll->GetPosition(d, SystemTimer::FrameDelta(), lockTouch);
	}
	else 
	{
		r.y = scroll->GetPosition(d, SystemTimer::FrameDelta(), lockTouch);
	}
	scrollContainer->SetRect(r);
	
	List<UIControl*>::const_iterator it;
	Rect viewRect = GetRect(TRUE);
	List<UIControl*> scrollList = scrollContainer->GetChildren();
	List<UIControl*> removeList;
	
	//removing invisible elements
	for(it = scrollList.begin(); it != scrollList.end(); it++)
	{
		Rect crect = (*it)->GetRect(TRUE);
		if(orientation == ORIENTATION_HORIZONTAL)
		{
			if(crect.x + crect.dx < viewRect.x - viewRect.dx || crect.x > viewRect.x + viewRect.dx*2)
			{
				removeList.push_back(*it);
			}
		}
		else 
		{
			if(crect.y + crect.dy < viewRect.y - viewRect.dy || crect.y > viewRect.y + viewRect.dy*2)
			{
				removeList.push_back(*it);
			}
		}
	}
	for(it = removeList.begin(); it != removeList.end(); it++)
	{
		scrollContainer->RemoveControl((*it));
	}
	

	//adding elements at the list end
	int32 ind = -1;
	UIListCell *fc = NULL;
	for(it = scrollList.begin(); it != scrollList.end(); it++)
	{
		UIListCell *lc = (UIListCell *)(*it);
		int32 i = lc->GetIndex();
		if(i > ind)
		{
			ind = i;
			fc = lc;
		}
	}
	if(fc)
	{
		int32 borderPos;
		int32 rPos;
		int size = 0;
		int32 off;
		if(orientation == ORIENTATION_HORIZONTAL)
		{
			borderPos = (int32)(viewRect.dx + viewRect.dx / 2.0f);
			off = (int32)scrollContainer->GetRect().x;
			rPos = (int32)(fc->GetRect().x + fc->GetRect().dx + off);
		}
		else 
		{
			borderPos = (int32)(viewRect.dy + viewRect.dy / 22.0f);
			off = (int32)scrollContainer->GetRect().y;
			rPos = (int32)(fc->GetRect().y + fc->GetRect().dy + off);
		}
		while(rPos < borderPos && fc->GetIndex() < delegate->ElementsCount(this) - 1)
		{
			int32 i = fc->GetIndex() + 1;
			fc = delegate->CellAtIndex(this, i);
			if(orientation == ORIENTATION_HORIZONTAL)
			{
				size = delegate->CellWidth(this, i);
			}
			else 
			{
				size = delegate->CellHeight(this, i);
			}
			AddCellAtPos(fc, rPos - off, size, i);
			rPos += size;
			scroll->SetElementSize((float32)(rPos - off));
		}
	}

	//adding elements at the list begin
	ind = maximumElementsCount;
	fc = NULL;
	for(it = scrollList.begin(); it != scrollList.end(); it++)
	{
		UIListCell *lc = (UIListCell *)(*it);
		int32 i = lc->GetIndex();
		if(i < ind)
		{
			ind = i;
			fc = lc;
		}
	}
	if(fc)
	{
		int32 borderPos;
		int32 rPos;
		int size = 0;
		int32 off;
		if(orientation == ORIENTATION_HORIZONTAL)
		{
			borderPos = (int32)(-viewRect.dx/2.0f);
			off = (int32)scrollContainer->GetRect().x;
			rPos = (int32)(fc->GetRect().x + off);
		}
		else 
		{
			borderPos = (int32)(-viewRect.dy/2.0f);
			off = (int32)scrollContainer->GetRect().y;
			rPos = (int32)(fc->GetRect().y + off);
		}
		while(rPos > borderPos && fc->GetIndex() > 0)
		{
			int32 i = fc->GetIndex() - 1;
			fc = delegate->CellAtIndex(this, i);
			if(orientation == ORIENTATION_HORIZONTAL)
			{
				size = delegate->CellWidth(this, i);
			}
			else 
			{
				size = delegate->CellHeight(this, i);
			}
			rPos -= size;
			AddCellAtPos(fc, rPos - off, size, i);
		}
	}
	
}

void UIList::Draw(const UIGeometricData &geometricData)
{
	if(needRefresh)
	{
		FullRefresh();
	}
	UIControl::Draw(geometricData);
}

void UIList::Input(UIEvent *currentInput)
{
	if(orientation == ORIENTATION_HORIZONTAL)
	{
		newPos = currentInput->point.x;
	}
	else 
	{
		newPos = currentInput->point.y;
	}

	switch (currentInput->phase) 
	{
		case UIEvent::PHASE_BEGAN:
		{
			lockTouch = TRUE;
			oldPos = newPos;
			mainTouch = currentInput->tid;
		}
			break;
		case UIEvent::PHASE_DRAG:
		{
		}
			break;
		case UIEvent::PHASE_ENDED:
		{
			lockTouch = FALSE;
			mainTouch = 0;
		}
			break;
	}
}

bool UIList::SystemInput(UIEvent *currentInput)
{
	if(!inputEnabled || !visible || controlState & STATE_DISABLED)
	{
		return false;
	}

	if(currentInput->touchLocker != this)
	{
		if(currentInput->phase == UIEvent::PHASE_BEGAN)
		{
			if(IsPointInside(currentInput->point))
			{
				PerformEvent(EVENT_TOUCH_DOWN);
				Input(currentInput);
			}
		}
		else if(currentInput->tid == mainTouch && currentInput->phase == UIEvent::PHASE_DRAG)
		{
			if(orientation == ORIENTATION_HORIZONTAL)
			{
				if(abs(currentInput->point.x - newPos) > touchHoldSize)
				{
					UIControlSystem::Instance()->SwitchInputToControl(mainTouch, this);
					newPos = currentInput->point.x;
					return TRUE;
				}
			}
			else 
			{
				if(abs(currentInput->point.y - newPos) > touchHoldSize)
				{
					UIControlSystem::Instance()->SwitchInputToControl(mainTouch, this);
					newPos = currentInput->point.y;
					return TRUE;
				}
			}
		}
		else if(currentInput->tid == mainTouch && currentInput->phase == UIEvent::PHASE_ENDED)
		{
			mainTouch = 0;
			lockTouch = false;
		}

		

	}

	return UIControl::SystemInput(currentInput);
}
	
void UIList::OnSelectEvent(BaseObject *pCaller, void *pUserData, void *callerData)
{
	if(delegate)
	{
		delegate->OnCellSelected(this, (UIListCell*)pCaller);
	}
}

void UIList::AddCellAtPos(UIListCell *cell, int32 pos, int32 size, int32 index)
{
	DVASSERT(cell);
	DVASSERT(cell->cellStore == NULL || cell->cellStore == this);
	if(!cell->cellStore)
	{
		cell->cellStore = this;
		cell->AddEvent(EVENT_TOUCH_UP_INSIDE, Message(this, &UIList::OnSelectEvent));
		Vector<UIListCell*> *store = GetStoreVector(cell->identifier);
		if(!store)
		{
			store = new Vector<UIListCell*>;
			cellStore[cell->identifier] = store;
		}
		store->push_back(cell);
	}
	cell->currentIndex = index;
	Rect r = cell->GetRect();
	if(orientation == ORIENTATION_HORIZONTAL)
	{
		r.dx = (float32)size;
		r.x = (float32)pos;
	}
	else 
	{
		r.dy = (float32)size;
		r.y = (float32)pos;
	}
	cell->SetRect(r);
	scrollContainer->AddControl(cell);
	
}

Vector<UIListCell*> *UIList::GetStoreVector(const String &cellIdentifier)
{
	Map<String,Vector<UIListCell*>*>::const_iterator mit;
	mit = cellStore.find(cellIdentifier);
	if (mit == cellStore.end())
	{
		return NULL;
	}
	
	return mit->second;
}

UIListCell* UIList::GetReusableCell(const String &cellIdentifier)
{
	Vector<UIListCell*> *store = GetStoreVector(cellIdentifier);
	if(!store)
	{
		return NULL;
	}
	
	for(Vector<UIListCell*>::iterator it = store->begin(); it != store->end(); it++)
	{
		if((*it)->GetIndex() == -1)
		{
			return (*it);
		}
	}
	
	return NULL;
	
}
	
const List<UIControl*> &UIList::GetVisibleCells()
{
	return scrollContainer->GetChildren();	
}

	
void UIList::SetTouchHoldDelta(int32 holdDelta)
{
	touchHoldSize = holdDelta;
}
int32 UIList::GetTouchHoldDelta()
{
	return touchHoldSize;
}

void UIList::SetSlowDownTime(float newValue)
{
	scroll->SetSlowDownTime(newValue);
}
void UIList::SetBorderMoveModifer(float newValue)
{
	scroll->SetBorderMoveModifer(newValue);
}

void UIList::SystemWillAppear()
{
	UIControl::SystemWillAppear();	
	RefreshList();
}

void UIList::LoadFromYamlNode(YamlNode * node, UIYamlLoader * loader)
{
	UIControl::LoadFromYamlNode(node, loader);
		
	YamlNode * orientNode = node->Get("orientation");
	if (orientNode)
	{
		if (orientNode->AsString() == "ORIENTATION_VERTICAL")
			orientation = ORIENTATION_VERTICAL;
		else if (orientNode->AsString() == "ORIENTATION_HORIZONTAL")
			orientation = ORIENTATION_HORIZONTAL;
		else 
		{
			DVASSERT(0 && "Orientation constant is wrong");
		}
	}
		
		
		
		// TODO
	InitAfterYaml();
}
    
float32 UIList::VisibleAreaSize(UIScrollBar *forScrollBar)
{
    return scroll->GetViewSize();
}
    
float32 UIList::TotalAreaSize(UIScrollBar *forScrollBar)
{
    return scroll->GetElementSize();
}
    
float32 UIList::ViewPosition(UIScrollBar *forScrollBar)
{
    return scroll->GetPosition();
}

void UIList::OnViewPositionChanged(UIScrollBar *byScrollBar, float32 newPosition)
{
    scroll->SetPosition(-newPosition);
}
    
	
};