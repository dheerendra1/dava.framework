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

#ifndef __DAVAENGINE_UI_CONTROL_H__
#define __DAVAENGINE_UI_CONTROL_H__

#include "Base/BaseTypes.h"
#include "Base/BaseMath.h"
#include "Base/Message.h"
#include "Base/BaseObject.h"
#include "Base/EventDispatcher.h"

#include "UI/UIControlBackground.h"
#include "UI/UIEvent.h"
#include "Animation/AnimatedObject.h"
#include "Animation/Animation.h"
#include "Animation/Interpolation.h"
#include "FileSystem/YamlParser.h"
#include "UI/UIYamlLoader.h"
#include "DAVAConfig.h"

namespace DAVA
{
#define CONTROL_TOUCH_AREA		15
	/**
	 \ingroup controlsystem
	 \brief Compound of geometric transformations used to draw control in the screen space.
	 */
	
class UIGeometricData
{
	
public:
	UIGeometricData()
	{
		cosA = 1.0f;
		sinA = 0;
		oldAngle = 0;
		angle = 0;
	}
	Vector2 position;
	Vector2 size;
	
	Vector2 pivotPoint;
	Vector2	scale;
	float32	angle;
	
	float32 cosA;
	float32 sinA;
	
	void AddToGeometricData(const UIGeometricData &data)
	{
		position.x = data.position.x - data.pivotPoint.x * data.scale.x + position.x * data.scale.x;
		position.y = data.position.y - data.pivotPoint.y * data.scale.y + position.y * data.scale.y;
		if(data.angle != 0)
		{
			float tmpX = position.x;
			position.x = (tmpX - data.position.x) * data.cosA  + (data.position.y - position.y) * data.sinA + data.position.x;
			position.y = (tmpX - data.position.x) * data.sinA  + (position.y - data.position.y) * data.cosA + data.position.y;
		}
		scale.x *= data.scale.x;
		scale.y *= data.scale.y;
		angle += data.angle;
		if(angle != oldAngle)
		{
			oldAngle = angle;
			if(angle != data.angle)
			{
				cosA = cosf(angle);
				sinA = sinf(angle);
			}
			else 
			{
				cosA = data.cosA;
				sinA = data.sinA;
			}

		}

		unrotatedRect.x = position.x - pivotPoint.x * scale.x;
		unrotatedRect.y = position.y - pivotPoint.y * scale.y;
		unrotatedRect.dx = size.x * scale.x;
		unrotatedRect.dy = size.y * scale.y;
	}

	const Rect &GetUnrotatedRect() const
	{
		return unrotatedRect;
	}
	
private:
	Rect unrotatedRect;
	float32 oldAngle;

};


	/**
	 \ingroup controlsystem
	 \brief Base control system unit.
		Responsible for update, draw and user input processing.
		
		Methods call sequence:
		When the control adds to the hierarchy:

			-if hierarchy is allready on the screen SystemWillAppear() will be called. SystemWillAppear() 
				calls WillAppear() for the control and then calls SystemWillAppear() for all control children.
			
			-when the control adding to the hierarchy is done SystemDidAppear() and DidAppear() calls at the same way.
			
			-if hierarchy is not on the screen all methods would be called only when the hierarcy parent 
				be placed to the screen.

		When the control removes from hierarchy:

			-SystemWillDisappear() will be called. SystemWillDisappear() 
				calls WillDisappear() for the control and then calls SystemWillDisappear() for all control children.
			
			-when the control is removed from the hierarchy SystemDidDisappear() and DidDisappear() calls at the same way.
		
		Every frame:

			-SystemUpdate() is calls. SystemUpdate() calls Updadte() for the control then calls SystemUpdate()
				for the all control children.
			
			-SystemDraw() is calls. SystemDraw() calculates current control geometric data. Transmit information 
				about the parent color to the control background. Sets clip if requested. Calls Draw(). 
				Calls SystemDraw() for the all control children. Calls DrawAfterChilds(). Returns clip back.
				Draw() method proceed control background drawing by default.
				You can't remove, add or sort controls on the draw step.
	 
		Every input:

			-SystemInput() is calls. At the first control process SystemInput() for all their children. If one 
				of the children returns true from their SystemInput(), control is returns true too. If no one
				of the children returns true control is returns result of the SystemProcessInput() method.
			
			-SystemProcessInput() method checks if the control is responsible to process current input. If input
				is possible to process, SystemProcessInput() sets system flags and calls Input() method, then returns true. 
				If input is inpossible to process SystemProcessInput() returns false.
	 
		Each control contain UIControlBackground object responsible for visual
		representation. UIControlBackground can be changed for the custom. Or you can 
		just overload Draw() method for the custom drawing.
	 */
class UIControl : public AnimatedObject
{
	friend class UIControlSystem;
public:
	/**
	 \enum Control state bits.
	 */
	enum eControlState
	{
		STATE_NORMAL			= 1 << 0,//!<Control isn't under influence of any activities.
		STATE_PRESSED_INSIDE	= 1 << 1,//!<Mouse or touch comes into control.
		STATE_PRESSED_OUTSIDE	= 1 << 2,//!<Mouse or touch comes into control but dragged outside of control.
		STATE_DISABLED			= 1 << 3,//!<Control is disabled (don't process any input). Use this state only if you want change graphical representation of the control. Don't use this state for the disabling inputs for parts of the controls hierarchy!.
		STATE_SELECTED			= 1 << 4,//!<Just a state for base control, nothing more.
		STATE_HOVER				= 1 << 5,//!<This bit is rise then mouse is over the control.
		
