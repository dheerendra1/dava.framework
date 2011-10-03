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
	
class RenderDataObject;
class RenderDataStream;
    
    
/**
    \brief You can use this class to perform various important drawing operations in 2D & 3D.
    In most cases we use these functions for debug rendering, but in some cases it can be usefull in production. 
    Keep in mind that output of all line-drawing functions can depend on hardware and look differently on different systems
 */

class RenderHelper : public StaticSingleton<RenderHelper>
{
public:
	RenderHelper();
    ~RenderHelper();
    
    /**
        \brief Draws line from pt1 to pt2
        \param pt1 starting point 
        \param pt2 ending point
	 */
    void DrawLine(const Vector2 & pt1, const Vector2 & pt2); 
    /**
        \brief Draws line in 3D from pt1 to pt2
        \param pt1 starting point 
        \param pt2 ending point
	 */
	void DrawLine(const Vector3 & pt1, const Vector3 & pt2); 
    
    
    
    /**
        \brief Draws given rect in 2D space
        \param pt1 starting point 
        \param pt2 ending point
	 */    
    void DrawRect(const Rect & rect);
    
    /**
        \brief Fills given rect in 2D space
        \param pt1 starting point 
        \param pt2 ending point
	 */ 
    void FillRect(const Rect & rect);
    
	// point helpers
    
    /**
        \brief Draws given point in 2D space
        \param pt given point 
	 */
	void DrawPoint(const Vector2 & pt, float32 ptSize = 1.0f);
    
    /**
        \brief Draws given point in 3D space
        \param pt given point 
	 */
	void DrawPoint(const Vector3 & pt, float32 ptSize = 1.0f);
    
    /**
        \brief Draws circle in 2D space
        \param center center of the circle
        \param radius radius of the circle
     */
	void DrawCircle(const Vector2 & center, float32 radius);
    
    
    /**
        \brief Draws circle in 3D space on XY plane
        \param center center of the circle
        \param radius radius of the circle
     */
	void DrawCircle(const Vector3 & center, float32 radius);
	
	// polygon & line helper functions

	
    /**
        \brief Draws all concecutive lines from given polygon
        \param polygon the polygon we want to draw
        \param closed you should set this flag to true if you want to connect last point of polygon with first point
     */
    void DrawPolygon(Polygon2 & polygon, bool closed);
    /**
        \brief Draws all concecutive lines from given polygon
        \param polygon the polygon we want to draw
        \param closed you should set this flag to true if you want to connect last point of polygon with first point
     */
    void DrawPolygon(Polygon3 & polygon, bool closed);

    /**
        \brief Fill convex polygon with color. As all other draw functions this function use global color that can be set with RenderManager::Instance()->SetColor function. 
        \param polygon the polygon we want to draw
     */
    void FillPolygon(Polygon2 & polygon);
    
    /**
        \brief Fill convex polygon with color. As all other draw functions this function use global color that can be set with RenderManager::Instance()->SetColor function. 
        \param polygon the polygon we want to draw
     */
    void FillPolygon(Polygon3 & polygon);
    
    /**
        \brief Draws all concecutive lines from given polygon after transformation
        \param polygon the polygon we want to draw
        \param closed you should set this flag to true if you want to connect last point of polygon with first point
        \param transform transform that will be applied to polygon before it will be drawn
     */
	void DrawPolygonTransformed(Polygon2 & polygon, bool closed, const Matrix3 & transform);
    
    /**
        \brief Draws all points from given polygon
        \param polygon the polygon we want to draw
     */
    void DrawPolygonPoints(Polygon2 & polygon);

    /**
        \brief Draws all points from given polygon
        \param polygon the polygon we want to draw
     */
    void DrawPolygonPoints(Polygon3 & polygon);
    
    /**
        \brief Draws 2D bounding box
        \param box given bounding box
     */
	void DrawBox(const AABBox2 & box);

    /**
        \brief Draws 3D bounding box
        \param box given bounding box
     */
	void DrawBox(const AABBox3 & box);
	
    // Other debug functions  
	void DrawBSpline(BezierSpline3 * bSpline, int segments = 20, float ts = 0.0f, float te = 1.0f);
	void DrawInterpolationFunc(Interpolation::Func func, const Rect & destRect);
    //static void DrawLineWithEndPoints(const Vector3 & pt1, const Vector3 & pt2); 
	//static void DrawStrippedLine(Polygon2 & polygon, float lineLen, float spaceLen, float halfWidth, Texture * texture, float initialPos);

private:
    RenderDataObject * renderDataObject;
    RenderDataStream * vertexStream; 
    float32 vertices[32];
};
	
}

#endif // __DAVAENGINE_OBJC_FRAMEWORK_RENDER_HELPER_H__

