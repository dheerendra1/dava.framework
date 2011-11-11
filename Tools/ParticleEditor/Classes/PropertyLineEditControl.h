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

class PropertyLineEditControl;

class PropertyLineEditControlDelegate
{
public:
    virtual void OnPointAdd(PropertyLineEditControl *forControl, float32 t, float32 value) = 0;
    virtual void OnPointDelete(PropertyLineEditControl *forControl, float32 t) = 0;
    virtual void OnPointMove(PropertyLineEditControl *forControl, float32 lastT, float32 newT, float32 newV) = 0;
    virtual void OnMouseMove(PropertyLineEditControl *forControl, float32 t) = 0;
    virtual void OnPointSelected(PropertyLineEditControl *forControl, int32 index, Vector2 value) = 0;
};

class PropertyLineEditControl : public UIControl
{
public:
	PropertyLineEditControl();
	~PropertyLineEditControl();
	
	virtual void Update(float32 timeElapsed);
	virtual void Draw(const UIGeometricData &geometricData);
	
	virtual void Input(UIEvent * touch);
    
    void DeletePoint(float32 t);
    void AddPoint(float32 t, float32 value);
    void MovePoint(float32 lastT, float32 newT, float32 newV, bool changeV);
    
    void DeselectPoint();
    
    void SetMinY(float32 value);
    void SetMaxY(float32 value);
    
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
        bool operator == (const PropertyRect & p) const
		{
			return (this->x == p.x);
		}
	};
	
    void SetCurTime(float32 t);
    float32 GetCurTime();
    
	Vector<PropertyRect> & GetValues();
    PropertyLineKeyframes<float32> * GetPropertyLine();
    void SetText(WideString string);
    void SetByPropertyLine(PropertyLineKeyframes<float32> *props);
    void SetDelegate(PropertyLineEditControlDelegate *controlDelegate);
    void Reset();
    bool GetSelectedValue(Vector2 &v);
    void SetSelectedValue(Vector2 v);
    
private:
	
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
	
    float32 curTime;
    
	int32 activeValueIndex;
    int32 selectedValueIndex;
	
	Vector<PropertyRect> values;
    
    PropertyLineEditControlDelegate *delegate;
    
    UIStaticText *text;
};

#endif // __PROPERTY_LINE_EDIT_CONTROL_H__