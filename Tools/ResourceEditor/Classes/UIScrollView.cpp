/*
    Copyright (c) 2008, DAVA Consulting, LLC
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 
    * Created by Vitaliy Borodovsky
*/


#include "UIScrollView.h"
#include "Base/ObjectFactory.h"

namespace DAVA 
{
	
REGISTER_CLASS(UIScrollView);

const float32 UIScrollView::SCROLL_BEGIN_PIXELS = 8.0f;

UIScrollView::UIScrollView(const Rect &rect, const Vector2 &_contentSize, bool rectInAbsoluteCoordinates/* = false*/)
:	UIControl(rect, rectInAbsoluteCoordinates),
	contentSize(_contentSize),
	state(STATE_NONE),
	positionIndex(0),
	zoomScale(1.0f),
	scrollOrigin(0, 0),
	scrollZero(0, 0),
	lastTapTime(0),
	touchStartTime(0)
{
	inputEnabled = true;
	multiInput = true;
	SetClipContents(true);
	//SetDebugDraw(true, true);
	
	prevZoomScale = minScale = maxScale = zoomScale;
}

UIScrollView::~UIScrollView()
{
}

void UIScrollView::SetScales(float32 _minScale, float32 _maxScale)
{
	minScale = _minScale;
	maxScale = _maxScale;
}

void UIScrollView::SetScale(float currentScale)
{ 
	zoomScale = currentScale;
	
	scrollZero = Vector2(0.f, 0.f);
	
	float32 adjW = contentSize.dx*zoomScale;
	float32 adjH = contentSize.dy*zoomScale;
	
	if(adjW < size.x)
	{
		scrollZero.x = (size.x - adjW)/2.f;
	}
	
	if(adjH < size.y)
	{
		scrollZero.y = (size.y - adjH)/2.f;
	}
}
	
void UIScrollView::PerformScroll()
{
	Vector2 clickEndPosition = clickStartPosition;
	clickEndPosition.x -= (scrollOrigin.x);
	clickEndPosition.y -= (scrollOrigin.y);
	
	clickEndPosition.x /= zoomScale;
	clickEndPosition.y /= zoomScale;
	
	UIEvent modifiedTouch = scrollTouch;
	modifiedTouch.phase = UIEvent::PHASE_ENDED;
	modifiedTouch.point = clickEndPosition;
	
	ScrollTouch(&modifiedTouch);
}

void UIScrollView::Update(float32 timeElapsed)
{
	//Logger::Info("%f %f", scrollOrigin.x, scrollOrigin.y);
	
	if(touchStartTime)
	{
		uint64 delta = SystemTimer::Instance()->AbsoluteMS() - touchStartTime;
		if(delta > TOUCH_BEGIN_MS)
		{
			touchStartTime = 0;
		
			PerformScroll();
		}
	}
	
	if(state == STATE_DECCELERATION)
	{
		float32 timeElapsed = SystemTimer::Instance()->FrameDelta();
		
		scrollOrigin.x += deccelerationSpeed.x * scrollPixelsPerSecond * timeElapsed;
		scrollOrigin.y += deccelerationSpeed.y * scrollPixelsPerSecond * timeElapsed;
		
		scrollPixelsPerSecond *= 0.7f;
		
		if (scrollPixelsPerSecond <= 0.1f)
		{
			scrollPixelsPerSecond = 0.f;
			state = STATE_NONE;
		}
	}
	
	if(state != STATE_ZOOM && state != STATE_SCROLL) 
	{
		//hcenter
		if((scrollOrigin.x > 0) && ((scrollOrigin.x + contentSize.dx * zoomScale) < (size.x)))
		{
			float32 delta = scrollOrigin.x-scrollZero.x;
			scrollOrigin.x -= delta * timeElapsed * 5;
		}
		else if(scrollOrigin.x > 0)
		{
			scrollOrigin.x -= (scrollOrigin.x) * timeElapsed * 5;
		}
		else if((scrollOrigin.x + contentSize.dx * zoomScale) < (size.x))
		{
			scrollOrigin.x += ((size.x) - (scrollOrigin.x + contentSize.dx * zoomScale)) * timeElapsed * 5;
		}
		
		//vcenter
		if((scrollOrigin.y > 0) && ((scrollOrigin.y + contentSize.dy * zoomScale) < (size.y)))
		{
			float32 delta = scrollOrigin.y-scrollZero.y;
			scrollOrigin.y -= delta * timeElapsed * 5;
		}
		else if(scrollOrigin.y > 0)
		{
			scrollOrigin.y -= (scrollOrigin.y) * timeElapsed * 5;
		}
		else if((scrollOrigin.y + contentSize.dy * zoomScale) < (size.y))
		{
			scrollOrigin.y += ((size.y) - (scrollOrigin.y + contentSize.dy * zoomScale)) * timeElapsed * 5;
		}
	} 
	
	float32 invScale = 1.0f / zoomScale;
	scrollPosition.x = (scrollOrigin.x + scrollCurrentShift.x) * invScale;
	scrollPosition.y = (scrollOrigin.y + scrollCurrentShift.y) * invScale;
	
	//scrolling over the edge
	drawScrollPos.x = scrollCurrentShift.x;
	drawScrollPos.y = scrollCurrentShift.y;
	
//	if((scrollOrigin.x+scrollCurrentShift.x) > 0)
//	{
//		float32 dx = (scrollOrigin.x +scrollCurrentShift.x);
//		drawScrollPos.x = scrollCurrentShift.x - 2.*dx/3.;
//	}	
//	if((scrollOrigin.y + scrollCurrentShift.y) > 0)
//	{
//		float32 dy = (scrollOrigin.y +scrollCurrentShift.y);
//		drawScrollPos.y = scrollCurrentShift.y - 2.*dy/3.;
//	}
//	if((scrollOrigin.x + scrollCurrentShift.x + contentSize.dx * zoomScale) < relativeRect.dx)
//	{
//		float32 dx = (scrollOrigin.x + scrollCurrentShift.x + contentSize.dx * zoomScale) - relativeRect.dx;
//		drawScrollPos.x = scrollCurrentShift.x - 2.*dx/3.;
//	}
//	if((scrollOrigin.y + scrollCurrentShift.y + contentSize.dy * zoomScale) < relativeRect.dy)
//	{
//		float32 dy = (scrollOrigin.y + scrollCurrentShift.y + contentSize.dy * zoomScale) - relativeRect.dy;
//		drawScrollPos.y = scrollCurrentShift.y - 2.*dy/3.;
//	}
}

void UIScrollView::StartScroll(Vector2 _startScrollPosition)
{
	scrollStartInitialPosition = _startScrollPosition;
	scrollStartPosition = _startScrollPosition;
	scrollCurrentPosition = _startScrollPosition;
	scrollStartMovement = false;
	
	lastMousePositions[positionIndex & (MAX_MOUSE_POSITIONS - 1)] = _startScrollPosition;
	positionIndex++;
	positionIndex &= (MAX_MOUSE_POSITIONS - 1);
}

void UIScrollView::ProcessScroll(Vector2 _currentScrollPosition)
{
	scrollCurrentPosition = _currentScrollPosition;
	
	Vector2 lineLenght = scrollCurrentPosition - scrollStartInitialPosition;

	/*
		This check is required on iPhone, to avoid bugs in movement.
	 */
#if defined(__DAVAENGINE_IPHONE__)
	if (lineLenght.Length() >= SCROLL_BEGIN_PIXELS)
#endif
	{
		touchStartTime = 0;
		if (!scrollStartMovement)scrollStartPosition = scrollCurrentPosition;
		scrollCurrentShift = Vector2((scrollCurrentPosition.x - scrollStartPosition.x),  (scrollCurrentPosition.y - scrollStartPosition.y));
		scrollStartMovement = true;
	}

	lastMousePositions[positionIndex & (MAX_MOUSE_POSITIONS - 1)] = _currentScrollPosition;
	positionIndex++;
	positionIndex &= (MAX_MOUSE_POSITIONS - 1);
}

void UIScrollView::EndScroll()
{
	scrollOrigin.x += scrollCurrentShift.x;
	scrollOrigin.y += scrollCurrentShift.y;
	scrollCurrentShift.x = 0;
	scrollCurrentShift.y = 0;
}

bool UIScrollView::SystemInput(UIEvent *currentTouch)
{
	if(currentTouch->touchLocker != this)
	{
		if(clipContents && currentTouch->phase == UIEvent::PHASE_BEGAN)
		{
			if(!GetRect(TRUE).PointInside(currentTouch->point))
			{
				return FALSE;
			}
		}
//		List<UIControl*>::iterator it = childs.end();
//		it--;
//		for(; it != childs.end(); it--)
//		{
//			if((*it)->TouchControl(currentTouch))
//			{
//				return TRUE;
//			}
//		}
	}
	return SystemProcessInput(currentTouch);
}

void UIScrollView::Input(UIEvent *currentTouch)
{
//	if(currentTouch->tapCount == 2 && currentTouch->phase == UIControlTouch::PHASE_BEGAN)
//	{
//		float32 targetScale;
//		if(zoomScale < maxScale)
//		{
//			//zoom in
//			targetScale = maxScale;
//		}
//		else
//		{
//			targetScale = minScale;
//		}
//		LinearAnimation<float32> * scaleAnimation = new LinearAnimation<float32>(this, &zoomScale, targetScale, 0.5f,
//									Interpolation::EASY_IN_EASY_OUT);
//		scaleAnimation->Start(0);
//		
//		
//		{
//			float32 multScale = targetScale/zoomScale;
//			
//			if(targetScale < zoomScale)
//			{
//				LinearAnimation<float32> * xAnim = new LinearAnimation<float32>(this, &scrollOrigin.x, scrollZero.x, 0.5f, Interpolation::EASY_IN_EASY_OUT);
//				LinearAnimation<float32> * yAnim = new LinearAnimation<float32>(this, &scrollOrigin.y, scrollZero.y, 0.5f, Interpolation::EASY_IN_EASY_OUT);
//				xAnim->Start(1);
//				yAnim->Start(2);
//			}
//			else
//			{
//				Point2f adjPoint = Point2f(currentTouch->point.x*multScale, currentTouch->point.y*multScale);
//				Point2f delta = Point2f(adjPoint.x - currentTouch->point.x, adjPoint.y - currentTouch->point.y);
//				
//				LinearAnimation<float32> * xAnim = new LinearAnimation<float32>(this, &scrollOrigin.x, scrollOrigin.x-delta.x, 0.5f, Interpolation::EASY_IN_EASY_OUT);
//				LinearAnimation<float32> * yAnim = new LinearAnimation<float32>(this, &scrollOrigin.y, scrollOrigin.y-delta.y, 0.5f, Interpolation::EASY_IN_EASY_OUT);
//				
//				xAnim->Start(1);
//				yAnim->Start(2);
//			}
//		}
//	}
//  int32 saveState = state;
	
	Vector<UIEvent> touches = UIControlSystem::Instance()->GetAllInputs();
	if(1 == touches.size())
	{
		switch(currentTouch->phase) 
		{
			case UIEvent::PHASE_BEGAN:
			{
				scrollTouch = *currentTouch;
				
				Vector2 start = currentTouch->point;
				StartScroll(start);

				// init scrolling speed parameters
				scrollPixelsPerSecond = 0.0f;
				scrollStartTime = currentTouch->timestamp;//to avoid cast from uint64 to float64 SystemTimer::Instance()->AbsoluteMS();
				touchStartTime = SystemTimer::Instance()->AbsoluteMS();
				state = STATE_SCROLL;

				clickStartPosition = start;
			}
			break;
			case UIEvent::PHASE_DRAG:
			{
				if(state == STATE_SCROLL) 
				{
					if(currentTouch->tid == scrollTouch.tid)
					{
						// scrolling speed get parameters
						float64 scrollCurrentTime = currentTouch->timestamp;//SystemTimer::Instance()->AbsoluteMS();
						Vector2 scrollPrevPosition = scrollCurrentPosition;
						
						// perform scrolling
						ProcessScroll(currentTouch->point);
						
						// calculate scrolling speed
						float64 tmp = scrollCurrentTime;
						if(fabs(scrollCurrentTime - scrollStartTime) < 1e-9)
						{
							tmp += .1f;
						}
						
						Vector2 lineLength = scrollCurrentPosition - scrollPrevPosition;
						scrollPixelsPerSecond = (float32)((float64)lineLength.Length() / (tmp - scrollStartTime));
						scrollStartTime = scrollCurrentTime;
						
						///NSLog(@"pps:%f\n", scrollPixelsPerSecond);
					}
				}
			}
				break;
			case UIEvent::PHASE_ENDED:
			{
				if(state == STATE_SCROLL)
				{
					if(currentTouch->tid == scrollTouch.tid)
					{
						Vector2 scrollPrevPos = lastMousePositions[(positionIndex - 3) & (MAX_MOUSE_POSITIONS - 1)];
						Vector2 currentTouchPos = currentTouch->point; 
						
						deccelerationSpeed = Vector2(currentTouchPos.x, currentTouchPos.y);
						deccelerationSpeed.x -= scrollPrevPos.x;
						deccelerationSpeed.y -= scrollPrevPos.y;
						
						float32 deccelerationSpeedLen = sqrtf(deccelerationSpeed.x * deccelerationSpeed.x + deccelerationSpeed.y * deccelerationSpeed.y);
						if (deccelerationSpeedLen >= 0.00001f)
						{
							deccelerationSpeed.x /= deccelerationSpeedLen;
							deccelerationSpeed.y /= deccelerationSpeedLen;
						}
						else
						{
							deccelerationSpeed.x = 0.0f;
							deccelerationSpeed.y = 0.0f;
						}
						
						//Logger::Debug("Dcs: %f, %f\n", deccelerationSpeed.x, deccelerationSpeed.y);
						
						EndScroll();
						
						//scrollTouch = 0;
						if(scrollStartMovement)
						{
							state = STATE_DECCELERATION;
						}
						
						clickEndPosition = currentTouchPos;
					}
					
					if(touchStartTime)
					{
						touchStartTime = 0;
						PerformScroll();
					}
					
//					float32 clickLen = LineLength(clickStartPosition, clickEndPosition);
					
//					if (clickLen < SCROLL_BEGIN_PIXELS)
//					{			
//						clickEndPosition.x -= (scrollOrigin.x);
//						clickEndPosition.y -= (scrollOrigin.y);
//						
//						clickEndPosition.x /= zoomScale;
//						clickEndPosition.y /= zoomScale;
//						
//						UIControlTouch modifiedTouch = *currentTouch;
//						modifiedTouch.point = clickEndPosition;
//						
//						ScrollTouch(&modifiedTouch);
//					}	
				}
			}
				break;
		}
	}
	else if(2 == touches.size())
	{
		switch(currentTouch->phase) 
		{
			case UIEvent::PHASE_BEGAN:
			{
				if (state == STATE_SCROLL)
				{
					EndScroll();
					//scrollTouch = 0;
				}
				
				// init zoom parameters
				state = STATE_ZOOM;
				
				zoomTouches[0] = touches[0];
				zoomTouches[1] = touches[1];
				
				zoomStartPositions[0] = zoomTouches[0].point;
				zoomStartPositions[1] = zoomTouches[1].point;
				
				prevZoomScale = zoomScale; // save current scale to perform scaling in zoom mode
			}
			break;
			case UIEvent::PHASE_DRAG:
				if(state == STATE_ZOOM)
				{
					zoomTouches[0] = touches[0];
					zoomTouches[1] = touches[1];
					
					zoomCurrentPositions[0] = zoomTouches[0].point;
					zoomCurrentPositions[1] = zoomTouches[1].point;
					
					float initialDistance = sqrtf(
									  (zoomStartPositions[0].x - zoomStartPositions[1].x) * 
									  (zoomStartPositions[0].x - zoomStartPositions[1].x) + 
									  
									  (zoomStartPositions[0].y - zoomStartPositions[1].y) * 
									  (zoomStartPositions[0].y - zoomStartPositions[1].y));
					
					float currentDistance = sqrtf(
									  (zoomCurrentPositions[0].x - zoomCurrentPositions[1].x) * 
									  (zoomCurrentPositions[0].x - zoomCurrentPositions[1].x) + 
									  
									  (zoomCurrentPositions[0].y - zoomCurrentPositions[1].y) * 
									  (zoomCurrentPositions[0].y - zoomCurrentPositions[1].y));
					
					float32 saveZoomScale = zoomScale;
					float32 changeCoeff = initialDistance/currentDistance;
					float32 newScale = prevZoomScale * ((1.f - changeCoeff)/2.5f + 1.f);
					//float32 changeCoeff = currentDistance/initialDistance;
					//float32 newScale = prevZoomScale * changeCoeff * changeCoeff;
					if(newScale > maxScale)newScale = maxScale;
					if(newScale < minScale)newScale = minScale; 
					
					SetScale(newScale);
					
					Vector2 center = Vector2((zoomStartPositions[0].x + zoomStartPositions[1].x) / 2,
									(zoomStartPositions[0].y + zoomStartPositions[1].y) / 2);
					
					Vector2 scaleVectorOriginal = Vector2(center.x - scrollOrigin.x, center.y - scrollOrigin.y);
					Vector2 scaleVectorNew = scaleVectorOriginal;
					scaleVectorNew.x *= zoomScale / saveZoomScale;
					scaleVectorNew.y *= zoomScale / saveZoomScale;
					
					scaleVectorNew.x -= scaleVectorOriginal.x;
					scaleVectorNew.y -= scaleVectorOriginal.y;
					
					scrollOrigin.x -= scaleVectorNew.x;
					scrollOrigin.y -= scaleVectorNew.y;
				}
			break;
			case UIEvent::PHASE_ENDED:
			{
				Vector<UIEvent>::iterator it = touches.begin();
				bool zoomToScroll = false;
				for(; it != touches.end(); ++it)
				{
					if((*it).phase == UIEvent::PHASE_DRAG)
					{
						zoomToScroll = true;
						scrollTouch = *it;
					}
				}
				
				if(zoomToScroll)
				{
					//scrollTouch = *it;
					Vector2 start = scrollTouch.point;
					StartScroll(start);
					state = STATE_SCROLL;
					
					//zoomTouches[0] = 0;
					//zoomTouches[1] = 0;
				}
				else
				{
					if (state != STATE_DECCELERATION) 
					{
						state = STATE_NONE;
					}
				}
			}
			break;
		}
	}
	
//	if (saveState == state)
//	{
//		Logger::Debug("event: %d prevState: %d resultState: %d - alltouches: %d", currentTouch->phase, saveState, state, touches.size());
//	}else
//		Logger::Debug("CHANGED: event: %d prevState: %d resultState: %d - alltouches: %d", currentTouch->phase, saveState, state, touches.size());
}

void UIScrollView::SystemDraw(const UIGeometricData & geometricData)
{
	float32 invScale = 1.0f / zoomScale;
	Vector2 drawPos;
	drawPos.x = (scrollOrigin.x + drawScrollPos.x) * invScale;
	drawPos.y = (scrollOrigin.y + drawScrollPos.y) * invScale;
	
	//Logger::Debug("dp %f %f", drawPos.x, drawPos.y);
	
	//Rect2f currRect = r + _parentRect.GetPosition() + drawPos;

	UIGeometricData drawData;
	drawData.position = relativePosition;
	drawData.size = size;
	drawData.pivotPoint = pivotPoint;
	drawData.scale = scale;
	drawData.angle = angle;
	drawData.AddToGeometricData(geometricData);
	
	if(clipContents)
	{//WARNING: for now clip contents don't work for rotating controls if you have any ideas you are welcome
		RenderManager::Instance()->ClipPush();
		RenderManager::Instance()->ClipRect(drawData.GetUnrotatedRect());
	}
	
	
	if (debugDrawEnabled)
	{//TODO: Add debug draw for rotated controls
		RenderManager::Instance()->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
		RenderHelper::Instance()->DrawRect(drawData.GetUnrotatedRect());
		RenderManager::Instance()->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	}
	
	
	drawData.position = drawPos + relativePosition * invScale;
	drawData.size = size;
	drawData.pivotPoint = pivotPoint;
	drawData.scale = scale;
	drawData.angle = angle;
	drawData.AddToGeometricData(geometricData);

	
	if(visible)
	{
		Draw(drawData);
	}

	//isIteratorCorrupted = false;
	List<UIControl*>::iterator it = childs.begin();
	for(; it != childs.end(); it++)
	{
		(*it)->SystemDraw(drawData);
		//DVASSERT(!isIteratorCorrupted);
	}
	
	if(visible)
	{
		DrawAfterChilds(drawData);
	}
	if(clipContents)
	{
		RenderManager::Instance()->ClipPop();
	}
	
/*	if(clipContents)
	{
		RenderManager::ClipPush();
		RenderManager::ClipRect(relativeRect + _parentRect.GetPosition()); 
	}

	float32 invScale = 1.0f / zoomScale;
	Point2f drawPos;
	drawPos.x = (scrollOrigin.x + drawScrollPos.x) * invScale;
	drawPos.y = (scrollOrigin.y + drawScrollPos.y) * invScale;
	//Rect2f parentRect = _parentRect + drawPos;
	
	Rect2f r = relativeRect;
	r.x *= invScale;
	r.y *= invScale;
	Rect2f currRect = r + _parentRect.GetPosition() + drawPos;
	
	if(visible)
	{
		Draw(currRect);
	}
	
	List<UIControl*>::iterator it = childs.begin();
	for(; it != childs.end(); it++)
	{
		UIControl * c = *it;
		c->SystemDraw(currRect); 
	}
	if(visible)
	{
		DrawAfterChilds(currRect);
	}
	
	if(clipContents)
	{
		RenderManager::ClipPop();
	}
	*/
}
	
	
void UIScrollView::LoadFromYamlNode(YamlNode * node, UIYamlLoader * loader)
{
	UIControl::LoadFromYamlNode(node, loader);
	
	YamlNode *contentSizeNode = node->Get("contentSize");
	SetContentSize(contentSizeNode->AsPoint());
}		

}
