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

#include "UI/UIHoleTransition.h"
#include "Render/RenderManager.h"
#include "Platform/SystemTimer.h"
#include "UI/UIControlSystem.h"

namespace DAVA 
{

	
UIHoleTransition::UIHoleTransition()
{
	duration = 0.8f;
}

UIHoleTransition::~UIHoleTransition()
{
}
//	
//void UIHoleTransition::SetType(eType _type)
//{
//	type = _type;
//}
	
void UIHoleTransition::SetPolygon(const Polygon2 & pts)
{	
	clipPoly = pts;
}

void UIHoleTransition::Update(float32 timeElapsed)
{
	UIScreenTransition::Update(timeElapsed);
	normalizedTime = currentTime / duration;
	
	float scaleCoef = 1.0f;
	if (normalizedTime <= 0.5f)scaleCoef = interpolationFunc(1.0f - normalizedTime * 2.0f);
	else scaleCoef = interpolationFunc((normalizedTime - 0.5f) * 2.0f);
		
	for (int k = 0; k < clipPoly.pointCount; ++k)
	{
		points[k] = clipPoly.points[k];
		points[k] -= Vector3(GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f, 0);
		points[k] *= scaleCoef;
		points[k] += Vector3(GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f, 0);
	}
}

void UIHoleTransition::Draw(const UIGeometricData &geometricData)
{
	/*
	 renderTargetPrevScreen->SetScale(0.5f, 1.0f);
	 renderTargetPrevScreen->SetPosition(0, 0);
	 renderTargetPrevScreen->Draw();

	 renderTargetNextScreen->SetScale(0.5f, 1.0f);
	 renderTargetNextScreen->SetPosition(240, 0);
	 renderTargetNextScreen->Draw(); 

	 FROM_LEFT = 0, 
	 FROM_RIGHT,
	 FROM_TOP,
	 FROM_BOTTOM,
	 */
	
	RenderManager::Instance()->SetColor(0.0f, 0.0f, 0.0f, 1.0f);
	RenderManager::Instance()->FillRect(Rect(0.0f, 0.0f, (float32)GetScreenWidth(), (float32)GetScreenHeight()));
	RenderManager::Instance()->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	
	Texture * tx = renderTargetPrevScreen->GetTexture();
	if (normalizedTime > 0.5f)
		tx = renderTargetNextScreen->GetTexture();
	
	for (int k = 0; k < clipPoly.pointCount; ++k)
	{
		texCoords[k] = Vector2(points[k].x / tx->width, points[k].y / tx->height);
	}
	
	RenderManager::Instance()->SetVertexPointer(3, TYPE_FLOAT, 0, points);
	RenderManager::Instance()->SetTexCoordPointer(2, TYPE_FLOAT, 0, texCoords);
	RenderManager::Instance()->SetTexture(tx);
	
	RenderManager::Instance()->FlushState();
	
	glDrawArrays(GL_TRIANGLE_FAN, 0, clipPoly.pointCount);
	
	/*
	float32 startXPos[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	float32 startYPos[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	float32 endXPos[4] = {RenderManager::Instance()->ScreenWidth(), -RenderManager::ScreenWidth(), 0.0f, 0.0f};
	float32 endYPos[4] = {0.0f, 0.0f, RenderManager::ScreenHeight(), -RenderManager::ScreenHeight()};
	
	float32 xPrevPosition = (endXPos[type] - startXPos[type]) * normalizedTime;
	float32 yPrevPosition = (endYPos[type] - startYPos[type]) * normalizedTime;
	float32 xNextPosition = xPrevPosition - endXPos[type];
	float32 yNextPosition = yPrevPosition - endYPos[type];
	
	renderTargetPrevScreen->SetPosition(xPrevPosition, yPrevPosition);
	renderTargetPrevScreen->Draw();
	
	renderTargetNextScreen->SetPosition(xNextPosition, yNextPosition);
	renderTargetNextScreen->Draw(); */
	RenderManager::Instance()->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
}
	
};

