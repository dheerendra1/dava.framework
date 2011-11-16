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

#include "UI/UIJoypad.h"
#include "FileSystem/Logger.h"


namespace DAVA 
{
UIJoypad::UIJoypad(const Rect &rect, bool rectInAbsoluteCoordinates/* = FALSE*/)
:	UIControl(rect, rectInAbsoluteCoordinates)
,	deadAreaSize(10.0f)
,	needRecalcDigital(true)
,	needRecalcAnalog(true)
,	currentPos(Vector2(0,0))
,	mainTouch(0)
,   stick(NULL)
{
	inputEnabled = true;
}
    
UIJoypad::~UIJoypad()
{
    SafeRelease(stick);
}


const Vector2 &UIJoypad::GetDigitalPosition()
{
	if(needRecalcDigital)
	{
		RecalcDigitalPosition();
	}
	return digitalVector;
}
const Vector2 &UIJoypad::GetAnalogPosition()
{
	if(needRecalcAnalog)
	{
		RecalcAnalogPosition();
	}
	return analogVector;
}
    
void UIJoypad::CreateStickControl()
{
    if (!stick) 
    {
        stick = new UIControl(Rect(0, 0, 10, 10));
        stick->GetBackground()->SetAlign(ALIGN_HCENTER|ALIGN_VCENTER);
        stick->SetInputEnabled(false);
        stick->pivotPoint = Vector2(5, 5);
        stick->relativePosition = Vector2(size.x/2, size.y/2);
        AddControl(stick);
    }
}

void UIJoypad::RecalcDigitalPosition()
{
	needRecalcDigital = false;
	if(!currentPos.x && !currentPos.y)
	{
		digitalVector.x = 0;
		digitalVector.y = 0;
		return;
	}

	Vector2 v;
	v.x = currentPos.x;
	v.y = currentPos.y;
	float len = sqrtf(v.x*v.x + v.y*v.y);
	float ang = asinf(v.x / len);
	if(v.y > 0)
	{
		ang = PI - ang;
	}
	if(ang < 0)
	{
		ang += PI*2;
	}
	if(ang > PI*2)
	{
		ang -= PI*2;
	}
	
	if(ang > PI/8 && ang < PI - PI/8)
	{
		digitalVector.x = 1.0f;
	}
	else if(ang < PI*2 - PI/8 && ang > PI + PI/8)
	{
		digitalVector.x = -1.0f;
	}
	else
	{
		digitalVector.x = 0;
	}
	if(ang < PI/2 - PI/8 || ang > PI*2 - PI/2 + PI/8)
	{
		digitalVector.y = -1.0f;
	}
	else if(ang < PI*2 - PI/2 - PI/8 && ang > PI/2 + PI/8)
	{
		digitalVector.y = 1.0f;
	}
	else
	{
		digitalVector.y = 0;
	}
//	Logger::Info("x = %f, y = %f", digitalVector.x, digitalVector.y);
}
    
void UIJoypad::RecalcAnalogPosition()
{
	needRecalcAnalog = false;
    analogVector.x = currentPos.x/(size.x/2);
    analogVector.y = currentPos.y/(size.y/2);
//    Logger::Info("x = %f, y = %f", analogVector.x, analogVector.y);
}

void UIJoypad::SetStickSprite(Sprite *stickSprite, int32 frame)
{
    CreateStickControl();
    stick->SetSprite(stickSprite, frame);
}
    
void UIJoypad::SetStickSprite(const String &stickSpriteName, int32 frame)
{
    CreateStickControl();
    stick->SetSprite(stickSpriteName, frame);
}

void UIJoypad::Input(UIEvent *currentInput)
{
	if(!mainTouch && currentInput->phase == UIEvent::PHASE_BEGAN)
	{
		mainTouch = currentInput->tid;
	}
	
	if(mainTouch != currentInput->tid)
	{
		return;
	}
	
	if(currentInput->phase == UIEvent::PHASE_ENDED)
	{
		currentPos.x = 0;
		currentPos.y = 0;
		mainTouch = 0;
	}
	else 
	{
		Rect r = GetGeometricData().GetUnrotatedRect();//GetRect(true);
		currentPos = currentInput->point - r.GetPosition();
		
		currentPos -= Vector2(r.dx * 0.5f, r.dy * 0.5f);

		if(currentPos.x < deadAreaSize &&  currentPos.x > -deadAreaSize && currentPos.y < deadAreaSize &&  currentPos.y > -deadAreaSize)
		{
			currentPos.x = 0;
			currentPos.y = 0;
		}
        currentPos.x = Max(currentPos.x, -size.x/2);
        currentPos.x = Min(currentPos.x, size.x/2);
        currentPos.y = Max(currentPos.y, -size.y/2);
        currentPos.y = Min(currentPos.y, size.y/2);
	}
    stick->relativePosition.x = size.x/2 + currentPos.x;
    stick->relativePosition.y = size.y/2 + currentPos.y;

	needRecalcAnalog = true;
	needRecalcDigital = true;

}
void UIJoypad::InputCancelled(UIEvent *currentInput)
{
	if(currentInput->tid == mainTouch)
	{
		mainTouch = 0;

		currentPos.x = 0;
		currentPos.y = 0;

        stick->relativePosition.x = size.x/2 + currentPos.x;
        stick->relativePosition.y = size.y/2 + currentPos.y;

		needRecalcAnalog = true;
		needRecalcDigital = true;
	}
}

};



