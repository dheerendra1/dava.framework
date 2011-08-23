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

#include "UI/UIControl.h"
#include "UI/UIControlSystem.h"
#include "Animation/LinearAnimation.h"
#include "Debug/DVAssert.h"
#include "Render/RenderManager.h"
#include "Base/ObjectFactory.h"
#include "UI/UIYamlLoader.h"

namespace DAVA 
{
	REGISTER_CLASS(UIControl);
	
	UIControl::UIControl(const Rect &rect, bool rectInAbsoluteCoordinates/* = false*/)
	{
		parent = NULL;
		controlState = STATE_NORMAL;
		visible = true;
		/* 
			VB:
			please do not change anymore to false, it no make any sense to make all controls untouchable by default.
			for particular controls it can be changed, but no make sense to make that for all controls.
		 */
		inputEnabled = true; 
		
		background = new UIControlBackground();
		needToRecalcFromAbsoluteCoordinates = false;
		eventDispatcher = NULL;
		clipContents = false;
		
//		scaleInParent = true;

//		Vector2 relativePosition;
//		Vector2 size;
		
		
//		absoluteRect = Rect(0,0,0,0);
		debugDrawEnabled = false;
		absolutePosition = Vector2(0, 0);
		
		pivotPoint = Vector2(0, 0);
		scale = Vector2(1.0f, 1.0f);
		angle = 0;
		
		multiInput = false;
		exclusiveInput = false;
		currentInputID = 0;
		touchesInside = 0;
		totalTouches = 0;

		
		SetRect(rect, rectInAbsoluteCoordinates);

#ifdef ENABLE_CONTROL_EDIT
		__touchStart = Vector2(0.f, 0.f);
		__oldRect = relativeRect;
#endif
	}
	
	UIControl::~UIControl()
	{
		if(totalTouches > 0)
		{
			UIControlSystem::Instance()->CancelInputs(this);
		}
		SafeRelease(background);
		SafeRelease(eventDispatcher);
		RemoveAllControls();
	}
	
	void UIControl::SetParent(UIControl *newParent)
	{
		if (!newParent) 
		{
			if(totalTouches > 0)
			{
				UIControlSystem::Instance()->CancelInputs(this);
			}
		}
		parent = newParent;
		if(parent && needToRecalcFromAbsoluteCoordinates)
		{
			relativePosition = absolutePosition - parent->GetGeometricData().position;
			needToRecalcFromAbsoluteCoordinates = false;
		}
		
	}
	UIControl *UIControl::GetParent()
	{
		return parent;
	}
	
	bool UIControl::GetExclusiveInput()
	{
		return exclusiveInput;
	}
	
	void UIControl::SetExclusiveInput(bool isExclusiveInput, bool hierarchic/* = true*/)
	{
		exclusiveInput = isExclusiveInput;
		
		if(hierarchic)
		{	
			List<UIControl*>::iterator it = childs.begin();
			for(; it != childs.end(); ++it)
			{
				(*it)->SetExclusiveInput(isExclusiveInput, hierarchic);
			}
		}
	}
	
	bool UIControl::GetMultiInput()
	{
		return multiInput;
	}
	
	void UIControl::SetMultiInput(bool isMultiInput, bool hierarchic/* = true*/)
	{
		multiInput = isMultiInput;
		
		if(hierarchic)
		{	
			List<UIControl*>::iterator it = childs.begin();
			for(; it != childs.end(); ++it)
			{
				(*it)->SetMultiInput(isMultiInput, hierarchic);
			}
		}
	}
	
	
	void UIControl::AddEvent(int32 eventType, const Message &msg)
	{
		if(!eventDispatcher)
		{
			eventDispatcher = new EventDispatcher();
		}
		eventDispatcher->AddEvent(eventType, msg);
	}
	bool UIControl::RemoveEvent(int32 eventType, const Message &msg)
	{
		if(eventDispatcher)
		{
			return eventDispatcher->RemoveEvent(eventType, msg);
		}
		return false;
	}
	
	bool UIControl::RemoveAllEvents()
	{
		if(eventDispatcher)
		{
			return eventDispatcher->RemoveAllEvents();
		}
		return false;
	}
	
	void UIControl::PerformEvent(int32 eventType)
	{
		if(eventDispatcher)
		{
			eventDispatcher->PerformEvent(eventType, this);
		}
	}

	void UIControl::PerformEventWithData(int32 eventType, void *callerData)
	{
		if(eventDispatcher)
		{
			eventDispatcher->PerformEventWithData(eventType, this, callerData);
		}
	}
	

	const List<UIControl*> &UIControl::GetChildren()
	{
		return childs;
	}

	void UIControl::SetName(const String & _name)
	{
		name = _name;
	}
	
	const String & UIControl::GetName()
	{
		return name;
	}
	
	void UIControl::SetTag(int32 _tag)
	{
		tag = _tag;
	}
	
	int32 UIControl::GetTag()
	{
		return tag;
	}
	
	// return first control with given name
	UIControl * UIControl::FindByName(const String & name, bool recursive)
	{
		List<UIControl*>::iterator it = childs.begin();
		for(; it != childs.end(); it++)
		{
			UIControl * c = (*it);
			if (c->name == name)return c;
			
			if (recursive)
			{
				UIControl * inChilds = c->FindByName(name);
				if (inChilds)return inChilds;
			}
		}
		return 0;
	}

	
	
	int32 UIControl::GetState()
	{
		return controlState;
	}
	
	void UIControl::SetState(int32 state)
	{
		controlState = state;
	}
	
