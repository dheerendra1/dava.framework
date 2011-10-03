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
#include "CollisionTestScreen.h"
#include "TestScreen.h"


void CollisionTestScreen::LoadResources()
{

	
//	inputFunc = &CollisionTestScreen::InputAABBoxCollisions;
//	drawFunc = &CollisionTestScreen::DrawAABBoxCollisions;
	inputFunc = &CollisionTestScreen::InputPolygonCollisions;
	drawFunc = &CollisionTestScreen::DrawPolygonCollisions;

	box[0].AddPoint(Vector2(10, 10));
	box[0].AddPoint(Vector2(30, 30));
	box[1].AddPoint(Vector2(25, 25));
	box[1].AddPoint(Vector2(40, 40));
	activeBox = 0;
	
	poly[0].AddPoint(Vector2(0, 0));
	poly[0].AddPoint(Vector2(20, 20));
	poly[0].AddPoint(Vector2(0, 20));

	poly[1].AddPoint(Vector2(50, 50));
	poly[1].AddPoint(Vector2(100, 50));
	poly[1].AddPoint(Vector2(100, 100));
	poly[1].AddPoint(Vector2(50, 100));

	poly[2].AddPoint(Vector2(150, 150));
	poly[2].AddPoint(Vector2(200, 150));
	poly[2].AddPoint(Vector2(200, 200));
	poly[2].AddPoint(Vector2(125, 225));
	poly[2].AddPoint(Vector2(100, 200));
	activePoly = 0;	
	
	for (int32 k = 0; k < POLYGON_COUNT; ++k)
		pMatrix[k].Identity();
	
	//pMatrix[0].BuildRotation(DegToRad(10.0f));

	//float32 r1 = CrossProduct(Vector2(1,1), Vector2(-1, -1));
	//float32 r2 = CrossProduct(Vector2(0,1), Vector2(0, -1));
	
	Matrix3 m1, m2, m3;
	m1.BuildRotation(15.0f);
	m2.BuildTranslation(Vector2(10, 10));
	m3.BuildScale(Vector2(10, 10));

	Matrix3 res = m1 * m2 * m3 * m3 * m2 * m1;
	Matrix3 resInverse;
	//bool boolRes = res.GetInverse(resInverse);
	Matrix3 idenTest = res * resInverse;
	//int32 check = 0;
}

void CollisionTestScreen::UnloadResources()
{
}

void CollisionTestScreen::WillAppear()
{
}

void CollisionTestScreen::WillDisappear()
{
	
}

void CollisionTestScreen::Input(UIEvent * touch)
{
	(this->*inputFunc)(touch);
}

void CollisionTestScreen::Update(float32 timeElapsed)
{
}

void CollisionTestScreen::Draw(const UIGeometricData &geometricData)
{
	(this->*drawFunc)(geometricData);
}

void CollisionTestScreen::InputAABBoxCollisions(UIEvent * touch)
{
	if (touch->phase == UIEvent::PHASE_BEGAN)
	{
		touchStart = touch->point;
		for (int i = 0; i < 2; ++i)
			if (box[i].IsInside(touch->point))
				activeBox = &box[i];
	}else if ((touch->phase == UIEvent::PHASE_DRAG) || 	(touch->phase == UIEvent::PHASE_ENDED))
	{
		activeBox->min += touch->point - touchStart;
		activeBox->max += touch->point - touchStart;
		touchStart = touch->point;
		
		if (touch->phase == UIEvent::PHASE_ENDED)
			activeBox = 0;
	}
}

void CollisionTestScreen::DrawAABBoxCollisions(const UIGeometricData &geometricData)
{
	Color c(1.0f, 1.0f, 1.0f, 1.0f);
	if (box[0].IsIntersectsWithBox(box[1]))
	{
		c = Color(1.0f, 0.0f, 0.0f, 1.0f);
	}
	RenderManager::Instance()->SetColor(c);
	RenderHelper::Instance()->DrawBox(box[0]);
	RenderHelper::Instance()->DrawBox(box[1]);
}

void CollisionTestScreen::InputPolygonCollisions(UIEvent * touch)
{
	if (touch->phase == UIEvent::PHASE_BEGAN)
	{
		touchStart = touch->point;
		for (int i = 0; i < POLYGON_COUNT; ++i)
			if (poly[i].IsPointInside(touch->point))
				activePoly = &poly[i];
	}else if ((touch->phase == UIEvent::PHASE_DRAG) || 	(touch->phase == UIEvent::PHASE_ENDED))
	{

		if (!activePoly)return;
		activePoly->Translate(touch->point - touchStart);
		touchStart = touch->point;
		
		if (touch->phase == UIEvent::PHASE_ENDED)
			activePoly = 0;
	}
}

void CollisionTestScreen::DrawPolygonCollisions(const UIGeometricData &geometricData)
{
	//for (int i = 0; i < POLYGON_COUNT; ++i)
	//	if (poly[i].IsPointInside(touchStart));

	for (int k = 0; k < POLYGON_COUNT; ++k)
	{
		pColor[k] = Color(1.0f, 1.0f, 1.0f, 1.0f);
	}

	for (int k1 = 0; k1 < POLYGON_COUNT - 1; ++k1)
		for (int k2 = k1 + 1; k2 < POLYGON_COUNT; ++k2)
		if (Collisions::Instance()->IsPolygonIntersectsPolygon(poly[k1], poly[k2]))//poly[0].IsIntersectsWithBox(box[1]))
		{
			pColor[k1] = Color(1.0f, 0.0f, 0.0f, 1.0f);
			pColor[k2] = Color(1.0f, 0.0f, 0.0f, 1.0f);
			
			ContactManifold2 manifold;
			Collisions::Instance()->FindIntersectionPolygonToPolygon(poly[k1], poly[k2], manifold);
			for (int m = 0; m < manifold.count; ++m)
			{
				RenderHelper::Instance()->DrawPoint(manifold.contactPoints[m], 3.0f);
			}
		}
	
	for (int k = 0; k < POLYGON_COUNT; ++k)
	{
		RenderManager::Instance()->SetColor(pColor[k]);
		RenderHelper::Instance()->DrawPolygonTransformed(poly[k], true, pMatrix[k]);
	}
	//RenderHelper::DrawPolygon(poly[1], true);
}