		STATE_LAST,
		STATE_COUNT				=	6
	};
	/**
	 \enum Control events supported by default.
	 */
	enum eEventType
	{
		EVENT_TOUCH_DOWN			= 1,//!<Trigger when mouse button or touch comes down inside the control.
		EVENT_TOUCH_UP_INSIDE		= 2,//!<Trigger when mouse pressure or touch processed by the control is released.
		EVENT_VALUE_CHANGED			= 3,//!<Used only with sliders for now. Trigger when value of the slider is changed.
		EVENT_HOVERED_SET           = 4,//!<
		EVENT_HOVERED_REMOVED       = 5,//!<
        EVENTS_COUNT
	};	
	
	friend class ControlSystem;

	
public:
	/**
	 \brief Creates control with requested size and position.
	 \param[in] rect Size and coordinates of control you want.
	 \param[in] rectInAbsoluteCoordinates Send 'true' if you want to make control in screen coordinates. 
		Rect coordinates will be recalculated to the hierarchy coordinates. 
		Warning, rectInAbsoluteCoordinates isn't properly works for now!
	 */
	UIControl(const Rect &rect = Rect(), bool rectInAbsoluteCoordinates = false);
	
	
	/**
	 \brief Returns Sprite used for draw in the current UIControlBackground object.
		You can call this function directly for the controlBackgound.
	 \returns Sprite used for draw.
	 */
	virtual Sprite* GetSprite();
	/**
	 \brief Returns Sprite frame used for draw in the current UIControlBackground object.
		You can call this function directly for the controlBackgound.
	 \returns Sprite frame used for draw.
	 */
	virtual int32 GetFrame();
	/**
	 \brief Returns draw type used for draw in the current UIControlBackground object.
		You can call this function directly for the controlBackgound.
	 \returns Draw type used for draw.
	 */
	virtual UIControlBackground::eDrawType GetSpriteDrawType();
	/**
	 \brief Returns Sprite align used for draw in the current UIControlBackground object.
		You can call this function directly for the controlBackgound.
	 \returns Sprite eAlign bit mask used for draw.
	 */
	virtual int32 GetSpriteAlign();
	/**
	 \brief Sets Sprite for the control UIControlBackground object.
	 \param[in] spriteName Sprite path-name.
	 \param[in] spriteFrame Sprite frame you want to use for draw.
	 */
	virtual void SetSprite(const String &spriteName, int32 spriteFrame);
	/**
	 \brief Sets Sprite for the control UIControlBackground object.
	 \param[in] newSprite Pointer for a Sprite.
	 \param[in] spriteFrame Sprite frame you want to use for draw.
	 */
	virtual void SetSprite(Sprite *newSprite, int32 spriteFrame);
	/**
	 \brief Sets Sprite frame you want to use for draw for the control UIControlBackground object.
	 \param[in] spriteFrame Sprite frame.
	 */
	virtual void SetSpriteFrame(int32 spriteFrame);
	/**
	 \brief Sets draw type you want to use the control UIControlBackground object.
	 \param[in] drawType Draw type to use for drawing.
	 */
	virtual void SetSpriteDrawType(UIControlBackground::eDrawType drawType);
	/**
	 \brief Sets Sprite align you want to use for draw for the control UIControlBackground object.
	 \param[in] drawAlign Sprite eAlign bit mask.
	 */
	virtual void SetSpriteAlign(int32 align);
	
