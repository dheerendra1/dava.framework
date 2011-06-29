/*
*  UIScrollBar.h
*  Framework
*
*  Created by Alexey Prosin on 6/7/11.
*  Copyright 2011 __MyCompanyName__. All rights reserved.
*
*/

#ifndef __DAVAENGINE_UI_SCROLL_BAR_H__
#define __DAVAENGINE_UI_SCROLL_BAR_H__

#include "Base/BaseTypes.h"
#include "UI/UIControl.h"

#define MINIMUM_SLIDER_SIZE     30

namespace DAVA 
{

class UIScrollBar;
class UIScrollBarDelegate 
{
public:
    friend class UIScrollBar;
    virtual float32 VisibleAreaSize(UIScrollBar *forScrollBar) = 0;
    virtual float32 TotalAreaSize(UIScrollBar *forScrollBar) = 0;
    virtual float32 ViewPosition(UIScrollBar *forScrollBar) = 0;
    virtual void OnViewPositionChanged(UIScrollBar *byScrollBar, float32 newPosition) = 0;
};



class UIScrollBar : public UIControl
{//TODO: add top and bottom buttons
public:
    enum eScrollOrientation 
    {
            ORIENTATION_VERTICAL = 0
        ,	ORIENTATION_HORIZONTAL
    };
    
    UIScrollBar();
    UIScrollBar(const Rect &rect, eScrollOrientation requiredOrientation, bool rectInAbsoluteCoordinates = false);
    
    virtual ~UIScrollBar();
    
    void SetDelegate(UIScrollBarDelegate *newDelegate);
    UIControl *GetSlider();
    
    virtual void Draw(const UIGeometricData &geometricData);

    void Input(UIEvent *currentInput);

    
private:
    int32 orientation;
    UIScrollBarDelegate *delegate;
    
    UIControl *slider;
    
    bool resizeSliderProportionally;
    
};



};



#endif