	Sprite* UIControl::GetSprite()
	{
		return background->GetSprite();
	}
	int32 UIControl::GetFrame()
	{
		return background->GetFrame();
	}
	
	UIControlBackground::eDrawType UIControl::GetSpriteDrawType()
	{
		return background->GetDrawType();
	}
	int32 UIControl::GetSpriteAlign()
	{
		return background->GetAlign();
	}
	void UIControl::SetSprite(const String &spriteName, int32 spriteFrame)
	{
		background->SetSprite(spriteName, spriteFrame);
	}
	void UIControl::SetSprite(Sprite *newSprite, int32 spriteFrame)
	{
		background->SetSprite(newSprite, spriteFrame);
	}
	void UIControl::SetSpriteFrame(int32 spriteFrame)
	{
		background->SetFrame(spriteFrame);
	}
	void UIControl::SetSpriteDrawType(UIControlBackground::eDrawType drawType)
	{
		background->SetDrawType(drawType);
	}
	void UIControl::SetSpriteAlign(int32 align)
	{
		background->SetAlign(align);
	}
	
	
	void UIControl::SetBackground(UIControlBackground *newBg)
	{
		DVASSERT(newBg);
		SafeRelease(background);
		background = newBg->Clone();
	}

	UIControlBackground *UIControl::GetBackground()
	{
		return background;
	}

	const UIGeometricData &UIControl::GetGeometricData()
	{
		tempGeometricData.position = relativePosition;
		tempGeometricData.size = size;
		tempGeometricData.pivotPoint = pivotPoint;
		tempGeometricData.scale = scale;
		tempGeometricData.angle = angle;
		if(!parent)
		{
			tempGeometricData.AddToGeometricData(UIControlSystem::Instance()->GetBaseGeometricData());
			return tempGeometricData;
		}
		tempGeometricData.AddToGeometricData(parent->GetGeometricData());
		return tempGeometricData;
	}

	const Vector2 &UIControl::GetPosition(bool absoluteCoordinates/* = false*/)
	{
		if(!absoluteCoordinates)
		{
			return relativePosition;
		}
		if(parent)
		{
			absolutePosition = relativePosition + parent->GetGeometricData().position;
			return absolutePosition;
		}
		if(!needToRecalcFromAbsoluteCoordinates)
		{
			return relativePosition;
		}
		return absolutePosition;
	}
	
	void UIControl::SetPosition(const Vector2 &position, bool positionInAbsoluteCoordinates/* = false*/)
	{
		if(!positionInAbsoluteCoordinates)
		{
			relativePosition = position;
			needToRecalcFromAbsoluteCoordinates = false;
		}
		else
		{
			if(parent)
			{
				relativePosition = position - parent->GetGeometricData().position;
				needToRecalcFromAbsoluteCoordinates = false;
			}
			else
			{
				needToRecalcFromAbsoluteCoordinates = true;
				relativePosition = absolutePosition = position;
			}
		}
	}
	
	const Vector2 &UIControl::GetSize()
	{
		return size;
	}
	void UIControl::SetSize(const Vector2 &newSize)
	{
		size = newSize;
	}
	
	float32 UIControl::GetAngle()
	{
		return angle;
	}
	void UIControl::SetAngle(float32 angleInRad)
	{
		angle = angleInRad;
	}
	
	const Rect &UIControl::GetRect(bool absoluteCoordinates/* = FALSE*/)
	{
		Vector2 pos = GetPosition(absoluteCoordinates) - pivotPoint;
		returnedRect = Rect(pos.x, pos.y, size.x, size.y);
		return returnedRect;
	}
	
	void UIControl::SetRect(const Rect &rect, bool rectInAbsoluteCoordinates/* = FALSE*/)
	{
		Vector2 t(rect.dx, rect.dy);
		SetSize(t);
		t.x = rect.x;
		t.y = rect.y;
		t += pivotPoint;
		SetPosition(t, rectInAbsoluteCoordinates);
		
		/*
		scale.x = 1.0;
		scale.y = 1.0;
		if(parent && scaleInParent)
		{
			realScale.x = scale.x * parent->GetRealScale().x;
			realScale.y = scale.y * parent->GetRealScale().y;
		}
		else 
		{
			realScale = scale;
		}

		if(!rectInAbsoluteCoordinates)
		{
			relativeRect = rect;
			RecalcScaledRect();
			needToRecalcFromAbsoluteCoordinates = false;
		}
		else
		{
			if(parent)
			{
				relativeRect = rect - parent->GetRect(TRUE).GetPosition();
				RecalcScaledRect();
				needToRecalcFromAbsoluteCoordinates = false;
			}
			else
			{
				needToRecalcFromAbsoluteCoordinates = true;
				relativeRect = absoluteRect = rect;
				RecalcScaledRect();
			}
		}
		 */
	}
	
	void UIControl::SetScaledRect(const Rect &rect, bool rectInAbsoluteCoordinates/* = false*/)
	{
		if(!rectInAbsoluteCoordinates || !parent)
		{
			scale.x = rect.dx / size.x;
			scale.y = rect.dy / size.y;
			SetPosition(Vector2(rect.x + pivotPoint.x * scale.x, rect.y + pivotPoint.y * scale.y), rectInAbsoluteCoordinates);
		}
		else
		{
			const UIGeometricData &gd = parent->GetGeometricData();
			scale.x = rect.dx / (size.x * gd.scale.x);
			scale.y = rect.dy / (size.y * gd.scale.y);
			SetPosition(Vector2(rect.x + pivotPoint.x * scale.x, rect.y + pivotPoint.y * scale.y), rectInAbsoluteCoordinates);
		}
	}

	
	bool UIControl::GetVisible()
	{
		return visible;
	}
	