	/**
	 \brief Sets background what will be used for draw. 
		Background is cloned inside control.
	 \param[in] newBg control background you want to use for draw.
	 */
	virtual void SetBackground(UIControlBackground *newBg);
	/**
	 \brief Returns current background used for draw.
	 \returns background used for draw.
	 */
	virtual UIControlBackground * GetBackground();

	
	/**
	 \brief Returns untransformed control rect.
		To get control metrics that applies all control transformation you need to use 
		geometric data received with GetGeometricData().
		Warning, rectInAbsoluteCoordinates isn't properly works for now!
	 \returns control rect.
	 */
	virtual const Rect & GetRect(bool absoluteCoordinates = false);
	/**
	 \brief Sets the untransformed control rect.
		Warning, rectInAbsoluteCoordinates isn't properly works for now!
	 \param[in] rect new control rect.
	 */
	virtual void SetRect(const Rect &rect, bool rectInAbsoluteCoordinates = false);

	
	/**
	 \brief Returns untransformed control position.
		To get control metrics that applies all control transformation you need to use 
		geometric data received with GetGeometricData().
		Warning, rectInAbsoluteCoordinates isn't properly works for now!
	 \returns control position.
	 */
	virtual const Vector2 &GetPosition(bool absoluteCoordinates = false);
	/**
	 \brief Sets the untransformed control position.
		Warning, rectInAbsoluteCoordinates isn't properly works for now!
	 \param[in] position new control position.
	 */
	virtual void SetPosition(const Vector2 &position, bool positionInAbsoluteCoordinates = false);
	/**
	 \brief Returns untransformed control size.
		To get control metrics that applies all control transformation you need to use 
		geometric data received with GetGeometricData().
	 \returns control size.
	 */
	virtual const Vector2 &GetSize();
	/**
	 \brief Sets the untransformed control size.
	 \param[in] newSize new control size.
	 */
	virtual void SetSize(const Vector2 &newSize);

	/**
	 \brief Returns actual control transformation and metrics.
	 \returns control geometric data.
	 */
	virtual const UIGeometricData &GetGeometricData();
	/**
	 \brief Sets the scaled control rect.
		This method didn't apply any changes to the control size, but recalculate control scale.
	 Warning, rectInAbsoluteCoordinates isn't properly works for now!
	 \param[in] rect new control rect.
	 */
	virtual void SetScaledRect(const Rect &rect, bool rectInAbsoluteCoordinates = false);
	
	/**
	 \brief Returns control rotation angle in radians.
	 \returns control angle in radians.
	 */
	virtual float32 GetAngle();
	/**
	 \brief Sets contol rotation angle in radians.
		Control rotates around the pivot point.
	 \param[in] angleInRad new control angle in radians.
	 */
	virtual void SetAngle(float32 angleInRad);
	
	
	/**
	 \brief Returns control visibility.
		Invisible controls don't process any inputs. But allows input processing for their children.
		Also for invisible controls didn't calls Draw() and DrawAfterChilds() methods. 
		But this methods calls for their children.
	 \returns control visibility.
	 */
	virtual bool GetVisible();
	/**
	 \brief Sets contol visibility.
		Invisible controls don't process any inputs. But allows input processing for their children.
		Also for invisible controls didn't calls Draw() and DrawAfterChilds() methods.
		But this methods calls for their children.
		It's always better to remove part of controls hierarchy from the parent then to make them invisible. 
		Visibility usually use for the single controls.	 
	 \param[in] isVisible new control visibility.
	 \param[in] hierarchic use true if you want to all control children change visiblity.
	 */
	virtual void SetVisible(bool isVisible, bool hierarchic = true);

	/**
	 \brief Returns control input processing ability.
		Be ware! Base control processing inputs by default.
	 \returns true if control pocessing inputs.
	 */
	virtual bool GetInputEnabled();
	/**
	 \brief Sets contol input processing ability.
		If input is disabled control don't process any inputs. But allows input processing for their children.
		Please use input enabling/disabling for the single controls or forthe small parts of hierarchy.
		It's always better to add transparent control that covers all screen and would process all 
		incoming inputs to prevent input processing for the all screen controls or for the large part of hierarchy.
	 Visibility usually use for the single controls.	 
	 \param[in] isEnabled is control should process inputs?
	 \param[in] hierarchic use true if you want to all control children change input ability.
	 */
	virtual void SetInputEnabled(bool isEnabled, bool hierarchic = true);
	
