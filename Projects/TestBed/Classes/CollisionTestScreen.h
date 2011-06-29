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
        * Created by Vitaliy Borodovsky 
=====================================================================================*/
#ifndef __COLLISION_TEST_SCREEN_H__
#define __COLLISION_TEST_SCREEN_H__

#include "DAVAEngine.h"

using namespace DAVA;

class CollisionTestScreen : public UIScreen
{
public:
	virtual void LoadResources();
	virtual void UnloadResources();
	virtual void WillAppear();
	virtual void WillDisappear();
	
	virtual void Update(float32 timeElapsed);
	virtual void Draw(const UIGeometricData &geometricData);
	virtual void Input(UIEvent * touch);
	
private:
	typedef void (CollisionTestScreen::*InputFunc)(UIEvent * touch);
	typedef void (CollisionTestScreen::*DrawFunc)(const UIGeometricData &geometricData);
	
	InputFunc inputFunc;
	DrawFunc drawFunc;
	
	void InputAABBoxCollisions(UIEvent * touch);
	void DrawAABBoxCollisions(const UIGeometricData &geometricData);
	AABBox2 box[2];
	Vector2 touchStart;
	AABBox2 * activeBox;

	static const int32 POLYGON_COUNT = 3;
	void InputPolygonCollisions(UIEvent * touch);
	void DrawPolygonCollisions(const UIGeometricData &geometricData);
	Polygon2 poly[POLYGON_COUNT + 6];
	Color pColor[POLYGON_COUNT + 6];
	Matrix3 pMatrix[POLYGON_COUNT + 6];
	Polygon2 * activePoly;
	
	void InputPolyCollisions(UIEvent * touch);
	void DrawPolyCollisions(const UIGeometricData &geometricData);
};

#endif // __COLLISION_TEST_SCREEN_H__