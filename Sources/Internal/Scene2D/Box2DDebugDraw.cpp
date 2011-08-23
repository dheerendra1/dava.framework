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
#include "Scene2D/Box2DDebugDraw.h"
#include "Render/RenderManager.h"
#include "Render/RenderHelper.h"

namespace DAVA 
{	
void Box2DDebugDraw::SetPTDRatio(float32 _ptdRatio)
{
	ptdRatio = _ptdRatio;
}
	
void Box2DDebugDraw::SetCameraPos(const Vector2 &_cameraPos)
{
	cameraPos = _cameraPos;
}



void Box2DDebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	
	/*glColor3f(color.r, color.g, color.b);
	glBegin(GL_LINE_LOOP);
	for (int32 i = 0; i < vertexCount; ++i)
	{
		glVertex2f(vertices[i].x, vertices[i].y);
	}
	glEnd();*/
	
	RenderManager::Instance()->SetColor(color.r, color.g, color.b, 1.0f);
	Polygon3 polygon;
	for (int32 i = 0; i < vertexCount; ++i)
	{
		polygon.AddPoint(Vector3(vertices[i].x * ptdRatio + cameraPos.x, -vertices[i].y * ptdRatio + cameraPos.y, 0.0f));
	}
	RenderHelper::DrawPolygon(polygon, true);
	RenderManager::Instance()->ResetColor();
}

void Box2DDebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	RenderManager::Instance()->SetColor(color.r, color.g, color.b, 1.0f);
	Polygon3 polygon;
	for (int32 i = 0; i < vertexCount; ++i)
	{
		polygon.AddPoint(Vector3(vertices[i].x * ptdRatio + cameraPos.x, -vertices[i].y * ptdRatio + cameraPos.y, 0.0f));
	}
	RenderHelper::DrawPolygon(polygon, true);
	RenderManager::Instance()->ResetColor();
	
	/*glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);
	glBegin(GL_TRIANGLE_FAN);
	for (int32 i = 0; i < vertexCount; ++i)
	{
		glVertex2f(vertices[i].x, vertices[i].y);
	}
	glEnd();
	glDisable(GL_BLEND);

	glColor4f(color.r, color.g, color.b, 1.0f);
	glBegin(GL_LINE_LOOP);
	for (int32 i = 0; i < vertexCount; ++i)
	{
		glVertex2f(vertices[i].x, vertices[i].y);
	}
	glEnd();*/
}

void Box2DDebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
	RenderManager::Instance()->SetColor(color.r, color.g, color.b, 1.0f);
	Polygon3 polygon;

	const float32 k_segments = 16.0f;
	const float32 k_increment = 2.0f * b2_pi / k_segments;
	float32 theta = 0.0f;
	for (int32 i = 0; i < k_segments; ++i)
	{
		b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
		//glVertex2f(v.x, v.y);
		polygon.AddPoint(Vector3(v.x * ptdRatio + cameraPos.x, -v.y * ptdRatio + cameraPos.y, 0.0f));
		theta += k_increment;
	}
	RenderHelper::DrawPolygon(polygon, true);
	RenderManager::Instance()->ResetColor();
}

void Box2DDebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
	RenderManager::Instance()->SetColor(color.r, color.g, color.b, 1.0f);
	Polygon3 polygon;
	
	const float32 k_segments = 16.0f;
	const float32 k_increment = 2.0f * b2_pi / k_segments;
	float32 theta = 0.0f;
	for (int32 i = 0; i < k_segments; ++i)
	{
		b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
		//glVertex2f(v.x, v.y);
		polygon.AddPoint(Vector3(v.x * ptdRatio + cameraPos.x, -v.y * ptdRatio + cameraPos.y, 0.0f));
		theta += k_increment;
	}
	RenderHelper::DrawPolygon(polygon, true);
	RenderManager::Instance()->ResetColor();
	/*const float32 k_segments = 16.0f;
	const float32 k_increment = 2.0f * b2_pi / k_segments;
	float32 theta = 0.0f;
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);
	glBegin(GL_TRIANGLE_FAN);
	for (int32 i = 0; i < k_segments; ++i)
	{
		b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
		glVertex2f(v.x, v.y);
		theta += k_increment;
	}
	glEnd();
	glDisable(GL_BLEND);

	theta = 0.0f;
	glColor4f(color.r, color.g, color.b, 1.0f);
	glBegin(GL_LINE_LOOP);
	for (int32 i = 0; i < k_segments; ++i)
	{
		b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
		glVertex2f(v.x, v.y);
		theta += k_increment;
	}
	glEnd();

	b2Vec2 p = center + radius * axis;
	glBegin(GL_LINES);
	glVertex2f(center.x, center.y);
	glVertex2f(p.x, p.y);
	glEnd();*/
}