	/**
	 \brief Returns control enabling state.
		Disabled control don't process any inputs. But allows input processing for their children.
		Use this state only if you want change graphical representation of the control. 
		Don't use this state for the disabling inputs for parts of the controls hierarchy!
		All controls is enabled by default.
	 \returns true if control is disabled.
	 */
	virtual bool GetDisabled();
	/**
	 \brief Sets the contol enabling/disabling.
		Disabled control don't process any inputs. But allows input processing for their children.
		Use this state only if you want change graphical representation of the control. 
		Don't use this state for the disabling inputs for parts of the controls hierarchy!
		All controls is enabled by default.
	 \param[in] isDisabled is control disabled?
	 \param[in] hierarchic use true if you want to all control children change enabling/disabling.
	 */
	virtual void SetDisabled(bool isDisabled, bool hierarchic = true);
	
	/**
	 \brief Returns control selection state.
	 \returns is control selected.
	 */
	virtual bool GetSelected();
	/**
	 \brief Sets contol selection state.
		Selection state don't influence on any control activities.
	 \param[in] isSelected is control selected?
	 \param[in] hierarchic use true if you want to all control children change selection state.
	 */
	virtual void SetSelected(bool isSelected, bool hierarchic = true);

	/**
	 \brief Returns control clip contents state.
		Clip contents is disabled by default.
	 \returns true if control rect clips draw and input areas of his children.
	 */
	virtual bool GetClipContents();
	/**
	 \brief Sets clip contents state.
		If clip contents is enabled all incoming inputs for the control children processed only
		inside the control rect of parent. All children draw clips too.
		Clip contents is disabled by default.
	 \param[in] isNeedToClipContents true if control should clips all children draw and input by his rect.
	 */
	virtual void SetClipContents(bool isNeedToClipContents);

	/**
	 \brief Returns control hover state.
		Only controlsa what processed inputs may be hovered.
	 \returns control hover state is true if mouse placed over the control rect and no mous buttons is pressed.
	 */
	virtual bool GetHover();

	/**
	 \brief Is exclusive input enabled.
		If control have exlusive input enabled and this control starts to process 
		inputs. All inputs would be directed only to this control. But this control can
		process multiple number of inputs at a time.
		Exclusive input is disabled by default.
	 \returns true if control supports exclusive input.
	 */
	virtual bool GetExclusiveInput();
	/**
	 \brief Enables or disables control exclusive input.
		If control have exlusive input enabled and this control starts to process 
		inputs. All inputs would be directed only to this control. But this control can
		process multiple number of inputs at a time.
		Exclusive input is disabled by default.
	 \param[in] isExclusiveInput should control process inputs exclusively?
	 \param[in] hierarchic use true if you want to all control children change exclusive input state.
	 */
	virtual void SetExclusiveInput(bool isExclusiveInput, bool hierarchic = true);
	/**
	 \brief Checks if control is multiple input enabled.
		If multiple input is enabled control can process all incoming inputs (Two or 
		more touches for example). Otherwise control process only first incoming input.
		Multiply input is disabled by default.
	 \returns true if control supports multyple inputs.
	 */
	virtual bool GetMultiInput();
	/**
	 \brief Sets contol multi input processing.
		If multiple input is enabled control can process all incoming inputs (Two or 
		more touches for example). Otherwise control process only first incoming input.
		Multiply input is disabled by default.
	 \param[in] isMultiInput should control supports multiple inputs?
	 \param[in] hierarchic use true if you want to all control children change multi nput support state.
	 */
	virtual void SetMultiInput(bool isMultiInput, bool hierarchic = true);
	
	
	/**
	 \brief Sets the contol name.
		Later you can find control by this name. 
	 \param[in] _name new control name.
	 */
	void SetName(const String & _name);
	/**
	 \brief Returns current name of the control.
	 \returns control name.
	 */
	const String & GetName(); 

	/**
	 \brief Sets the contol tag.
	 \param[in] tag new control tag.
	 */
	void SetTag(int32 tag);
	/**
	 \brief Returns current control tag.
	 \returns control tag.
	 */
	int32 GetTag();

	/**
	 \brief Returns control with given name.
	 \param[in] name requested control name.
	 \param[in] recursive use true if you want fro recursive search.
	 \returns first control with given name.
	 */
	UIControl * FindByName(const String & name, bool recursive = true);
	
	/**
	 \brief Returns control state bit mask.
	 \returns control state.
	 */
	int32 GetState();
	/**
	 \brief Sets control state bit mask.
		Try to not use this method manually.
	 \param[in] state new control bit mask.
	 */
	void SetState(int32 state);

