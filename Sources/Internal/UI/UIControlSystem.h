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
#ifndef __DAVAENGINE_UI_CONTROL_SYSTEM_H__
#define __DAVAENGINE_UI_CONTROL_SYSTEM_H__

#include "Base/BaseTypes.h"
#include "Base/BaseMath.h"
#include "Base/Singleton.h"
#include "UI/UIControl.h"
#include "UI/UIEvent.h"
#include "UI/UIScreenTransition.h"
#include "UI/UILoadingTransition.h"
#include "UI/UIPopup.h"

#define FRAME_SKIP	5

/**
	\defgroup controlsystem	UI System
*/
namespace DAVA
{

class UIScreen;
	/**
	 \brief	UIControlSystem it's a core of the all controls work.
		ControlSystem managed all update, draw, appearence and disappearence of the controls.
		ControlSystem works with th UIScreenManager to process screen setting and switching.
		Also ControlSystem processed all user input events to the controls.
	 */
class UIControlSystem : public Singleton<UIControlSystem>
{
	friend void Core::CreateSingletons();
	
	int frameSkip;
	int transitionType;
	
	Vector<UIEvent> totalInputs;

	
protected:
	~UIControlSystem();
	/**
	 \brief Don't call this constructor!
	 */
	UIControlSystem();
			
public:

	/**
	 \brief Sets the requested screen as current.
		Screen will be seted only on the next frame.
		Previous seted screen will be removed.
	 \param[in] Screen you want to set as current
	 \param[in] Transition you want to use for the screen setting.
	 */
	void SetScreen(UIScreen *newMainControl, UIScreenTransition * transition = 0);

	/**
	 \brief Sets the requested screen as current.
	 \returns currently seted screen
	 */
	UIScreen *GetScreen();

	/**
	 \brief Instantly replace one screen to enother.
		Call this only on your own risk if you are really know what you need. 
		May cause to abnormal behavior!
		Internally used by UITransition.
	 \param[in] Screen you want to set as current.
	 */
	void ReplaceScreen(UIScreen *newMainControl);

	/**
	 \brief Adds new popup to the popup container.
	 \param[in] Popup control to add.
	 */
	void AddPopup(UIPopup *newPopup);

	/**
	 \brief Removes popup from the popup container.
	 \param[in] Popup control to remove.
	 */
	void RemovePopup(UIPopup *newPopup);

	/**
	 \brief Removes all popups from the popup container.
	 */
	void RemoveAllPopups();
	
	/**
	 \brief Returns popups container.
		User can manage this container manually (change popup sequence, removes or adds popups)
	 \returns popup container
	 */
	UIControl *GetPopupContainer();
	
	/**
	 \brief Disabled all controls inputs.
		Locking all inputs if input is unlocked or incrementing lock counter.
	 \returns current lock input counter
	 */
	int32 LockInput();
	
	/**
	 \brief Enabling all controls inputs.
	 Decrementing lock counter if counter is zero unlocking all inputs.
	 \returns current lock input counter
	 */
	int32 UnlockInput();
	
	/**
	 \brief Returns lock input counter.
	 \returns current lock input counter
	 */
	int32 GetLockInputCounter() const;

	/**
	 \brief Cancel all inputs for the requested control.
	 \param[in] control to cancel inputs for.
	 */
	void CancelInputs(UIControl *control);

	/**
	 \brief Cancel requested input.
	 \param[in] event to cancel.
	 */
	void CancelInput(UIEvent *touch);

	/**
	 \brief Cancelling all current inputs.
	 */
	void CancelAllInputs();
	
	/**
	 \brief Sets the current screen to 0 LOL.
	 */
	void Reset();

    /// TODO: touchType to be removed?
	/**
	 \brief Calls by the system for input processing.
	 */
	void OnInput(int32 touchType, const Vector<UIEvent> &activeInputs, const Vector<UIEvent> &allInputs, bool fromReplay = false);
	
	/**
	 \brief Callse very frame by the system for update.
	 */
	void Update();

	/**
	 \brief Calls every frame by the system for draw.
		Draws all controls hierarchy to the screen.
	 */
	void Draw();
	
//	void SetTransitionType(int newTransitionType);
	
			
	/**
	 \brief Returns all currently active inputs.
	 \returns all inputs active in the system
	 */
	const Vector<UIEvent>  &GetAllInputs(); 
	
	/**
	 \brief Sets requested control as a exclusive input locker.
	 All inputs goes only to the exclusive input locker if input locker is present.
	 \param[in] control to set the input locker.
	 */
	void SetExclusiveInputLocker(UIControl *locker);

	/**
	 \brief Returns current exclusive input locker. Returns NULL if exclusive input locker is not present.
	 \returns exclusive input locker
	 */
	UIControl *GetExclusiveInputLocker();

	/**
	 \brief Returns base geometric data seted in the system.
		Base GeometricData is usually has parameters looks a like:
		baseGeometricData.position = Vector2(0, 0);
		baseGeometricData.size = Vector2(0, 0);
		baseGeometricData.pivotPoint = Vector2(0, 0);
		baseGeometricData.scale = Vector2(1.0f, 1.0f);
		baseGeometricData.angle = 0;
		But system can change this parameters for the 
		specific device capabilities.
	 
	 \returns GeometricData uset for the base draw
	 */
	const UIGeometricData &GetBaseGeometricData();
	
	/**
	 \brief Sets input with the requested ID to the required control.
		Input removes from the current owner. OnInputCancel() calls for the old control.  
		New control starts to handle all input activities.
	 \param[in] Input ID. Can be found in the UIEvent:tid.
	 \param[in] Control that should handle the input.
	 */
	void SwitchInputToControl(int32 eventID, UIControl *targetControl);
	
	/**
	 \brief Sets input area size.
		Used to convert input size to the virtual size. 
		Usually used by the system internally.
		Shell call CalculateScaleMultiplliers() for changes start to work.
	 \param[in] Input ID. Can be found in the UIEvent:tid.
	 \param[in] Control that should handle the input.
	 */
	void SetInputScreenAreaSize(int32 width, int32 height);
	
	/**
	 \brief Recalculate multipliers for the control system to convert incoming inputs into virtual coordinates.
		Usually called by the system internally.
	 */
	void CalculateScaleMultipliers();

	/**
	 \brief Used internally by Replay class
	 */
	void ReplayEvents();

	/**
	 \brief Called by the core when screen size is changed
	 */
    void ScreenSizeChanged();


	/**
	 \brief Called by the control to set himself as the hovered control
	 */
    void SetHoveredControl(UIControl *newHovered);

	/**
	 \brief Returns control hovered by the mnouse for now
	 */
    UIControl *GetHoveredControl(UIControl *newHovered);
    
private:
	
	

	void ProcessScreenLogic();
	
	void RecalculatePointToVirtual(const Vector2 &physicalPoint, Vector2 &virtualPoint);
	
	UIScreen * currentScreen;
	UIScreen * nextScreen;
	UIScreen * prevScreen;

	bool removeCurrentScreen;
	
	UIControl *exclusiveInputLocker;
    UIControl *hovered;

	UIControl * popupContainer;
	Vector<UIPopup*> popupsToRemove;
	
	int32 lockInputCounter;
	
	UIScreenTransition * transition;
	
	UIGeometricData baseGeometricData;
	
	int32 inputWidth;
	int32 inputHeight;
	float32 scaleFactor;
	Vector2 inputOffset;
	
	friend class UIScreenTransition;
	friend class UIScreenManager;
};
};

#endif