	void UIControl::SetVisible(bool isVisible, bool hierarchic/* = true*/)
	{
		visible = isVisible;
		if (!visible) 
		{
			if(totalTouches > 0)
			{
				UIControlSystem::Instance()->CancelInputs(this);
			}
		}
		
		if(hierarchic)
		{	
			List<UIControl*>::iterator it = childs.begin();
			for(; it != childs.end(); ++it)
			{
				(*it)->SetVisible(isVisible, hierarchic);
			}
		}
	}
	
	bool UIControl::GetInputEnabled()
	{
		return inputEnabled;
	}
	
	void UIControl::SetInputEnabled(bool isEnabled, bool hierarchic/* = true*/)
	{
		inputEnabled = isEnabled;
		if(hierarchic)
		{	
			List<UIControl*>::iterator it = childs.begin();
			for(; it != childs.end(); ++it)
			{
				(*it)->SetInputEnabled(isEnabled, hierarchic);
			}
		}
	}
	
	bool UIControl::GetDisabled()
	{
		return ((controlState & STATE_DISABLED) != 0);
	}
	
	void UIControl::SetDisabled(bool isDisabled, bool hierarchic/* = true*/)
	{
		if(isDisabled)
		{
			controlState |= STATE_DISABLED;
		}
		else
		{
			controlState &= ~STATE_DISABLED;
		}
		
		if(hierarchic)
		{	
			List<UIControl*>::iterator it = childs.begin();
			for(; it != childs.end(); ++it)
			{
				(*it)->SetDisabled(isDisabled, hierarchic);
			}
		}
	}
	
	bool UIControl::GetSelected()
	{
		return ((controlState & STATE_SELECTED) != 0);
	}
	
	void UIControl::SetSelected(bool isSelected, bool hierarchic/* = true*/)
	{
		if(isSelected)
		{
			controlState |= STATE_SELECTED;
		}
		else
		{
			controlState &= ~STATE_SELECTED;
		}
		
		if(hierarchic)
		{	
			List<UIControl*>::iterator it = childs.begin();
			for(; it != childs.end(); ++it)
			{
				(*it)->SetSelected(isSelected, hierarchic);
			}
		}
	}

	
	bool UIControl::GetClipContents()
	{
		return clipContents;
	}
	void UIControl::SetClipContents(bool isNeedToClipContents)
	{
		clipContents = isNeedToClipContents;
	}

	bool UIControl::GetHover()
	{
		return (controlState & STATE_HOVER) != 0;
	}

//	void UIControl::SystemClearHoverState()
//	{
//		controlState &= ~STATE_HOVER;
//		List<UIControl*>::iterator it = childs.begin();
//		for(; it != childs.end(); ++it)
//		{
//			(*it)->SystemClearHoverState();
//		}
//	}

	void UIControl::AddControl(UIControl *control)
	{
		control->Retain();
		if(control->parent)
		{
			control->parent->RemoveControl(control);
		}
		bool onScreen = IsOnScreen();
		if(onScreen)
		{
			control->SystemWillAppear();
		}
		control->isUpdated = false;
//		control->WillAppear();
		control->SetParent(this);
		childs.push_back(control);
		if(onScreen)
		{
			control->SystemDidAppear();
		}
		isIteratorCorrupted = true;
	}
	void UIControl::RemoveControl(UIControl *control)
	{
		List<UIControl*>::iterator it = childs.begin();
		for(; it != childs.end(); it++)
		{
			if((*it) == control)
			{
				bool onScreen = IsOnScreen();
				if(onScreen)
				{
					control->SystemWillDisappear();
				}
				control->SetParent(NULL);
				childs.erase(it);
				if(onScreen)
				{
					control->SystemDidDisappear();
				}
				control->Release();
				isIteratorCorrupted = true;
				return;
			}
		}
	}
	void UIControl::RemoveAllControls()
	{
		while(!childs.empty())
		{
			RemoveControl(childs.front());
		}
	}
	void UIControl::BringChildFront(UIControl *_control)
	{
		List<UIControl*>::iterator it = childs.begin();
		for(; it != childs.end(); it++)
		{
			if((*it) == _control)
			{
				childs.erase(it);
				childs.push_back(_control);
				isIteratorCorrupted = true;
				return;
			}
		}
	}
	void UIControl::BringChildBack(UIControl *_control)
	{
		List<UIControl*>::iterator it = childs.begin();
		for(; it != childs.end(); it++)
		{
			if((*it) == _control)
			{
				childs.erase(it);
				childs.push_front(_control);
				isIteratorCorrupted = true;
				return;
			}
		}
	}
	
	void UIControl::InsertChildBelow(UIControl * _control, UIControl * _belowThisChild)
	{
		List<UIControl*>::iterator it = childs.begin();
		for(; it != childs.end(); it++)
		{
			if((*it) == _belowThisChild)
			{
				
				_control->Retain();
				if(_control->parent)
				{
					_control->parent->RemoveControl(_control);
				}
				bool onScreen = IsOnScreen();
				if(onScreen)
				{
					_control->SystemWillAppear();
				}
				childs.insert(it, _control);
				_control->SetParent(this);
				if(onScreen)
				{
					_control->SystemDidAppear();
				}
				isIteratorCorrupted = true;
				return;
			}
		}
		
		AddControl(_control);
	}
	void UIControl::InsertChildAbove(UIControl * _control, UIControl * _aboveThisChild)
	{
		List<UIControl*>::iterator it = childs.begin();
		for(; it != childs.end(); it++)
		{
			if((*it) == _aboveThisChild)
			{
				_control->Retain();
				if(_control->parent)
				{
					_control->parent->RemoveControl(_control);
				}
				bool onScreen = IsOnScreen();
				if(onScreen)
				{
					_control->SystemWillAppear();
				}
				childs.insert(++it, _control);
				_control->SetParent(this);
				if(onScreen)
				{
					_control->SystemDidAppear();
				}
				isIteratorCorrupted = true;
				return;
			}
		}
		
		AddControl(_control);
	}
	
