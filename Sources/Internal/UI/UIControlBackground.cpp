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

#include "UI/UIControlBackground.h"
#include "Debug/DVAssert.h"
#include "UI/UIControl.h"
#include "Core/Core.h"
#include "Render/RenderManager.h"
#include "Render/RenderHelper.h"

namespace DAVA
{

UIControlBackground::UIControlBackground()
:	spr(NULL)
,	frame(0)
,	align(ALIGN_HCENTER|ALIGN_VCENTER)
,	type(DRAW_ALIGNED)
,	color(1.0f, 1.0f, 1.0f, 1.0f)
,	drawColor(1.0f, 1.0f, 1.0f, 1.0f)
,	leftStretchCap(0)
,	topStretchCap(0)
,	spriteModification(0)
,	colorInheritType(COLOR_IGNORE_PARENT)
,	usePerPixelAccuracy(false)
,	lastDrawPos(0, 0)
{
	rdoObject = new RenderDataObject();
    vertexStream = rdoObject->SetStream(EVF_VERTEX, TYPE_FLOAT, 2, 0, 0);
    texCoordStream = rdoObject->SetStream(EVF_TEXCOORD0, TYPE_FLOAT, 2, 0, 0);
	//rdoObject->SetStream()
}
	
UIControlBackground *UIControlBackground::Clone()
{
	UIControlBackground *cb = new UIControlBackground();
	cb->CopyDataFrom(this);
	return cb;
}
	
void UIControlBackground::CopyDataFrom(UIControlBackground *srcBackground)
{
	SafeRelease(spr);
	spr = SafeRetain(srcBackground->spr);
	frame = srcBackground->frame;
	align = srcBackground->align;
	type = srcBackground->type;
	color = srcBackground->color;
	spriteModification = srcBackground->spriteModification;
	colorInheritType = srcBackground->colorInheritType;
	usePerPixelAccuracy = srcBackground->usePerPixelAccuracy;
}


UIControlBackground::~UIControlBackground()
{
	SafeRelease(rdoObject);
	SafeRelease(spr);
}
	
Sprite*	UIControlBackground::GetSprite()
{
	return spr;	
}
int32	UIControlBackground::GetFrame()
{
	return frame;
}
int32	UIControlBackground::GetAlign()
{
	return align;
}

int32	UIControlBackground::GetModification()
{
	return spriteModification;
}

UIControlBackground::eColorInheritType UIControlBackground::GetColorInheritType()
{
	return (eColorInheritType)colorInheritType;
}


UIControlBackground::eDrawType	UIControlBackground::GetDrawType()
{
	return (UIControlBackground::eDrawType)type;
}
	
	
void UIControlBackground::SetSprite(const String &spriteName, int32 drawFrame)
{
	Sprite *tempSpr = Sprite::Create(spriteName);
	SetSprite(tempSpr, drawFrame);
	SafeRelease(tempSpr);
}

void UIControlBackground::SetSprite(Sprite* drawSprite, int32 drawFrame)
{
	SafeRelease(spr);
	spr = SafeRetain(drawSprite);
	frame =  drawFrame;
}
void UIControlBackground::SetFrame(int32 drawFrame)
{
	DVASSERT(spr);
	frame = drawFrame;
}

void UIControlBackground::SetAlign(int32 drawAlign)
{
	align = drawAlign;
}
void UIControlBackground::SetDrawType(UIControlBackground::eDrawType drawType)
{
	type = drawType;
}

void UIControlBackground::SetModification(int32 modification)
{
	spriteModification = modification;	
}
	
void	UIControlBackground::SetColorInheritType(UIControlBackground::eColorInheritType inheritType)
{
	DVASSERT(inheritType >= 0 && inheritType < COLOR_INHERIT_TYPES_COUNT);
	colorInheritType = inheritType;
}
	

	
	
const Color &UIControlBackground::GetDrawColor()
{
	return drawColor;
}
	
void UIControlBackground::SetParentColor(const Color &parentColor)
{
	switch (colorInheritType) 
	{
		case COLOR_MULTIPLY_ON_PARENT:
		{
			drawColor.r = color.r * parentColor.r;
			drawColor.g = color.g * parentColor.g;
			drawColor.b = color.b * parentColor.b;
			drawColor.a = color.a * parentColor.a;
		}
			break;
		case COLOR_ADD_TO_PARENT:
		{
			drawColor.r = Min(color.r + parentColor.r, 1.0f);
			drawColor.g = Min(color.g + parentColor.g, 1.0f);
			drawColor.b = Min(color.b + parentColor.b, 1.0f);
			drawColor.a = Min(color.a + parentColor.a, 1.0f);
		}
			break;
		case COLOR_REPLACE_TO_PARENT:
		{
			drawColor = parentColor;
		}
			break;
		case COLOR_IGNORE_PARENT:
		{
			drawColor = color;
		}
			break;
		case COLOR_MULTIPLY_ALPHA_ONLY:
		{
			drawColor = color;
			drawColor.a = color.a * parentColor.a;
		}
			break;
		case COLOR_REPLACE_ALPHA_ONLY:
		{
			drawColor = color;
			drawColor.a = parentColor.a;
		}
			break;
	}	
}
	



void UIControlBackground::Draw(const UIGeometricData &geometricData)
{
	

	Rect drawRect = geometricData.GetUnrotatedRect();
//	if(drawRect.x > RenderManager::Instance()->GetScreenWidth() || drawRect.y > RenderManager::Instance()->GetScreenHeight() || drawRect.x + drawRect.dx < 0 || drawRect.y + drawRect.dy < 0)
//	{//TODO: change to screen size from control system and sizes from sprite
//		return;
//	}
	
	RenderManager::Instance()->SetColor(drawColor.r, drawColor.g, drawColor.b, drawColor.a);
	
	Sprite::DrawState drawState;
	if (spr)
	{
		drawState.frame = frame;
		if (spriteModification) 
		{
			drawState.flags = spriteModification;
		}
//		spr->Reset();
//		spr->SetFrame(frame);
//		spr->SetModification(spriteModification);
	}
	switch (type) 
	{
		case DRAW_ALIGNED:
		{
			if (!spr)break;
			if(align & ALIGN_LEFT)
			{
				drawState.position.x = drawRect.x;
			}
			else if(align & ALIGN_RIGHT)
			{
				drawState.position.x = drawRect.x + drawRect.dx - spr->GetWidth() * geometricData.scale.x;
			}
			else
			{
				drawState.position.x = drawRect.x + ((drawRect.dx - spr->GetWidth() * geometricData.scale.x) * 0.5f) ;
			}
			if(align & ALIGN_TOP)
			{
				drawState.position.y = drawRect.y;
			}
			else if(align & ALIGN_BOTTOM)
			{
				drawState.position.y = drawRect.y + drawRect.dy - spr->GetHeight() * geometricData.scale.y;
			}
			else
			{
				drawState.position.y = drawRect.y + ((drawRect.dy - spr->GetHeight() * geometricData.scale.y + spr->GetDefaultPivotPoint().y * geometricData.scale.y) * 0.5f) ;
			}
			if(geometricData.angle != 0)
			{
				float tmpX = drawState.position.x;
				drawState.position.x = (tmpX - geometricData.position.x) * geometricData.cosA  + (geometricData.position.y - drawState.position.y) * geometricData.sinA + geometricData.position.x;
				drawState.position.y = (tmpX - geometricData.position.x) * geometricData.sinA  + (drawState.position.y - geometricData.position.y) * geometricData.cosA + geometricData.position.y;
//				spr->SetAngle(geometricData.angle);
				drawState.angle = geometricData.angle;
			}
//			spr->SetPosition(x, y);
			drawState.scale = geometricData.scale;
			drawState.pivotPoint = spr->GetDefaultPivotPoint();
//			spr->SetScale(geometricData.scale);
			if (usePerPixelAccuracy && lastDrawPos == drawState.position && drawState.scale.x == 1.0 && drawState.scale.y == 1.0)
			{
				drawState.usePerPixelAccuracy = usePerPixelAccuracy;
//				Logger::Info("UIControlBackground: Trying to draw with per pixel accuracy");

			}
			lastDrawPos = drawState.position;


			spr->Draw(&drawState);
		}
		break;

		case DRAW_SCALE_TO_RECT:
		{
			if (!spr)break;

			drawState.position = geometricData.position;
			drawState.flags = spriteModification;
			drawState.scale.x = drawRect.dx / spr->GetSize().dx;
			drawState.scale.y = drawRect.dy / spr->GetSize().dy;
			drawState.pivotPoint.x = geometricData.pivotPoint.x / (geometricData.size.x / spr->GetSize().dx);
			drawState.pivotPoint.y = geometricData.pivotPoint.y / (geometricData.size.y / spr->GetSize().dy);
			drawState.angle = geometricData.angle;

//			spr->SetPosition(geometricData.position);
//			spr->SetScale(drawRect.dx / spr->GetSize().dx, drawRect.dy / spr->GetSize().dy);
//			spr->SetPivotPoint(geometricData.pivotPoint.x / (geometricData.size.x / spr->GetSize().dx), geometricData.pivotPoint.y / (geometricData.size.y / spr->GetSize().dy));
//			spr->SetAngle(geometricData.angle);
			
			spr->Draw(&drawState);
		}
		break;
		
		case DRAW_SCALE_PROPORTIONAL:
		{
			if (!spr)break;
			float32 w, h;
			w = drawRect.dx / (spr->GetWidth() * geometricData.scale.x);
			h = drawRect.dy / (spr->GetHeight() * geometricData.scale.y);
			float ph = spr->GetDefaultPivotPoint().y;
			if(w < h)
			{
				h = spr->GetHeight() * w * geometricData.scale.x;
				ph *= w;
				w = drawRect.dx;
			}
			else
			{
				w = spr->GetWidth() * h * geometricData.scale.y;
				ph *= h;
				h = drawRect.dy;
			}
			if(align & ALIGN_LEFT)
			{
				drawState.position.x = drawRect.x;
			}
			else if(align & ALIGN_RIGHT)
			{
				drawState.position.x = (drawRect.x + drawRect.dx - w);
			}
			else
			{
				drawState.position.x = drawRect.x + (int32)((drawRect.dx - w) * 0.5) ;
			}
			if(align & ALIGN_TOP)
			{
				drawState.position.y = drawRect.y;
			}
			else if(align & ALIGN_BOTTOM)
			{
				drawState.position.y = (drawRect.y + drawRect.dy - h);
			}
			else
			{
				drawState.position.y = (drawRect.y) + (int32)((drawRect.dy - h + ph) * 0.5) ;
			}
			drawState.scale.x = w / spr->GetWidth();
			drawState.scale.y = h / spr->GetHeight();
//			spr->SetScaleSize(w, h);
			if(geometricData.angle != 0)
			{
				float32 tmpX = drawState.position.x;
				drawState.position.x = ((tmpX - geometricData.position.x) * geometricData.cosA  + (geometricData.position.y - drawState.position.y) * geometricData.sinA + geometricData.position.x);
				drawState.position.y = ((tmpX - geometricData.position.x) * geometricData.sinA  + (drawState.position.y - geometricData.position.y) * geometricData.cosA + geometricData.position.y);
				drawState.angle = geometricData.angle;
//				spr->SetAngle(geometricData.angle);
			}
//			spr->SetPosition((float32)x, (float32)y);
			
			spr->Draw(&drawState);
		}
		break;
		
		case DRAW_FILL:
		{//TODO: add rotation
			RenderHelper::Instance()->FillRect(drawRect);
		}	
		break;
			
		case DRAW_STRETCH_BOTH:
		case DRAW_STRETCH_HORIZONTAL:
		case DRAW_STRETCH_VERTICAL:
			DrawStretched(drawRect);	
		break;
	}
	
	RenderManager::Instance()->ResetColor();
	
}

	
void UIControlBackground::DrawStretched(const Rect &drawRect)
{
	if (!spr)return;
	Texture *texture = spr->GetTexture(frame);
	
	
	float32 x = spr->GetRectOffsetValueForFrame(frame, Sprite::X_POSITION_IN_TEXTURE);
	float32 y = spr->GetRectOffsetValueForFrame(frame, Sprite::Y_POSITION_IN_TEXTURE);
	float32 dx = spr->GetRectOffsetValueForFrame(frame, Sprite::ACTIVE_WIDTH);
	float32 dy = spr->GetRectOffsetValueForFrame(frame, Sprite::ACTIVE_HEIGHT);
//	if (spr->IsUseContentScale()) 
//	{
		dx /= Core::Instance()->GetResourceToVirtualFactor(spr->GetResourceSizeIndex());
		dy /= Core::Instance()->GetResourceToVirtualFactor(spr->GetResourceSizeIndex());
//	}
	
	float32 leftCap = leftStretchCap / Core::Instance()->GetResourceToVirtualFactor(spr->GetResourceSizeIndex());
	float32 topCap = topStretchCap / Core::Instance()->GetResourceToVirtualFactor(spr->GetResourceSizeIndex());

	float32 vertices[16 * 2];
	float32 texCoords[16 * 2];
	
	float32 textureWidth = (float32)texture->GetWidth();
	float32 textureHeight = (float32)texture->GetHeight();
	
	int32 vertInTriCount = 18;
	int32 vertCount = 16;
	switch (type) 
	{
		case DRAW_STRETCH_HORIZONTAL:
		{
			
			vertices[0] = drawRect.x;
			vertices[1] = drawRect.y;
			
			vertices[2] = drawRect.x + leftStretchCap;
			vertices[3] = drawRect.y;
			
			vertices[4] = drawRect.x + drawRect.dx - leftStretchCap;
			vertices[5] = drawRect.y;
			
			vertices[6] = drawRect.x + drawRect.dx;
			vertices[7] = drawRect.y;
			
			vertices[8] = drawRect.x;
			vertices[9] = drawRect.y + drawRect.dy;
			
			vertices[10] = drawRect.x + leftStretchCap;
			vertices[11] = drawRect.y + drawRect.dy;
			
			vertices[12] = drawRect.x + drawRect.dx - leftStretchCap;
			vertices[13] = drawRect.y + drawRect.dy;
			
			vertices[14] = drawRect.x + drawRect.dx;
			vertices[15] = drawRect.y + drawRect.dy;
			
			
			texCoords[0] = x / textureWidth;
			texCoords[1] = y / textureHeight;
			
			texCoords[2] = (x + leftCap) / textureWidth;
			texCoords[3] = y / textureHeight;
			
			texCoords[4] = (x + dx - leftCap) / textureWidth;
			texCoords[5] = y / textureHeight;
			
			texCoords[6] = (x + dx) / textureWidth;
			texCoords[7] = y / textureHeight;
			
			texCoords[8] = x / textureWidth;
			texCoords[9] = (y + dy) / textureHeight;
			
			texCoords[10] = (x + leftCap) / textureWidth;
			texCoords[11] = (y + dy) / textureHeight;
			
			texCoords[12] = (x + dx - leftCap) / textureWidth;
			texCoords[13] = (y + dy) / textureHeight;
			
			texCoords[14] = (x + dx) / textureWidth;
			texCoords[15] = (y + dy) / textureHeight;
		}
		break;
		case DRAW_STRETCH_VERTICAL:
		{
			vertices[0] = drawRect.x;
			vertices[1] = drawRect.y;
			
			vertices[2] = drawRect.x;
			vertices[3] = drawRect.y + topStretchCap;
			
			vertices[4] = drawRect.x;
			vertices[5] = drawRect.y + drawRect.dy - topStretchCap;
			
			vertices[6] = drawRect.x;
			vertices[7] = drawRect.y + drawRect.dy;
			
			vertices[8] = drawRect.x + drawRect.dx;
			vertices[9] = drawRect.y;
			
			vertices[10] = drawRect.x + drawRect.dx;
			vertices[11] = drawRect.y + topStretchCap;
			
			vertices[12] = drawRect.x + drawRect.dx;
			vertices[13] = drawRect.y + drawRect.dy - topStretchCap;
			
			vertices[14] = drawRect.x + drawRect.dx;
			vertices[15] = drawRect.y + drawRect.dy;
			
			texCoords[0] = x / textureWidth;
			texCoords[1] = y / textureHeight;
			
			texCoords[2] = x / textureWidth;
			texCoords[3] = (y + topCap) / textureHeight;
			
			texCoords[4] = x / textureWidth;
			texCoords[5] = (y + dy - topCap) / textureHeight;
			
			texCoords[6] = x / textureWidth;
			texCoords[7] = (y + dy) / textureHeight;
			
			texCoords[8] = (x + dx) / textureWidth;
			texCoords[9] = y / textureHeight;
			
			texCoords[10] = (x + dx) / textureWidth;
			texCoords[11] = (y + topCap) / textureHeight;
			
			texCoords[12] = (x + dx) / textureWidth;
			texCoords[13] = (y + dy - topCap) / textureHeight;
			
			texCoords[14] = (x + dx) / textureWidth;
			texCoords[15] = (y + dy) / textureHeight;
		}
		break;
		case DRAW_STRETCH_BOTH:
		{
			vertInTriCount = 18 * 3;
			vertCount = 32;
			vertices[0] = drawRect.x;
			vertices[1] = drawRect.y;
			
			vertices[2] = drawRect.x + leftStretchCap;
			vertices[3] = drawRect.y;
			
			vertices[4] = drawRect.x + drawRect.dx - leftStretchCap;
			vertices[5] = drawRect.y;
			
			vertices[6] = drawRect.x + drawRect.dx;
			vertices[7] = drawRect.y;
			
			vertices[8] = drawRect.x;
			vertices[9] = drawRect.y + topStretchCap;
			
			vertices[10] = drawRect.x + leftStretchCap;
			vertices[11] = drawRect.y + topStretchCap;
			
			vertices[12] = drawRect.x + drawRect.dx - leftStretchCap;
			vertices[13] = drawRect.y + topStretchCap;
			
			vertices[14] = drawRect.x + drawRect.dx;
			vertices[15] = drawRect.y + topStretchCap;
			
			vertices[16] = drawRect.x;
			vertices[17] = drawRect.y + drawRect.dy - topStretchCap;
			
			vertices[18] = drawRect.x + leftStretchCap;
			vertices[19] = drawRect.y + drawRect.dy - topStretchCap;
			
			vertices[20] = drawRect.x + drawRect.dx - leftStretchCap;
			vertices[21] = drawRect.y + drawRect.dy - topStretchCap;
			
			vertices[22] = drawRect.x + drawRect.dx;
			vertices[23] = drawRect.y + drawRect.dy - topStretchCap;
			
			vertices[24] = drawRect.x;
			vertices[25] = drawRect.y + drawRect.dy;
			
			vertices[26] = drawRect.x + leftStretchCap;
			vertices[27] = drawRect.y + drawRect.dy;
			
			vertices[28] = drawRect.x + drawRect.dx - leftStretchCap;
			vertices[29] = drawRect.y + drawRect.dy;
			
			vertices[30] = drawRect.x + drawRect.dx;
			vertices[31] = drawRect.y + drawRect.dy;			
			
			// 
			
			texCoords[0] = x / textureWidth;
			texCoords[1] = y / textureHeight;
			
			texCoords[2] = (x + leftCap) / textureWidth;
			texCoords[3] = y / textureHeight;
			
			texCoords[4] = (x + dx - leftCap) / textureWidth;
			texCoords[5] = y / textureHeight;
			
			texCoords[6] = (x + dx) / textureWidth;
			texCoords[7] = y / textureHeight;
			
			texCoords[8] = x / textureWidth;
			texCoords[9] = (y + topCap) / textureHeight;
			
			texCoords[10] = (x + leftCap) / textureWidth;
			texCoords[11] = (y + topCap) / textureHeight;
			
			texCoords[12] = (x + dx - leftCap) / textureWidth;
			texCoords[13] = (y + topCap) / textureHeight;
			
			texCoords[14] = (x + dx) / textureWidth;
			texCoords[15] = (y + topCap) / textureHeight;
			
			texCoords[16] = x / textureWidth;
			texCoords[17] = (y + dy - topCap) / textureHeight;
			
			texCoords[18] = (x + leftCap) / textureWidth;
			texCoords[19] = (y + dy - topCap)  / textureHeight;
			
			texCoords[20] = (x + dx - leftCap) / textureWidth;
			texCoords[21] = (y + dy - topCap)  / textureHeight;
			
			texCoords[22] = (x + dx) / textureWidth;
			texCoords[23] = (y + dy - topCap)  / textureHeight;
			
			texCoords[24] = x / textureWidth;
			texCoords[25] = (y + dy) / textureHeight;
			
			texCoords[26] = (x + leftCap) / textureWidth;
			texCoords[27] = (y + dy) / textureHeight;
			
			texCoords[28] = (x + dx - leftCap) / textureWidth;
			texCoords[29] = (y + dy) / textureHeight;
			
			texCoords[30] = (x + dx) / textureWidth;
			texCoords[31] = (y + dy) / textureHeight;
		}
		break;
	}
	
//	if (Core::GetContentScaleFactor() != 1.0 && RenderManager::IsRenderTarget()) 
//	{
//		for (int i = 0; i < vertCount; i++) 
//		{
//			vertices[i] *= Core::GetVirtualToPhysicalFactor();
//	}
//	}


	uint16 indeces[18 * 3] = 
	{
		0, 1, 4, 
		1, 5, 4,
		1, 2, 5,
		2, 6, 5, 
		2, 3, 6,
		3, 7, 6,

		4, 5, 8,
		5, 9, 8,
		5, 6, 9,
		6, 10, 9,
		6, 7, 10,
		7, 11, 10,

		8, 9, 12,
		9, 12, 13,
		9, 10, 13,
		10, 14, 13,
		10, 11, 14,
		11, 15, 14
	};

	vertexStream->Set(TYPE_FLOAT, 2, 0, vertices);
	texCoordStream->Set(TYPE_FLOAT, 2, 0, texCoords);

	RenderManager::Instance()->SetTexture(texture);
	RenderManager::Instance()->SetRenderEffect(RenderManager::TEXTURE_MUL_FLAT_COLOR);
    RenderManager::Instance()->SetRenderData(rdoObject);
	RenderManager::Instance()->DrawElements(PRIMITIVETYPE_TRIANGLELIST, vertInTriCount, EIF_16, indeces);

	/*GLenum glErr = glGetError();
	if (glErr != GL_NO_ERROR)
	{
		Logger::Debug("GLError: 0x%x", glErr);
	}*/
}
	

void UIControlBackground::SetLeftRightStretchCap(float32 _leftStretchCap)
{
	leftStretchCap = _leftStretchCap;
}

void UIControlBackground::SetTopBottomStretchCap(float32 _topStretchCap)
{
	topStretchCap = _topStretchCap;
}
	
	

};