	/**
	 \brief Returns control parent.
	 \returns if contorl hasn't parent returns NULL.
	 */
	UIControl *GetParent();
	/**
	 \brief Returns list of control children.
	 \returns list of control children.
	 */
	const List<UIControl*> &GetChildren();

	

	/**
	 \brief Add control as a child.
		Children draws in the sequence of adding. If child has another parent 
		this child removes from the parrent firstly.
	 \param[in] control control to add.
	 */
	virtual void AddControl(UIControl *control);
	/**
	 \brief Removes control from the children list.
		If child isn't present in the method owners list nothin happens.
	 \param[in] control control to remove.
	 */
	virtual void RemoveControl(UIControl *control);
	/**
	 \brief Removes all children from the control.
	 */
	virtual void RemoveAllControls();
	/**
	 \brief Brings given child front.
		This child will be drawn at the top of the control children.
		If child isn't present in the owners list nothin happens.
	 \param[in] _control control to bring front.
	 */
	virtual void BringChildFront(UIControl *_control);
	/**
	 \brief Brings given child back.
		This child will be drawn at the bottom of the control children.
		If child isn't present in the owners list nothin happens.
	 \param[in] _control control to bring back.
	 */
	virtual void BringChildBack(UIControl *_control);
	/**
	 \brief Inserts given child before the requested.
	 \param[in] _control control to insert.
	 \param[in] _belowThisChild control to insert before. If this control isn't present in the 
		children list new child adds at the top of the list.
	 */
	virtual void InsertChildBelow(UIControl * _control, UIControl * _belowThisChild);
	/**
	 \brief Inserts given child after the requested.
	 \param[in] _control control to insert.
	 \param[in] _aboveThisChild control to insert after. If this control isn't present in the 
	 children list new child adds at the top of the list.
	 */
	virtual void InsertChildAbove(UIControl * _control, UIControl * _aboveThisChild);
	/**
	 \brief Sends given child before the requested.
		If one of the given children isn't present in the owners list nothin happens.
	 \param[in] _control control to move.
	 \param[in] _belowThisChild control to sends before.
	 */
	virtual void SendChildBelow(UIControl * _control, UIControl * _belowThisChild);
	/**
	 \brief Sends given child after the requested.
		If one of the given children isn't present in the owners list nothin happens.
	 \param[in] _control control to move.
	 \param[in] _aboveThisChild control to sends after.
	 */
	virtual void SendChildAbove(UIControl * _control, UIControl * _aboveThisChild);
	
	
	/**
	 \brief Adds callback message for the event trigger.
	 \param[in] eventType event type you want to process.
	 \param[in] msg message should be calld when the event triggered.
	 */
	void AddEvent(int32 eventType, const Message &msg);
	/**
	 \brief Removes callback message for the event trigger.
	 \param[in] eventType event type you want to remove.
	 \param[in] msg message to remove.
	 \returns true if event is removed.
	 */
	bool RemoveEvent(int32 eventType, const Message &msg);
	/**
	 \brief Function to remove all events from event dispatcher.
	 \returns true if we removed something, false if not
	 */
	bool RemoveAllEvents();
	
	/**
	 \brief Send given event to the all subscribed objects.
	 \param[in] eventType event type you want to process.
	 */
	void PerformEvent(int32 eventType);
	/**
	 \brief Send given event with given user data to the all subscribed objects.
	 \param[in] eventType event type you want to process.
	 \param[in] callerData data you want to send to the all messages.
	 */
	void PerformEventWithData(int32 eventType, void *callerData);
	
	/**
	 \brief Creates the absoulutely identic copy of the control.
	 \returns control copy.
	 */
	virtual UIControl *Clone();
	/**
	 \brief Copies all contorl parameters from the sended control.
	 \param[in] srcControl Source control to copy parameters from.
	 */
	virtual void CopyDataFrom(UIControl *srcControl);
	
	
	 //Animation helpers