	void UIControl::SendChildBelow(UIControl * _control, UIControl * _belowThisChild)
	{
		//TODO: Fix situation when controls not from this hierarchy
		
		// firstly find control in list and erase it
		List<UIControl*>::iterator it = childs.begin();
		for(; it != childs.end(); it++)
		{
			if((*it) == _control)
			{
				childs.erase(it);
				isIteratorCorrupted = true;
				break;
			}
		}
		// after that find place where we should put the control and do that
		it = childs.begin();
		for(; it != childs.end(); it++)
		{
			if((*it) == _belowThisChild)
			{
				childs.insert(it, _control);
				isIteratorCorrupted = true;
				return;
			}
		}
		DVASSERT_MSG(0, "Control _belowThisChild not found");
	}
	
	void UIControl::SendChildAbove(UIControl * _control, UIControl * _aboveThisChild)
	{
		//TODO: Fix situation when controls not from this hierarhy

		// firstly find control in list and erase it
		List<UIControl*>::iterator it = childs.begin();
		for(; it != childs.end(); it++)
		{
			if((*it) == _control)
			{
				childs.erase(it);
				isIteratorCorrupted = true;
				break;
			}
		}
		// after that find place where we should put the control and do that
		it = childs.begin();
		for(; it != childs.end(); it++)
		{
			if((*it) == _aboveThisChild)
			{
				childs.insert(++it, _control);
				isIteratorCorrupted = true;
				return;
			}
		}

		DVASSERT_MSG(0, "Control _aboveThisChild not found");
	}
	
	UIControl *UIControl::Clone()
	{
		UIControl *c = new UIControl(Rect(relativePosition.x, relativePosition.y, size.x, size.y));
		c->CopyDataFrom(this);
		return c;
	}
	
	void UIControl::CopyDataFrom(UIControl *srcControl)
	{
		relativePosition = srcControl->relativePosition;
		size = srcControl->size;
		pivotPoint = srcControl->pivotPoint;
		scale = srcControl->scale;
		angle = srcControl->angle;
		SafeRelease(background);
		background = srcControl->background->Clone();
        
        tag = srcControl->GetTag();

		needToRecalcFromAbsoluteCoordinates = srcControl->needToRecalcFromAbsoluteCoordinates;

		controlState = srcControl->controlState;
		visible = srcControl->visible;
		inputEnabled = srcControl->inputEnabled;
		clipContents = srcControl->clipContents;
		
		SafeRelease(eventDispatcher);
		if(srcControl->eventDispatcher)
		{
			eventDispatcher = new EventDispatcher();
			eventDispatcher->CopyDataFrom(srcControl->eventDispatcher);
		}
		
		RemoveAllControls();
		List<UIControl*>::iterator it = srcControl->childs.begin();
		for(; it != srcControl->childs.end(); it++)
		{
			
			UIControl *c = (*it)->Clone();
			AddControl(c);
			c->Release();
		}
	}

	
	bool UIControl::IsOnScreen()
	{
		if(parent)
		{
			return parent->IsOnScreen();
		}
		
		if(UIControlSystem::Instance()->GetScreen() == this || UIControlSystem::Instance()->GetPopupContainer() == this)
		{
			return true;
		}
		return false;
	}


	void UIControl::SystemWillAppear()
	{
		WillAppear();

		List<UIControl*>::iterator it = childs.begin();
		while(it != childs.end())
		{
			isIteratorCorrupted = false;
			UIControl *current = *it;
			current->Retain();
			current->SystemWillAppear();
			current->Release();
			if(isIteratorCorrupted)
			{
				it = childs.begin();
				continue;
			}
			it++;
		}
	}
	
	void UIControl::SystemWillDisappear()
	{
		WillDisappear();
        if (GetHover()) 
        {
            UIControlSystem::Instance()->SetHoveredControl(NULL);
        }

		List<UIControl*>::iterator it = childs.begin();
		while(it != childs.end())
		{
			isIteratorCorrupted = false;
			UIControl *current = *it;
			current->Retain();
			current->SystemWillDisappear();
			current->Release();
			if(isIteratorCorrupted)
			{
				it = childs.begin();
				continue;
			}
			it++;
		}
	}
	
	void UIControl::SystemDidAppear()
	{
		DidAppear();

		List<UIControl*>::iterator it = childs.begin();
		while(it != childs.end())
		{
			isIteratorCorrupted = false;
			UIControl *current = *it;
			current->Retain();
			current->SystemDidAppear();
			current->Release();
			if(isIteratorCorrupted)
			{
				it = childs.begin();
				continue;
			}
			it++;
		}
	}
	
	void UIControl::SystemDidDisappear()
	{
		DidDisappear();

		List<UIControl*>::iterator it = childs.begin();
		while(it != childs.end())
		{
			isIteratorCorrupted = false;
			UIControl *current = *it;
			current->Retain();
			current->SystemDidDisappear();
			current->Release();
			if(isIteratorCorrupted)
			{
				it = childs.begin();
				continue;
			}
			it++;
		}
	}
    
