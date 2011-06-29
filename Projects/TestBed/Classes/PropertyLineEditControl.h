/*
 *  TestScreen.h
 *  TemplateProjectMacOS
 *
 *  Created by Vitaliy  Borodovsky on 3/21/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef __PROPERTY_LINE_EDIT_CONTROL_H__
#define __PROPERTY_LINE_EDIT_CONTROL_H__

#include "DAVAEngine.h"

using namespace DAVA;


class GridControl : public UIControl
{
public:	
	GridControl();
	~GridControl();
	
	virtual void WillAppear();
	virtual void Update(float32 timeElapsed);
	virtual void Draw(const UIGeometricData &geometricData);
};

class PropertyLineEditControl : public UIControl
{
public:
	PropertyLineEditControl();
	~PropertyLineEditControl();
	
	
	virtual void Update(float32 timeElapsed);
	virtual void Draw(const UIGeometricData &geometricData);
	
	virtual void Input(UIEvent * touch);

	
	class PropertyRect
	{
	public:
		PropertyRect(float32 _x, float32 _y)
		{
			x = _x;
			y = _y;
		}
		
		float32 x;
		float32 y;
		Rect GetRect();
	};
	
	
private:
	//UITextField * textField[10];
	
	Vector2 CalcRealPosition(const PropertyRect & rect, bool absolute = true);
	Rect RectFromPosition(const Vector2 & pos);
	void FromMouseToPoint(Vector2 vec, PropertyRect & rect);
	const Rect & GetWorkZone();
	int32 FindActiveValueFromPosition(const Vector2 & absolutePosition);
	int32 FindValueForInsertion(const Vector2 & absolutePosition);
	
	Rect workZone;
	
	float32 minX;
	float32 minY;
	float32 maxX;
	float32 maxY;
	
	float32 windowX;
	float32 windowY;
	
	int32 activeValueIndex;
	
	Vector<PropertyRect> values;
};

#endif // __PROPERTY_LINE_EDIT_CONTROL_H__