void Box2DDebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	RenderManager::Instance()->SetColor(color.r, color.g, color.b, 1.0f);
	RenderManager::Instance()->DrawLine(Vector2(p1.x * ptdRatio + cameraPos.x, -p1.y * ptdRatio + cameraPos.y), 
                                        Vector2(p2.x * ptdRatio + cameraPos.x, -p2.y * ptdRatio + cameraPos.y));
	RenderManager::Instance()->ResetColor();
	/*glColor3f(color.r, color.g, color.b);
	glBegin(GL_LINES);
	glVertex2f(p1.x, p1.y);
	glVertex2f(p2.x, p2.y);
	glEnd();*/
}

void Box2DDebugDraw::DrawTransform(const b2Transform& xf)
{
    b2_float32 tmpTest = xf.GetAngle();
    
    b2Vec2 p1 = xf.position, p2;
    const float32 k_axisScale = 0.4f;

    p2 = p1 + k_axisScale * xf.R.col1;
    DrawSegment(p1, p2, b2Color(1.0f, 0.0f, 0.0f));

    p2 = p1 + k_axisScale * xf.R.col2;
    DrawSegment(p1, p2, b2Color(0.0f, 1.0f, 0.0f));

    /*b2Vec2 p1 = xf.position, p2;
	const float32 k_axisScale = 0.4f;
	glBegin(GL_LINES);
	
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex2f(p1.x, p1.y);
	p2 = p1 + k_axisScale * xf.R.col1;
	glVertex2f(p2.x, p2.y);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(p1.x, p1.y);
	p2 = p1 + k_axisScale * xf.R.col2;
	glVertex2f(p2.x, p2.y);

	glEnd();*/
}

void Box2DDebugDraw::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color)
{
	RenderManager::Instance()->SetColor(color.r, color.g, color.b, 1.0f);
	RenderHelper::DrawPoint(Vector2(p.x * ptdRatio + cameraPos.x, -p.y * ptdRatio + cameraPos.y), size * ptdRatio);
	RenderManager::Instance()->ResetColor();
	/*glPointSize(size);
	glBegin(GL_POINTS);
	glColor3f(color.r, color.g, color.b);
	glVertex2f(p.x, p.y);
	glEnd();
	glPointSize(1.0f);*/
}

void Box2DDebugDraw::DrawString(int x, int y, const char *string, ...)
{
	/*char buffer[128];

	va_list arg;
	va_start(arg, string);
	vsprintf(buffer, string, arg);
	va_end(arg);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	gluOrtho2D(0, w, h, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor3f(0.9f, 0.6f, 0.6f);
	glRasterPos2i(x, y);
	int32 length = (int32)strlen(buffer);
	for (int32 i = 0; i < length; ++i)
	{
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, buffer[i]);
	}

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);*/
}

void Box2DDebugDraw::DrawAABB(b2AABB* aabb, const b2Color& c)
{
	/*glColor3f(c.r, c.g, c.b);
	glBegin(GL_LINE_LOOP);
	glVertex2f(aabb->lowerBound.x, aabb->lowerBound.y);
	glVertex2f(aabb->upperBound.x, aabb->lowerBound.y);
	glVertex2f(aabb->upperBound.x, aabb->upperBound.y);
	glVertex2f(aabb->lowerBound.x, aabb->upperBound.y);
	glEnd();*/
}
	
};