	void UIControl::SystemScreenSizeDidChanged(const Rect &newFullScreenRect)
    {
		ScreenSizeDidChanged(newFullScreenRect);
        
		List<UIControl*>::iterator it = childs.begin();
		while(it != childs.end())
		{
			isIteratorCorrupted = false;
			UIControl *current = *it;
			current->Retain();
			current->SystemScreenSizeDidChanged(newFullScreenRect);
			current->Release();
			if(isIteratorCorrupted)
			{
				it = childs.begin();
				continue;
			}
			it++;
		}
    }

    
	
	
	void UIControl::WillAppear()
	{
		
	}
	void UIControl::WillDisappear()
	{
		
	}
	void UIControl::DidAppear()
	{
		
	}
	void UIControl::DidDisappear()
	{
		
	}
    void UIControl::ScreenSizeDidChanged(const Rect &newFullScreenRect)
    {
        
    }
    
	

	void UIControl::SystemUpdate(float32 timeElapsed)
	{
		Update(timeElapsed);
		isUpdated = true;
		List<UIControl*>::iterator it = childs.begin();
		for(; it != childs.end(); it++)
		{
			(*it)->isUpdated = false;
		}
		
		it = childs.begin();
		while(it != childs.end())
		{
			isIteratorCorrupted = false;
			UIControl *current = *it;
			if(!current->isUpdated)
			{
				current->Retain();
				current->SystemUpdate(timeElapsed);
				current->Release();
				if(isIteratorCorrupted)
				{
					it = childs.begin();
					continue;
				}
			}
			it++;
		}
	}
	void UIControl::SystemDraw(const UIGeometricData &geometricData)
	{
		UIGeometricData drawData;
		drawData.position = relativePosition;
		drawData.size = size;
		drawData.pivotPoint = pivotPoint;
		drawData.scale = scale;
		drawData.angle = angle;
		drawData.AddToGeometricData(geometricData);
		
		if(parent)
		{
			GetBackground()->SetParentColor(parent->GetBackground()->GetDrawColor());
		}
		else 
		{
			GetBackground()->SetParentColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
		}

				
		if(clipContents)
		{//WARNING: for now clip contents don't work for rotating controls if you have any ideas you are welcome
			RenderManager::Instance()->ClipPush();
			RenderManager::Instance()->ClipRect(drawData.GetUnrotatedRect());
		}
		if(visible)
		{
			Draw(drawData);
		}
		
		if (debugDrawEnabled)
		{//TODO: Add debug draw for rotated controls
			RenderManager::Instance()->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
			RenderManager::Instance()->DrawRect(drawData.GetUnrotatedRect());
			RenderManager::Instance()->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
		}
		
		
		isIteratorCorrupted = false;
		List<UIControl*>::iterator it = childs.begin();
		for(; it != childs.end(); it++)
		{
			(*it)->SystemDraw(drawData);
			DVASSERT(!isIteratorCorrupted);
		}
		
		if(visible)
		{
			DrawAfterChilds(drawData);
		}
		if(clipContents)
		{
			RenderManager::Instance()->ClipPop();
		}
		
	}
	
	bool UIControl::IsPointInside(const Vector2 &point, bool expandWithFocus/* = false*/)
	{
		UIGeometricData gd = GetGeometricData();
		Rect rect = gd.GetUnrotatedRect();
		if(expandWithFocus)
		{
			rect.dx += CONTROL_TOUCH_AREA*2;
			rect.dy += CONTROL_TOUCH_AREA*2;
			rect.x -= CONTROL_TOUCH_AREA;
			rect.y -= CONTROL_TOUCH_AREA;
		}
		if(gd.angle != 0)
		{
			Vector2 testPoint;
			testPoint.x = (point.x - gd.position.x) * gd.cosA  + (gd.position.y - point.y) * -gd.sinA + gd.position.x;
			testPoint.y = (point.x - gd.position.x) * -gd.sinA  + (point.y - gd.position.y) * gd.cosA + gd.position.y;
			return rect.PointInside(testPoint);
		}

		return rect.PointInside(point);
	}