	/**
	 \brief Starts wait animation for the control.
	 \param[in] time animation time.
	 \param[in] track animation track. 0 by default.
	 \returns Animation object
	 */
	Animation *		WaitAnimation(float32 time, int32 track = 0);
	/**
	 \brief Starts move and size animation for the control.
	 \param[in] rect New control position and size.
	 \param[in] time animation time.
	 \param[in] interpolationFunc time interpolation method.
	 \param[in] track animation track. 0 by default.
	 \returns Animation object
	 */
	Animation *		MoveAnimation(const Rect & rect, float32 time, Interpolation::FuncType interpolationFunc = Interpolation::LINEAR, int32 track = 0);
	/**
	 \brief Starts move and scale animation for the control. Changing scale instead of size.
	 \param[in] rect New control position and size.
	 \param[in] time animation time.
	 \param[in] interpolationFunc time interpolation method.
	 \param[in] track animation track. 0 by default.
	 \returns Animation object
	 */
	Animation *		ScaledRectAnimation(const Rect & rect, float32 time, Interpolation::FuncType interpolationFunc = Interpolation::LINEAR, int32 track = 0);
	/**
	 \brief Starts scale animation for the control. Changing scale instead of size.
	 \param[in] newSize New control size.
	 \param[in] time animation time.
	 \param[in] interpolationFunc time interpolation method.
	 \param[in] track animation track. 0 by default.
	 \returns Animation object
	 */
	Animation *		ScaledSizeAnimation(const Vector2 & newSize, float32 time, Interpolation::FuncType interpolationFunc = Interpolation::LINEAR, int32 track = 0);
	/**
	 \brief Starts control position animation.
	 \param[in] _position New control position.
	 \param[in] time animation time.
	 \param[in] interpolationFunc time interpolation method.
	 \param[in] track animation track. 0 by default.
	 \returns Animation object
	 */
	Animation *		PositionAnimation(const Vector2 & _position, float32 time, Interpolation::FuncType interpolationFunc = Interpolation::LINEAR, int32 track = 0);
	/**
	 \brief Starts control size animation.
	 \param[in] _size New control size.
	 \param[in] time animation time.
	 \param[in] interpolationFunc time interpolation method.
	 \param[in] track animation track. 0 by default.
	 \returns Animation object
	 */
	Animation *		SizeAnimation(const Vector2 & _size, float32 time, Interpolation::FuncType interpolationFunc = Interpolation::LINEAR, int32 track = 0);
	/**
	 \brief Starts control scale animation.
	 \param[in] newScale New control scale.
	 \param[in] time animation time.
	 \param[in] interpolationFunc time interpolation method.
	 \param[in] track animation track. 0 by default.
	 \returns Animation object
	 */
	Animation *		ScaleAnimation(const Vector2 & newScale, float32 time, Interpolation::FuncType interpolationFunc = Interpolation::LINEAR, int32 track = 0);
	/**
	 \brief Starts control rotation angle animation.
	 \param[in] newAngle New control rotation angle.
	 \param[in] time animation time.
	 \param[in] interpolationFunc time interpolation method.
	 \param[in] track animation track. 0 by default.
	 \returns Animation object
	 */
	Animation *		AngleAnimation(float32 newAngle, float32 time, Interpolation::FuncType interpolationFunc = Interpolation::LINEAR, int32 track = 0);
	/**
	 \brief Starts input enabling switching animation. This animation changing control 
		input enabling state on the next frame after the animation start.
	 \param[in] touchable New input enabled value.
	 \param[in] hierarhic Is value need to be changed in all coltrol children.
	 \param[in] track animation track. 0 by default.
	 \returns Animation object
	 */
	Animation *		TouchableAnimation(bool touchable, bool hierarhic = true, int32 track = 0);
	/**
	 \brief Starts control disabling animation. This animation changing control
		disable state on the next frame after the animation start.
	 \param[in] disabled New control disabling value.
	 \param[in] hierarhic Is value need to be changed in all coltrol children.
	 \param[in] track animation track. 0 by default.
	 \returns Animation object
	 */
	Animation *		DisabledAnimation(bool disabled, bool hierarhic = true, int32 track = 0);
	/**
	 \brief Starts control visible animation. This animation changing control visibility
		on the next frame after the animation start.
	 \param[in] visible New control visible value.
	 \param[in] hierarhic Is value need to be changed in all coltrol children.
	 \param[in] track animation track. 0 by default.
	 \returns Animation object
	 */
	Animation *		VisibleAnimation(bool visible, bool hierarhic = true, int32 track = 0);
	/**
	 \brief Starts control removation animation. This animation removes control from the parent
	 on the next frame  after the animation start.
	 \param[in] track animation track. 0 by default.
	 \returns Animation object
	 */
	Animation *		RemoveControlAnimation(int32 track = 0);
	/**
	 \brief Starts control color animation.
	 \param[in] New control color.
	 \param[in] animation time.
	 \param[in] time interpolation method.
	 \param[in] track animation track. 0 by default.
	 \returns Animation object
	 */
	Animation *		ColorAnimation(const Color & finalColor, float32 time, Interpolation::FuncType interpolationFunc = Interpolation::LINEAR, int32 track = 0);

protected:
	void TouchableAnimationCallback(BaseObject * caller, void * param, void *callerData);
	void DisabledAnimationCallback(BaseObject * caller, void * param, void *callerData);
	void VisibleAnimationCallback(BaseObject * caller, void * param, void *callerData);
	void RemoveControlAnimationCallback(BaseObject * caller, void * param, void *callerData);

public:

