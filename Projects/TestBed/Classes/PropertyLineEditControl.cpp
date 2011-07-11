/*
 *  TestScreen.cpp
 *  TemplateProjectMacOS
 *
 *  Created by Vitaliy  Borodovsky on 3/21/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "PropertyLineEditControl.h"

REGISTER_CLASS(PropertyLineEditControl);

PropertyLineEditControl::PropertyLineEditControl()
{
	minX = 0.0f;
	maxX = 1.0f;
	
	minY = -100.0f;
	maxY = 100.0f;
	
	values.push_back(PropertyRect(0.0f, 0.0f));
	values.push_back(PropertyRect(0.25f, -100.0f));
	values.push_back(PropertyRect(0.50f, 100.0f));
	values.push_back(PropertyRect(0.75f, 50.0f));
	values.push_back(PropertyRect(1.0f, -50.0f));

	SetClipContents(true);
	
	activeValueIndex = -1;
}

PropertyLineEditControl::~PropertyLineEditControl()
{
	
}

const Rect & PropertyLineEditControl::GetWorkZone()
{
	workZone = GetRect();
	workZone.x += 10;
	workZone.y += 10;
	workZone.dx -= 20;
	workZone.dy -= 20;
	return workZone;
}


int32 PropertyLineEditControl::FindActiveValueFromPosition(const Vector2 & absolutePoint)
{
	for (int32 k = 0; k < (int32)values.size(); ++k)
	{
		Rect valRect = RectFromPosition(CalcRealPosition(values[k]));
		if (valRect.PointInside(absolutePoint))
		{
			return k;
		}
	}
	return -1;
}


int32 PropertyLineEditControl::FindValueForInsertion(const Vector2 & absolutePoint)
{
	for (int32 k = 0; k < (int32)values.size(); ++k)
	{
		Vector2 pos = CalcRealPosition(values[k]);
		if (absolutePoint.x < pos.x)
			return k;
	}
	return values.size();
}

void PropertyLineEditControl::Input(UIEvent * touch)
{
	//UIGeometricData geoData = GetGeometricData();
	//relativePoint -= geoData.position;
	Vector2 absolutePoint = touch->point;
	
	if (touch->tid == UIEvent::BUTTON_1)
	{
		if (touch->phase == UIEvent::PHASE_BEGAN)
		{
			activeValueIndex = FindActiveValueFromPosition(absolutePoint);
		}
		
		if (touch->phase == UIEvent::PHASE_DRAG)
		{
			if (activeValueIndex != -1)
			{
				FromMouseToPoint(absolutePoint, values[activeValueIndex]);
			}
		}
		
		if (touch->phase == UIEvent::PHASE_ENDED)
		{
			activeValueIndex = -1;
		}
	}
	
	if (touch->tid == UIEvent::BUTTON_2)
	{
		if (touch->phase == UIEvent::PHASE_ENDED)
		{
			int32 k = FindActiveValueFromPosition(absolutePoint);
			if (k != -1)
			{
				values.erase( values.begin() + k );
			}else 
			{
				int32 insPos = FindValueForInsertion(absolutePoint);
				//if ((insPos >= 0) && (insPos < (int32)values.size()))
				{
					Vector<PropertyRect>::iterator r = values.insert(values.begin() + insPos, PropertyRect(0, 0));
					int n = r - values.begin();
					FromMouseToPoint(absolutePoint, values[n]);
				}
			}
		}
	}
}

void PropertyLineEditControl::Update(float32 timeElapsed)
{
	
}


void PropertyLineEditControl::FromMouseToPoint(Vector2 vec, PropertyRect & rect)
{
	Rect controlRect = GetWorkZone();
	
	controlRect.ClampToRect(vec);
	
	// go to coords of work rect
	vec.x -= controlRect.x;
	vec.y -= controlRect.y;
	
	float32 rectX = (maxX - minX) * vec.x / controlRect.dx + minX;
	float32 rectY = (maxY - minY) * vec.y / controlRect.dy + minY;
	
	PropertyRect prev(minX, minY);
	PropertyRect next(maxX, maxY);
	
	if (activeValueIndex != -1)
	{
		if (activeValueIndex > 0)
			prev = values[activeValueIndex - 1];
		if (activeValueIndex < (int32)values.size() - 1)
			next = values[activeValueIndex + 1];
	}
	
	if (rectX < prev.x)rectX = prev.x;
	if (rectX > next.x)rectX = next.x;
	
	rect.x = rectX;
	rect.y = rectY;
}

Vector2 PropertyLineEditControl::CalcRealPosition(const PropertyRect & rect, bool absolute)
{
	const Rect & controlRect = GetWorkZone();
	
	Vector2 vec;
	vec.x = (rect.x - minX) * controlRect.dx / (maxX - minX);
	vec.y = (rect.y - minY) * controlRect.dy / (maxY - minY);
	if (absolute)
	{
		vec.x += controlRect.x;
		vec.y += controlRect.y;
	}
	return vec;
}
			
Rect PropertyLineEditControl::RectFromPosition(const Vector2 & pos)
{
	Rect r;
	r.x = pos.x - 4;
	r.y = pos.y - 4;
	r.dx = 8;
	r.dy = 8;	
	return r;
}

void PropertyLineEditControl::Draw(const UIGeometricData &geometricData)
{
	RenderManager::Instance()->SetColor(1.0f, 1.0f, 1.0f, 0.3f);
	RenderHelper::Instance()->FillRect(geometricData.GetUnrotatedRect());

	RenderManager::Instance()->SetColor(1.0f, 1.0f, 1.0f, 0.9f);
	RenderHelper::Instance()->DrawRect(GetWorkZone());

	RenderManager::Instance()->SetColor(1.0f, 1.0f, 1.0f, 1.0f);

	for (int32 k = 0; k < (int32)values.size() - 1; ++k)
	{
		RenderHelper::Instance()->DrawLine(CalcRealPosition(values[k]), CalcRealPosition(values[k + 1]));
	}
	
	Vector2 vlast = CalcRealPosition(values[(int32)values.size() - 1]);
	Vector2 vlast2 = CalcRealPosition(PropertyRect(maxX, maxY));
	vlast2.y = vlast.y;
	
	RenderHelper::Instance()->DrawLine(vlast, vlast2);
	
	for (int32 k = 0; k < (int32)values.size(); ++k)
	{
		Vector2 pos = CalcRealPosition(values[k]);
		RenderHelper::Instance()->DrawRect(RectFromPosition(pos));
	}

	RenderManager::Instance()->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
}


