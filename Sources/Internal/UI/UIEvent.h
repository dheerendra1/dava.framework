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

#ifndef __DAVAENGINE_UI_EVENT_H__
#define __DAVAENGINE_UI_EVENT_H__

#include "Base/BaseTypes.h"
#include "Base/BaseMath.h"
#include "Base/BaseObject.h"

namespace DAVA
{
	
class UIControl;
/**
\ingroup controlsystem
\brief User input representation.
	Represent user input event used in the control system. Contains all input data.
*/
class UIEvent
{
public:
	/**
	 \enum Control state bits.
	 */
	enum eInputPhase
	{
			PHASE_BEGAN = 0	//!<Screen touch or mouse button press is began.
		,	PHASE_DRAG		//!<User moves mouse with presset button or finger over the screen.
		,	PHASE_ENDED		//!<Screen touch or mouse button press is ended.
#ifndef __DAVAENGINE_IPHONE__
		,	PHASE_MOVE		//!<Mouse move event. Mouse moves without pressing any buttons. Works only with mouse controller.
#endif
		,	PHASE_CANCELLED	//!<Event was cancelled by the platform or by the control system for the some reason.

        ,	PHASE_KEYCHAR	//!<Event is a keyboard key pressing event.
	};
	
	/**
	 \enum Internal Control Sytem event activity state.
	 */
	enum eInputActivityState
	{
			ACTIVITY_STATE_INACTIVE	=	0	
		,	ACTIVITY_STATE_ACTIVE
		,	ACTIVITY_STATE_CHANGED
	};
	
	/**
	 \enum Input state accordingly to control.
	 */
	enum eControlInputState
	{
			CONTROL_STATE_RELEASED	=	0	//!<Input is released
		,	CONTROL_STATE_INSIDE			//!<Input processed inside control rerct for now
		,	CONTROL_STATE_OUTSIDE			//!<Input processed outside of the control rerct for now
	};
	
	enum eButtonID 
	{
			BUTTON_NONE	= 0
		,	BUTTON_1
		,	BUTTON_2
		,	BUTTON_3
	};
	
	int32 tid;//!< event id, for the platforms with mouse this id means mouse button id
	Vector2 point;//!< point of pressure in virtual coordinates
	Vector2 physPoint;//!< point of pressure in physical coordinates
	float64 timestamp;//!< time stemp of the event occurrence
	int32 phase;//!< began, ended, moved. See eInputPhase
	UIControl *touchLocker;//!< control that handles this input
	int32 activeState;//!< state of input in control system (active, inactive, changed)
	int32 controlState;//!< input state relative to control (outside, inside). Used for point inputs only(mouse, touch)
	int32 tapCount;//!< count of the continuous inputs (clicks for mouse)
	char16 keyChar;//!< unicode/translated character produced by key using current language, caps etc. Used only with PHASE_KEYCHAR.

	UIEvent()
	{
		activeState = ACTIVITY_STATE_INACTIVE;
		touchLocker = NULL;
		tid = 0;
		controlState = CONTROL_STATE_RELEASED;
		tapCount = 0;
	}
};
};

#endif