	/**
	 \brief enabling or disabling dbug draw for the control.
	 \param[in] _debugDrawEnabled New debug draw value.
	 \param[in] hierarchic Is value need to be changed in all coltrol children.
	 */
	void	SetDebugDraw(bool _debugDrawEnabled, bool hierarchic = false);
	
public:
	
	/**
	 \brief Called before control will be added to view hierarchy.
		Can be overrided for control additioanl functionality implementation.
		By default this method is empty.
	 */
	virtual void WillAppear();
	/**
	 \brief Called before control will be removed from the view hierarchy.
		Can be overrided for control additioanl functionality implementation.
		By default this method is empty.
	 */
	virtual void WillDisappear();
	/**
	 \brief Called when control added to view hierarchy.
		Can be overrided for control additioanl functionality implementation.
		By default this method is empty.
	 */
	virtual void DidAppear(); 
	/**
	 \brief Called when control removed from the view hierarchy.
		Can be overrided for control additioanl functionality implementation.
		By default this method is empty.
	 */
	virtual void DidDisappear();
	/**
	 \brief Called before control will be added to view hierarchy. 
		Internal method used by ControlSystem. Can be overriden to prevent hierarchical call.
	 */
	virtual void SystemWillAppear();
	/**
	 \brief Called before control will be removed from the view hierarchy. 
		Internal method used by ControlSystem. Can be overriden to prevent hierarchical call.
	 */
	virtual void SystemWillDisappear();
	/**
	 \brief Called when control added to view hierarchy. 
		Internal method used by ControlSystem. Can be overriden to prevent hierarchical call.
	 */
	virtual void SystemDidAppear();
	/**
	 \brief Called when control removed from the view hierarchy. 
		Internal method used by ControlSystem. Can be overriden to prevent hierarchical call.
	 */
	virtual void SystemDidDisappear();

    /**
	 \brief Called when screen size is changed. 
        This method called for the currently active screen when the screen size is changed. Or called after WillAppear() for the other screens.
        Internal method used by ControlSystem. Can be overriden to prevent hierarchical call.
	 \param[in] newFullScreenSize New full screen size in virtual coordinates. 
        Rect may be larger when the virtual screen size. Rect x and y position may be smaller when 0.
	 */
	virtual void SystemScreenSizeDidChanged(const Rect &newFullScreenRect);
    /**
	 \brief Called when screen size is changed. 
        This method called for the currently active screen when the screen size is changed. Or called after WillAppear() for the other screens.
	 \param[in] newFullScreenSize New full screen size in virtual coordinates. 
        Rect may be larger when the virtual screen size. Rect x and y position may be smaller when 0.
	 */
	virtual void ScreenSizeDidChanged(const Rect &newFullScreenRect);
    
	/**
	 \brief SystemUpdate() calls Updadte() for the control then SystemUpdate() calls for the all control children. 
		Internal method used by ControlSystem. Can be overriden to prevent hierarchical call or adjust functionality.
	 \param[in] timeElapsed Current frame time delta.
	 */
	virtual void SystemUpdate(float32 timeElapsed);
	/**
	 \brief Calls on every frame to process controls drawing.
		Firstly this method calls Draw() for the curent control. When SystemDraw() called for the every control child.
		And at the end DrawAfterChilds() called for current control.
		Internal method used by ControlSystem.
		Can be overriden to adjust draw hierarchy.
	 \param[in] geometricData Parent geometric data.
	 */
	virtual void SystemDraw(const UIGeometricData &geometricData);// Internal method used by ControlSystem
	/**
	 \brief Calls on every input event. Calls SystemInput() for all control children.
		If no one of the children is processed input. Calls ProcessInput() for the current control.
		Internal method used by ControlSystem.
	 \param[in] currentInput Input information.
	 \returns true if control processed this input.
	 */
	virtual bool SystemInput(UIEvent *currentInput);
	/**
	 \brief Process incoming input and if it's necessary calls Input() method for the control.
		Internal method used by ControlSystem.
	 \param[in] currentInput Input information.
	 \returns true if control processed this input.
	 */
	virtual bool SystemProcessInput(UIEvent *currentInput);// Internal method used by ControlSystem
	/**
	 \brief Calls when input processd by control is cancelled.
		Internal method used by ControlSystem.
	 \param[in] currentInput Input information.
	 */
	virtual void SystemInputCancelled(UIEvent *currentInput);