	bool UIControl::SystemProcessInput(UIEvent *currentInput)
	{
		if(!inputEnabled || !visible || controlState & STATE_DISABLED)
		{
			return false;
		}
		if(UIControlSystem::Instance()->GetExclusiveInputLocker()
		   && UIControlSystem::Instance()->GetExclusiveInputLocker() != this)
		{
			return false;
		}
		
		
		
		switch (currentInput->phase) 
		{
#ifndef __DAVAENGINE_IPHONE__
			case UIEvent::PHASE_KEYCHAR:
			{
					Input(currentInput);
			}
				break;
			case UIEvent::PHASE_MOVE:
			{
				if (!currentInput->touchLocker && IsPointInside(currentInput->point))
				{
                    UIControlSystem::Instance()->SetHoveredControl(this);
					Input(currentInput);
					return true;
				}
			}
				break;
#endif
			case UIEvent::PHASE_BEGAN:
			{
				if (!currentInput->touchLocker && IsPointInside(currentInput->point))
				{
#ifdef ENABLE_CONTROL_EDIT
					__touchStart = currentInput->point;
					__oldRect = relativeRect;
#endif
					if(multiInput || !currentInputID)
					{
						controlState |= STATE_PRESSED_INSIDE;
						controlState &= ~STATE_NORMAL;
						touchesInside++;
						totalTouches++;
						currentInput->controlState = UIEvent::CONTROL_STATE_INSIDE;
						
						
						PerformEventWithData(EVENT_TOUCH_DOWN, currentInput);
						currentInput->touchLocker = this;
						if(exclusiveInput)
						{
							UIControlSystem::Instance()->SetExclusiveInputLocker(this);
						}
						
						if(!multiInput)
						{
							currentInputID = currentInput->tid;
						}

						Input(currentInput);
						return true;
					}
					else 
					{
						currentInput->touchLocker = this;
						return true;
					}

				}
			}
				break;
			case UIEvent::PHASE_DRAG:
			{
				if(currentInput->touchLocker == this)
				{
					if(multiInput || currentInputID == currentInput->tid)
					{

						if(controlState & STATE_PRESSED_INSIDE || controlState & STATE_PRESSED_OUTSIDE)
						{
#ifdef ENABLE_CONTROL_EDIT
							relativePosition = __oldPosition + currentInput->point - __touchStart;
#endif
							if (IsPointInside(currentInput->point, true))
							{
								if(currentInput->controlState == UIEvent::CONTROL_STATE_OUTSIDE)
								{
									currentInput->controlState = UIEvent::CONTROL_STATE_INSIDE;
									touchesInside++;
									if(touchesInside > 0)
									{
										controlState |= STATE_PRESSED_INSIDE;
										controlState &= ~STATE_PRESSED_OUTSIDE;
#ifndef __DAVAENGINE_IPHONE__
										controlState |= STATE_HOVER;
#endif
									}
								}
							}
							else
							{
								if(currentInput->controlState == UIEvent::CONTROL_STATE_INSIDE)
								{
									currentInput->controlState = UIEvent::CONTROL_STATE_OUTSIDE;
									touchesInside--;
									if(touchesInside == 0)
									{
										controlState |= STATE_PRESSED_OUTSIDE;
										controlState &= ~STATE_PRESSED_INSIDE;
									}
								}
							}
						}
						Input(currentInput);
					}
					return true;
				}
			}
				break;
			case UIEvent::PHASE_ENDED:
			{
				if(currentInput->touchLocker == this)
				{
					if(multiInput || currentInputID == currentInput->tid)
					{
						Input(currentInput);
						if(currentInput->tid == currentInputID)
						{
							currentInputID = 0;
						}
						if(totalTouches > 0)
						{
							totalTouches--;
							if(currentInput->controlState == UIEvent::CONTROL_STATE_INSIDE)
							{
								touchesInside--;
#ifndef __DAVAENGINE_IPHONE__
								if(totalTouches == 0)
								{
									controlState |= STATE_HOVER;
								}
#endif
							}

							currentInput->controlState = UIEvent::CONTROL_STATE_RELEASED;
						
							if(totalTouches == 0)
							{
#ifdef ENABLE_CONTROL_EDIT
								relativePosition = __oldPosition + currentInput->point - __touchStart;
								__oldPosition = relativePosition;
								__touchStart = Vector2(0.f, 0.f);
								Logger::Info("DEBUG_CONTROL_COORDINATE: Vector2(%.1f, %.1f)", relativeRect.x, relativeRect.y); 
#endif
								if (IsPointInside(currentInput->point, true))
								{
									PerformEventWithData(EVENT_TOUCH_UP_INSIDE, currentInput);
								}
								controlState &= ~STATE_PRESSED_INSIDE;
								controlState &= ~STATE_PRESSED_OUTSIDE;
								controlState |= STATE_NORMAL;
								if(UIControlSystem::Instance()->GetExclusiveInputLocker() == this)
								{
									UIControlSystem::Instance()->SetExclusiveInputLocker(NULL);
								}
							}
							else if(touchesInside <= 0)
							{
								controlState |= STATE_PRESSED_OUTSIDE;
								controlState &= ~STATE_PRESSED_INSIDE;
#ifndef __DAVAENGINE_IPHONE__
								controlState &= ~STATE_HOVER;
#endif
							}

						}
					}


					currentInput->touchLocker = NULL;
					return true;
				}
			}
				break;
		}
		
		return false;
	}

	bool UIControl::SystemInput(UIEvent *currentInput)
	{
		isUpdated = true;
		//if(currentInput->touchLocker != this)
		{
			if(clipContents && (currentInput->phase != UIEvent::PHASE_DRAG && currentInput->phase != UIEvent::PHASE_ENDED))
			{
				if(!IsPointInside(currentInput->point))
				{
					return false;
				}
			}

			List<UIControl*>::reverse_iterator it = childs.rbegin();
			for(; it != childs.rend(); it++)
			{
				(*it)->isUpdated = false;
			}
			
			it = childs.rbegin();
			while(it != childs.rend())
			{
				isIteratorCorrupted = false;
				UIControl *current = *it;
				if(!current->isUpdated)
				{
					current->Retain();
					if(current->SystemInput(currentInput))
					{
						current->Release();
						return true;
					}
					current->Release();
					if(isIteratorCorrupted)
					{
						it = childs.rbegin();
						continue;
					}
				}
				it++;
			}
		}
		return SystemProcessInput(currentInput);
	}
	
	void UIControl::SystemInputCancelled(UIEvent *currentInput)
	{
		if(currentInput->controlState != UIEvent::CONTROL_STATE_RELEASED)
		{
			totalTouches--;
		}
		if(currentInput->controlState == UIEvent::CONTROL_STATE_INSIDE)
		{
			touchesInside--;
		}

		if(touchesInside == 0)
		{
			controlState &= ~STATE_PRESSED_INSIDE;
			controlState &= ~STATE_PRESSED_OUTSIDE;
			controlState |= STATE_NORMAL;
			if(UIControlSystem::Instance()->GetExclusiveInputLocker() == this)
			{
				UIControlSystem::Instance()->SetExclusiveInputLocker(NULL);
			}
		}
		
		currentInput->controlState = UIEvent::CONTROL_STATE_RELEASED;
		if(currentInput->tid == currentInputID)
		{
			currentInputID = 0;
		}
		currentInput->touchLocker = NULL;


		InputCancelled(currentInput);
	}
    
