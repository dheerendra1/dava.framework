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

#ifndef __DAVAENGINE_UI_SLIDER_H__
#define __DAVAENGINE_UI_SLIDER_H__

#include "Base/BaseTypes.h"
#include "UI/UIControl.h"

namespace DAVA
{

class UIButton;
class UISlider : public UIControl
{
public:
	UISlider();
	
	UISlider(const Rect & rect);
	virtual ~UISlider();

	
	virtual void SetThumbSprite(Sprite * sprite, int32 frame);
	virtual void SetThumbSprite(const String & spriteName, int32 frame);
	
	virtual void SetMinSprite(Sprite * sprite, int32 frame);
	virtual void SetMinSprite(const String & spriteName, int32 frame);

	virtual void SetMaxSprite(Sprite * sprite, int32 frame);
	virtual void SetMaxSprite(const String & spriteName, int32 frame);
	
	inline float32 GetMinValue();
	inline float32 GetMaxValue();
	void SetMinMaxValue(float32 _minValue, float32 _maxValue);
	
	virtual void Draw(const UIGeometricData &geometricData);
	
	inline bool IsEventsContinuos();
	inline void SetEventsContinuos(bool isEventsContinuos);
	inline float32 GetValue();
    
    void SetMaxValue(float32 value);
    void SetMinValue(float32 value);
	void SetValue(float32 value);

    
    void SetThumb(UIControl *newThumb);
    inline UIControl *GetThumb();

	virtual void LoadFromYamlNode(YamlNode * node, UIYamlLoader * loader);
	
protected:
	bool isEventsContinuos;
	
	int32 leftInactivePart;
	int32 rightInactivePart;
	
	float32 minValue;
	float32 maxValue;
	
	float32 currentValue;

	void Input(UIEvent *currentInput);
	
	void RecalcButtonPos();

	UIControl * thumbButton;
	UIControlBackground * bgMin;
	UIControlBackground * bgMax;

	float32 clipPointRelative;
	Vector2 relTouchPoint;
	
	void InitThumb();
};
    
    
inline UIControl *UISlider::GetThumb()
{
    return thumbButton;
}


inline bool UISlider::IsEventsContinuos()
{
	return isEventsContinuos;
}

inline void UISlider::SetEventsContinuos(bool _isEventsContinuos)
{
	isEventsContinuos = _isEventsContinuos;	
}
	
inline float32 UISlider::GetValue()
{
	return currentValue;
}

inline float32 UISlider::GetMinValue()
{
	return minValue;
}
	
inline float32 UISlider::GetMaxValue()
{
	return maxValue;
}

};

#endif