    /**
	 \brief Called when control is set as the hovered (by the mouse) control. 
     Internal method used by ControlSystem. Can be overriden only by the people ho knows UI architecture.
	 */
	virtual void SystemDidSetHovered();
    /**
	 \brief Called when control is not a hovered (by the mouse) control. 
     Internal method used by ControlSystem. Can be overriden only by the people ho knows UI architecture.
	 */
	virtual void SystemDidRemoveHovered();

    /**
	 \brief Called when control is set as the hovered (by the mouse) control. 
     Can be overriden to implement start hoverig reaction.
	 */
    virtual void DidSetHovered();
    /**
	 \brief Called when control is not a hovered (by the mouse) control. 
     Can be overriden to implement end hoverig reaction.
	 */
	virtual void DidRemoveHovered();

	
	/**
	 \brief Calls on every input event coming to control.
		Should be overriden to implement custom input reaction.
		During one input processing step into control may come more then one input event.
		For example: Pressing began event and pressing ended or five conituous mose move events etc. 
		Called only if control inputEnable is true.
	 \param[in] currentInput Input information.
	 */
	virtual void Input(UIEvent *currentInput);
	/**
	 \brief Calls when input processd by control is cancelled.
		Should be overriden to implement custom input cancelling reaction.
	 \param[in] currentInput Input information.
	 */
	virtual void InputCancelled(UIEvent *currentInput);
	/**
	 \brief Calls on every frame with frame delata time parameter. 
		Should be overriden to implement perframe functionality.
		Default realization is empty.
	 \param[in] timeElapsed Current frame time delta.
	 */
	virtual void Update(float32 timeElapsed);
	/**
	 \brief Calls on every frame to draw control.
		Can be overriden to implement custom draw functionality.
		Default realization is drawing UIControlBackground with requested parameters.
	 \param[in] geometricData Control geometric data.
	 */
	virtual void Draw(const UIGeometricData &geometricData);
	/**
	 \brief Calls on every frame with UIGeometricData after all children is drawed. 
		Can be overriden to implement after children drawing.
		Default realization is empty.
	 \param[in] geometricData Control geometric data.
	 */
	virtual void DrawAfterChilds(const UIGeometricData &geometricData);
	
		//TODO: Борода напиши дескрипшн.
	virtual void LoadFromYamlNode(YamlNode * node, UIYamlLoader * loader);
	/**
	 \brief Called when this control and his children are loaded.
	 */
	virtual void LoadFromYamlNodeCompleted() {};
	

	/**
	 \brief Returns control on screen status.
	 \returns True if control in view hierarchy for now.
	 */
	bool IsOnScreen();
	/**
	 \brief Returns point status realtive to control .
	 \param[in] point Point to check.
	 \param[in] expandWithFocus Is area should be expanded with focus.
	 \returns True if inside the control rect.
	 */
	virtual bool IsPointInside(const Vector2 &point, bool expandWithFocus = false);


public:

	Vector2 relativePosition;//!<position in the parent control.
	Vector2 size;//!<control size.

	Vector2 pivotPoint;//!<control pivot point. Top left control corner by default.
	Vector2	scale;//!<control scale. Scale relative to pivot point.
	float32	angle;//!<control rotation angle. Rotation around pivot point.
	

protected:
	
//	void SystemClearHoverState();//<! Internal method used by ControlSystem

	Vector2 absolutePosition;
	
	UIControl *parent;
	List<UIControl*> childs;
	UIControlBackground *background;
//	Rect absoluteRect;
	int32 controlState;
	bool visible;
	bool inputEnabled;
	bool clipContents;

	bool multiInput;
	bool exclusiveInput;
	int32 currentInputID;
	int32 touchesInside;
	int32 totalTouches;
	
	
	Rect returnedRect;
	UIGeometricData tempGeometricData;

	EventDispatcher *eventDispatcher;
	
	bool needToRecalcFromAbsoluteCoordinates;
	
	bool debugDrawEnabled;
	
	void SetParent(UIControl *newParent);
	virtual ~UIControl();
	
#ifdef ENABLE_CONTROL_EDIT
	Vector2	__touchStart;
	Vector2		__oldPosition;
#endif
	
private:
	bool isUpdated;
	bool isIteratorCorrupted;
	String	name;
	int32	tag;

};
};

#endif