    void UIControl::SystemDidSetHovered()
    {
        controlState |= STATE_HOVER;
        PerformEventWithData(EVENT_HOVERED_SET, NULL);
        DidSetHovered();
    }
    
	void UIControl::SystemDidRemoveHovered()
    {
        PerformEventWithData(EVENT_HOVERED_REMOVED, NULL);
		controlState &= ~STATE_HOVER;
        DidRemoveHovered();
    }
    
    void UIControl::DidSetHovered()
    {
    }
    
	void UIControl::DidRemoveHovered()
    {
    }
    

	void UIControl::Input(UIEvent *currentInput)
	{
		
	}
	void UIControl::InputCancelled(UIEvent *currentInput)
	{
	}

	void UIControl::Update(float32 timeElapsed)
	{
		
	}
	void UIControl::Draw(const UIGeometricData &geometricData)
	{
		background->Draw(geometricData);
	}
	void UIControl::DrawAfterChilds(const UIGeometricData &geometricData)
	{
		
	}
	
	void UIControl::LoadFromYamlNode(YamlNode * node, UIYamlLoader * loader)
	{
		YamlNode * spriteNode = node->Get("sprite");
		YamlNode * colorNode = node->Get("color");
		YamlNode * frameNode = node->Get("frame"); 
		YamlNode * rectNode = node->Get("rect");
		YamlNode * alignNode = node->Get("align");
		YamlNode * pivotNode = node->Get("pivot");
		YamlNode * colorInheritNode = node->Get("colorInherit");
        YamlNode * drawTypeNode = node->Get("drawType");
		YamlNode * angleNode = node->Get("angle");
		YamlNode * tagNode = node->Get("tag");
		
		Rect rect = GetRect();
		if (rectNode)
		{
			rect = rectNode->AsRect();
		}
		
		Sprite * sprite = 0;
		if(spriteNode)
		{
			sprite = Sprite::Create(spriteNode->AsString());
			if (rect.dx == -1.0f)rect.dx = (float32)sprite->GetWidth();
			if (rect.dy == -1.0f)rect.dy = (float32)sprite->GetHeight();
		}
		
		if(colorNode)
		{
			GetBackground()->color = loader->GetColorFromYamlNode(colorNode);
			if(!spriteNode)
			{
				GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
			}
		}
		SetRect(rect);
		
		int frame = 0;
		if (frameNode)frame = frameNode->AsInt();

		if(spriteNode)
		{
			GetBackground()->SetSprite(sprite, frame);
			SafeRelease(sprite);
		}
		
		int32 align = loader->GetAlignFromYamlNode(alignNode);
		SetSpriteAlign(align);
        //GetBackground()->SetAlign(align);
	
		YamlNode * clipNode = node->Get("clip");
		if (clipNode)
		{
			bool clipContents = loader->GetBoolFromYamlNode(clipNode, false); 
			SetClipContents(clipContents);
		}
		
		YamlNode * visibleNode = node->Get("visible");
		if(visibleNode)
		{
			bool visible = loader->GetBoolFromYamlNode(clipNode, false); 
			SetVisible(visible);
		}
		
		if (pivotNode)
		{
			pivotPoint = pivotNode->AsPoint();
		}

        YamlNode * inputNode = node->Get("noInput");

        if (inputNode)
        {
            bool inputDis = loader->GetBoolFromYamlNode(inputNode, false);
            SetInputEnabled(!inputDis, false);
        }

		if(colorInheritNode)
		{
			UIControlBackground::eColorInheritType type = (UIControlBackground::eColorInheritType)loader->GetColorInheritTypeFromNode(colorInheritNode);
			GetBackground()->SetColorInheritType(type);
		}
        
        if(drawTypeNode)
		{
			UIControlBackground::eDrawType type = (UIControlBackground::eDrawType)loader->GetDrawTypeFromNode(drawTypeNode);
			GetBackground()->SetDrawType(type);
		}

		if(angleNode)
		{
			angle = angleNode->AsFloat();
		}

		if(tagNode)
		{
			tag = tagNode->AsInt();
		}
	}
	
	Animation *	UIControl::WaitAnimation(float32 time, int32 track)
	{
		Animation * animation = new Animation(this, time, Interpolation::LINEAR);
		animation->Start(track);
		return animation;
	}
	
	Animation *	UIControl::PositionAnimation(const Vector2 & _position, float32 time, Interpolation::FuncType interpolationFunc, int32 track)
	{
		LinearAnimation<Vector2> * animation = new LinearAnimation<Vector2>(this, &relativePosition, _position, time, interpolationFunc);
		animation->Start(track);
		return animation;
	}
	
	Animation *	UIControl::SizeAnimation(const Vector2 & _size, float32 time, Interpolation::FuncType interpolationFunc, int32 track)
	{
		LinearAnimation<Vector2> * animation = new LinearAnimation<Vector2>(this, &size, _size, time, interpolationFunc);
		animation->Start(track);
		return animation;
	}
	
	Animation *	UIControl::ScaleAnimation(const Vector2 & newScale, float32 time, Interpolation::FuncType interpolationFunc, int32 track)
	{
		LinearAnimation<Vector2> * animation = new LinearAnimation<Vector2>(this, &scale, newScale, time, interpolationFunc);
		animation->Start(track);
		return animation;
	}

