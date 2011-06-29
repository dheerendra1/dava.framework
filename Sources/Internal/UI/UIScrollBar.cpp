/*
*  UIScrollBar.cpp
*  Framework
*
*  Created by Alexey Prosin on 6/7/11.
*  Copyright 2011 __MyCompanyName__. All rights reserved.
*
*/

#include "UI/UIScrollBar.h"
#include "Base/ObjectFactory.h"

namespace DAVA 
{
UIScrollBar::UIScrollBar()
{
}

UIScrollBar::UIScrollBar(const Rect &rect, eScrollOrientation requiredOrientation, bool rectInAbsoluteCoordinates/* = false*/)
:	UIControl(rect, rectInAbsoluteCoordinates)
,	delegate(NULL)
,	orientation(requiredOrientation)
,   resizeSliderProportionally(true)
{
    slider = new UIControl(Rect(0, 0, rect.dx, rect.dy));
    slider->SetInputEnabled(false, false);
    AddControl(slider);
}

UIScrollBar::~UIScrollBar()
{
    SafeRelease(slider);
}

void UIScrollBar::SetDelegate(UIScrollBarDelegate *newDelegate)
{
    delegate = newDelegate;
}

UIControl *UIScrollBar::GetSlider()
{
    return slider;
}

void UIScrollBar::Input(UIEvent *currentInput)
{
    if (!delegate) 
    {
        return;
    }
    
    if (currentInput->phase == UIEvent::PHASE_BEGAN
        || currentInput->phase == UIEvent::PHASE_DRAG
        || currentInput->phase == UIEvent::PHASE_ENDED)
    {
        float32 newPos;
        const Rect &r = GetGeometricData().GetUnrotatedRect();
        if(orientation == ORIENTATION_HORIZONTAL)
        {
            newPos = (currentInput->point.x - r.x - slider->size.x/2) * (delegate->TotalAreaSize(this) / size.x);
        }
        else 
        {
            newPos = (currentInput->point.y - r.y - slider->size.y/2) * (delegate->TotalAreaSize(this) / size.y);
        }
        
        newPos = Min(Max(0.0f, newPos), delegate->TotalAreaSize(this) - delegate->VisibleAreaSize(this));
        delegate->OnViewPositionChanged(this, newPos);
    }
}


void UIScrollBar::Draw(const UIGeometricData &geometricData)
{
    if (delegate) 
    {
        float32 visibleArea = delegate->VisibleAreaSize(this);
        float32 totalSize = delegate->TotalAreaSize(this);
        float32 viewPos = -delegate->ViewPosition(this);
        switch (orientation) 
        {
            case ORIENTATION_VERTICAL:
            {
                if (resizeSliderProportionally)
                {
                    slider->size.y = size.y * (visibleArea / totalSize);
                    if (slider->size.y < MINIMUM_SLIDER_SIZE) 
                    {
                        slider->size.y = MINIMUM_SLIDER_SIZE;
                    }
                    if (slider->size.y >= size.y) 
                    {
                        slider->SetVisible(false, true);
                    }
                    else 
                    {
                        slider->SetVisible(true, true);
                    }
                }
                    //TODO: optimize
                slider->relativePosition.y = (size.y - slider->size.y) * (viewPos / (totalSize - visibleArea));
                if (slider->relativePosition.y < 0) 
                {
                    slider->size.y += slider->relativePosition.y;
                    slider->relativePosition.y = 0;
                }
                else if(slider->relativePosition.y + slider->size.y > size.y)
                {
                    slider->size.y = size.y - slider->relativePosition.y;
                }

            }
                break;
            case ORIENTATION_HORIZONTAL:
            {
                if (resizeSliderProportionally)
                {
                    slider->size.x = size.x * (visibleArea / totalSize);
                    if (slider->size.x < MINIMUM_SLIDER_SIZE) 
                    {
                        slider->size.x = MINIMUM_SLIDER_SIZE;
                    }
                    if (slider->size.x >= size.x) 
                    {
                        slider->SetVisible(false, true);
                    }
                    else 
                    {
                        slider->SetVisible(true, true);
                    }
                }
                slider->relativePosition.x = (size.x - slider->size.x) * (viewPos / (totalSize - visibleArea));
                if (slider->relativePosition.x < 0) 
                {
                    slider->size.x += slider->relativePosition.x;
                    slider->relativePosition.x = 0;
                }
                else if(slider->relativePosition.x + slider->size.x > size.x)
                {
                    slider->size.x = size.x - slider->relativePosition.x;
                }
            }
                break;
        }
    }
    UIControl::Draw(geometricData);
}
};
