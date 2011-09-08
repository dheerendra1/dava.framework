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
#ifndef __DAVAENGINE_RENDER_HELPER_H__
#define __DAVAENGINE_RENDER_HELPER_H__

#include "Base/BaseTypes.h"
#include "Base/BaseMath.h"
#include "Base/StaticSingleton.h"
#include "Render/RenderBase.h"

#include "Render/Texture.h"
#include "Animation/Interpolation.h"

namespace DAVA
{
	
/**
    \brief 
 
 */
class RenderDataObject;
class RenderDataStream;
    
class RenderHelper : public StaticSingleton<RenderHelper>
{
public:
	RenderHelper();
    ~RenderHelper();
    
    void DrawLine(const Vector2 & pt1, const Vector2 & pt2); 
	void DrawLine(const Vector3 & pt1, const Vector3 & pt2); 
    void DrawRect(const Rect & rect);
    void FillRect(const Rect & rect);
    

    RenderDataObject * renderDataObject;
    RenderDataStream * vertexStream; 
    float32 vertices[32];
    
	// point helpers
	static void DrawPoint(const Vector2 & pt, float32 ptSize = 1.0f);
	static void DrawPoint(const Vector3 & pt, float32 ptSize = 1.0f);
	static void DrawCircle(const Vector2 & center, float32 radius);
	static void DrawCircle(const Vector3 & center, float32 radius);
	
	// polygon & line helper functions
	//static 
	void DrawPolygon(Polygon2 & polygon, bool closed);
	static void DrawPolygon(Polygon3 & polygon, bool closed);
	static void DrawPolygonTransformed(Polygon2 & polygon, bool closed, const Matrix3 & transform);
	static void DrawPolygonPoints(Polygon2 & polygon, bool closed);
	static void DrawPolygonPoints(Polygon3 & polygon, bool closed);
    
	
    static void DrawLineWithEndPoints(const Vector3 & pt1, const Vector3 & pt2); 
	static void DrawStrippedLine(Polygon2 & polygon, float lineLen, float spaceLen, float halfWidth, Texture * texture, float initialPos);
	static void DrawBox(const AABBox2 & box);
	static void DrawBox(const AABBox3 & box);
	// 3d helper functions (bboxes and so on)
	
	static void DrawBSpline(BezierSpline3 * bSpline, int segments = 20, float ts = 0.0f, float te = 1.0f);
	static void DrawInterpolationFunc(Interpolation::Func func, const Rect & destRect);
	
};
	
}

#endif // __DAVAENGINE_OBJC_FRAMEWORK_RENDER_HELPER_H__

