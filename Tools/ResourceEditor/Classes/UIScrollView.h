/*
 *  ScrollView.h
 *  MahJongQuest
 *
 *  Created by Dizz on 11/13/09.
 *  Copyright 2009 1. All rights reserved.
 *
 */

#ifndef __DAVAENGINE_UISCROLLVIEW_H__
#define __DAVAENGINE_UISCROLLVIEW_H__

#include "DAVAEngine.h"

namespace DAVA 
{

class UIScrollView : public UIControl
{
public:
	UIScrollView(const Rect &rect = Rect(), const Vector2 &contentSize = Vector2(), bool rectInAbsoluteCoordinates = false);
	virtual ~UIScrollView();
	
	void SetScales(float32 minScale, float32 maxScale);
	void SetContentSize(const Vector2 &_contentSize)  { contentSize = _contentSize; }
	void SetScale(float currentScale);
	void SetOffset(const Vector2 & offset) { scrollOrigin = offset; }
	
protected:
	virtual void Update(float32 timeElapsed);
	virtual bool SystemInput(UIEvent *currentTouch);
	virtual void Input(UIEvent *currentTouch);
	virtual void SystemDraw(const UIGeometricData & geometricData);
	virtual void LoadFromYamlNode(YamlNode * node, UIYamlLoader * loader);

	//called on TOUCH_UP which is now scroll or zoom event
	virtual void		ScrollTouch(UIEvent *currentTouch) {};

	enum 
	{
		STATE_NONE = 0,
		STATE_SCROLL,
		STATE_ZOOM,
		STATE_DECCELERATION,
		STATE_SCROLL_TO_SPECIAL,
	};
	
	
	
	void		StartScroll(Vector2 startScrollPosition);
	void		ProcessScroll(Vector2 currentScrollPosition);
	void		EndScroll();
	void		PerformScroll();
	
	Vector2		contentSize;
	
	int32		state;
	Vector2		scrollPosition; // Used only during rendering process
	Vector2		scrollOrigin;
	Vector2		scrollCurrentShift;	
	Vector2		scrollZero; //point of autoscroll aim
	// Click information
	Vector2		clickStartPosition;
	Vector2		clickEndPosition;
	// Scroll information
	Vector2		scrollStartInitialPosition;	// position of click
	Vector2		scrollStartPosition;		// position related to current scroll start pos, can be different from scrollStartInitialPosition
	Vector2		scrollCurrentPosition;	// scroll current position
	Vector2		drawScrollPos;
	bool		scrollStartMovement;
	UIEvent		scrollTouch;
	float64		scrollStartTime;
	uint64		touchStartTime;
	float32		scrollPixelsPerSecond;
	Vector2		deccelerationSpeed;
	float32		deccelerationTime;
	
	//zoom
	UIEvent	zoomTouches[2];
	Vector2		zoomStartPositions[2];
	Vector2		zoomCurrentPositions[2];
	float32		prevZoomScale;
	float32		zoomScale;
	float32		minScale;
	float32		maxScale;
	
	static const int32		MAX_MOUSE_POSITIONS = 16;
	static const float32	SCROLL_BEGIN_PIXELS;
	static const uint64		TOUCH_BEGIN_MS = 250;
	
	Vector2		lastMousePositions[MAX_MOUSE_POSITIONS];
	int32			positionIndex;
	
	uint64		lastTapTime;

};
};

#endif //__DAVAENGINE_UISCROLLVIEW__