	Animation * UIControl::AngleAnimation(float32 newAngle, float32 time, Interpolation::FuncType interpolationFunc /*= Interpolation::LINEAR*/, int32 track /*= 0*/)
	{
		LinearAnimation<float32> * animation = new LinearAnimation<float32>(this, &angle, newAngle, time, interpolationFunc);
		animation->Start(track);
		return animation;
	}


	Animation * UIControl::MoveAnimation(const Rect & rect, float time, Interpolation::FuncType interpolationFunc, int32 track)
	{
		TwoVector2LinearAnimation *animation = new TwoVector2LinearAnimation(this
				, &relativePosition, Vector2(rect.x + pivotPoint.x, rect.y + pivotPoint.y)
				, &size, Vector2(rect.dx, rect.dy), time, interpolationFunc);
		animation->Start(track);
		return animation;
	}
	
	Animation *	UIControl::ScaledRectAnimation(const Rect & rect, float32 time, Interpolation::FuncType interpolationFunc, int32 track)
	{
		Vector2 finalScale(rect.dx / size.x, rect.dy / size.y);
		
		TwoVector2LinearAnimation *animation = new TwoVector2LinearAnimation(this
				, &relativePosition, Vector2(rect.x + pivotPoint.x * finalScale.x, rect.y + pivotPoint.y * finalScale.y)
				, &scale, finalScale, time, interpolationFunc);
		animation->Start(track);
		return animation;
	}
	
	Animation *	UIControl::ScaledSizeAnimation(const Vector2 & newSize, float32 time, Interpolation::FuncType interpolationFunc, int32 track)
	{
		Vector2 finalScale(newSize.x / size.x, newSize.y / size.y);
		LinearAnimation<Vector2> * animation = new LinearAnimation<Vector2>(this, &scale, finalScale, time, interpolationFunc);
		animation->Start(track);
		return animation;
	}


	void UIControl::TouchableAnimationCallback(BaseObject * caller, void * param, void *callerData)
	{
		bool * params = (bool*)param;
		SetInputEnabled(params[0], params[1]);
		delete[]params;
	}
	
	Animation * UIControl::TouchableAnimation(bool touchable, bool hierarhic/* = true*/, int32 track/* = 0*/)
	{
		//TODO: change to bool animation - Dizz
		Animation * animation = new Animation(this, 0.01f, Interpolation::LINEAR);
		bool * params = new bool[2];
		params[0] = touchable;
		params[1] = hierarhic;
		animation->AddEvent(Animation::EVENT_ANIMATION_START, Message(this, &UIControl::TouchableAnimationCallback, (void*)params));
		animation->Start(track);
		return animation;
	}
	
	void UIControl::DisabledAnimationCallback(BaseObject * caller, void * param, void *callerData)
	{
		bool * params = (bool*)param;
		SetDisabled(params[0], params[1]);
		delete[]params;
	}
	
	Animation * UIControl::DisabledAnimation(bool disabled, bool hierarhic/* = true*/, int32 track/* = 0*/)
	{
		//TODO: change to bool animation - Dizz
		Animation * animation = new Animation(this, 0.01f, Interpolation::LINEAR);
		bool * params = new bool[2];
		params[0] = disabled;
		params[1] = hierarhic;
		animation->AddEvent(Animation::EVENT_ANIMATION_START, Message(this, &UIControl::DisabledAnimationCallback, (void*)params));
		animation->Start(track);
		return animation;
	}

	void UIControl::VisibleAnimationCallback(BaseObject * caller, void * param, void *callerData)
	{
		bool * params = (bool*)param;
		SetVisible(params[0], params[1]);
		delete[]params;
	}
	
	Animation * UIControl::VisibleAnimation(bool visible, bool hierarhic/* = true*/, int32 track/* = 0*/)
	{
		//TODO: change to bool animation - Dizz
		Animation * animation = new Animation(this, 0.01f, Interpolation::LINEAR);
		bool * params = new bool[2];
		params[0] = visible;
		params[1] = hierarhic;
		animation->AddEvent(Animation::EVENT_ANIMATION_START, Message(this, &UIControl::VisibleAnimationCallback, (void*)params));
		animation->Start(track);
		return animation;
	}
	
	void UIControl::RemoveControlAnimationCallback(BaseObject * caller, void * param, void *callerData)
	{
		if(parent)
		{
			parent->RemoveControl(this);
		}
	}

	Animation *	UIControl::RemoveControlAnimation(int32 track)
	{
		Animation * animation = new Animation(this, 0.01f, Interpolation::LINEAR);
		animation->AddEvent(Animation::EVENT_ANIMATION_START, Message(this, &UIControl::RemoveControlAnimationCallback));
		animation->Start(track);
		return animation;
	}
	
	Animation *	 UIControl::ColorAnimation(const Color & finalColor, float32 time, Interpolation::FuncType interpolationFunc, int32 track)
	{
		LinearAnimation<Color> * animation = new LinearAnimation<Color>(this, &background->color, finalColor, time, interpolationFunc);
		animation->Start(track);
		return animation;
	}
	
	void UIControl::SetDebugDraw(bool _debugDrawEnabled, bool hierarchic/* = false*/)
	{
		debugDrawEnabled = _debugDrawEnabled;
		if(hierarchic)
		{	
			List<UIControl*>::iterator it = childs.begin();
			for(; it != childs.end(); ++it)
			{
				(*it)->SetDebugDraw(debugDrawEnabled, hierarchic);
			}
		}
	}

}