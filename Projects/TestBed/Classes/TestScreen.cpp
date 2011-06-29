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
#include "TestScreen.h"
#include "TouchEffectsControl.h"
//#import <OpenGL/glu.h>

void TestScreen::LoadResources()
{
	UIYamlLoader::Load(this, "~res:/Screens/test_screen.yaml");

	Logger::Debug("TestScreen = (%f, %f, %f, %f)", GetRect().x, GetRect().y, GetRect().dx, GetRect().dy);
	
	button = new UIButton(Rect(0, 0, 100, 50));
	button->SetDebugDraw(true);
	button->SetStateDrawType(UIControl::STATE_NORMAL|UIControl::STATE_HOVER|UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
	button->GetStateBackground(UIControl::STATE_NORMAL)->color = Color(0.0f, 0.4f, 0.0f, 1.0f);
	button->GetStateBackground(UIControl::STATE_HOVER)->color = Color(0.0f, 0.8f, 0.0f, 1.0f);
	button->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->color = Color(0.6f, 1.0f, 0.6f, 1.0f);
	AddControl(button);
	
	button->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::OnSwitchScreenMode));
	
	SetMultiInput(true, true);
	
	x = 0;
	dx = 0.5f;
	
	testSprite = Sprite::Create("~res:/Gfx/Effects/flower");
}

void TestScreen::UnloadResources()
{
}

void TestScreen::WillAppear()
{
	AddControl(new TouchEffectsControl());
}

void TestScreen::WillDisappear()
{
	
}

void TestScreen::Input(UIEvent * touch)
{
//	if(touch->phase == UIEvent::PHASE_ENDED)
//	{
//		Rect dest(touch->point.x, touch->point.y, 100, 50);
//		button->MoveAnimation(dest, 0.5f, Interpolation::EASY_IN_EASY_OUT);
//	}
//
//	if ((touch->phase == UIEvent::PHASE_ENDED) && (touch->buttonId == UIEvent::BUTTON_2))
//	{
//	}
}

void TestScreen::OnSwitchScreenMode(BaseObject * sender, void * userData, void * callerData)
{
 	int32 screenMode = Core::Instance()->GetScreenMode();
 	if (screenMode == Core::MODE_FULLSCREEN)
 	{
 		Core::Instance()->SwitchScreenToMode(Core::MODE_WINDOWED);
 	}else if (screenMode == Core::MODE_WINDOWED)
 	{
 		Core::Instance()->SwitchScreenToMode(Core::MODE_FULLSCREEN);
 	}
}

GLuint textureName = 0;
void TestScreen::Draw(const UIGeometricData &geometricData)
{
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
/*	glBindTexture(GL_TEXTURE_2D, testSprite->GetTexture()->id);

	const int width = 16;
	const int height = 16;

	uint8 data[width * height * 4];

	for (int w = 0; w < width; ++w)
		for (int h = 0; h < height; ++h)
		{
			int k = h * width + w;
			data[k * 4 + 0] = 255;
			data[k * 4 + 1] = 0;//(float)w / (float)width * 255.0f;
			data[k * 4 + 2] = 0;//(float)h / (float)height * 255.0f;
			data[k * 4 + 3] = 255;
		}
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,  data);
*/

	testSprite->SetPosition(100, 100);
	testSprite->SetScale(5, 5);
	testSprite->SetAngle(0);
	testSprite->Draw();
	x += dx;

	
	testSprite->SetPosition(0, 100);
	testSprite->Draw();
	testSprite->SetPosition(13, 100);
	testSprite->Draw();
	testSprite->SetPosition(26, 100);
	testSprite->Draw();
	testSprite->SetPosition(13, 112);
	testSprite->Draw();
	testSprite->SetPosition(13, 124);
	testSprite->Draw();
	
// 	if (x + 200 >= RenderManager::Instance()->GetScreenWidth())
// 	{
// 		dx = -dx;
// 	}
// 	else if (x < 0)
// 	{
// 		dx = -dx;
// 	}	
	
	/*
//	glViewport( 0, 0, 800, 600);
//	
//    glMatrixMode( GL_PROJECTION );
//    glLoadIdentity();
//    gluPerspective( 30, 800 / 600, 1.0, 1000.0 );
//	
//    glMatrixMode( GL_MODELVIEW );
//    glLoadIdentity();
	
    // Set up rendering state.
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_CULL_FACE );
    glDisable( GL_LIGHTING );
    glDisable( GL_LIGHT0 );
	
    // Upload the texture.  Since we are sharing OpenGL object between our FullScreen and non-FullScreen contexts, we only need to do this once.
    if (textureName == 0) 
	{
        glGenTextures( 1, &textureName );
		glBindTexture(GL_TEXTURE_2D, textureName);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	}
	
	glColor4f(1, 1, 1, 1);
    // Set up texturing parameters.
    //glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, textureName );
	
    // Clear the framebuffer.
    glClearColor( 0, 0, 0, 0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    glPushMatrix();
    
	//      // Set up our single directional light (the Sun!).
	//        lightDirection[0] = cos(dtor(sunAngle));
	//        lightDirection[2] = sin(dtor(sunAngle));
	//        glLightfv( GL_LIGHT0, GL_POSITION, lightDirection );
	//
	//        // Back the camera off a bit.
	//        glTranslatef( 0.0, 0.0, -1.5 );
	//
	//        // Draw the Earth!
	//        quadric = gluNewQuadric();
	//        if (wireframe) {
	//            gluQuadricDrawStyle( quadric, GLU_LINE );
	//        }
	//        gluQuadricTexture( quadric, GL_TRUE );
	//        glMaterialfv( GL_FRONT, GL_AMBIENT, materialAmbient );
	//        glMaterialfv( GL_FRONT, GL_DIFFUSE, materialDiffuse );
	//        glRotatef( rollAngle, 1.0, 0.0, 0.0 );
	//        glRotatef( -23.45, 0.0, 0.0, 1.0 ); // Earth's axial tilt is 23.45 degrees from the plane of the ecliptic.
	//        glRotatef( animationPhase * 360.0, 0.0, 1.0, 0.0 );
	//        glRotatef( 90.0, 1.0, 0.0, 0.0 );
	//        //gluSphere( quadric, radius, 48, 24 );
	//		
	//		gluDeleteQuadric(quadric);
	//        quadric = NULL;
	
	
	const int width = 16;
	const int height = 16;
	
	uint8 data[width * height * 4];
	
	for (int w = 0; w < width; ++w)
		for (int h = 0; h < height; ++h)
		{
			int k = h * width + w;
			data[k * 4 + 0] = 0;
			data[k * 4 + 1] = (float)w / (float)width * 255.0f;
			data[k * 4 + 2] = (float)h / (float)height * 255.0f;
			data[k * 4 + 3] = 255;
		}
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,  data);
	
	glColor4f(1, 1, 1, 1);
	glBegin( GL_QUADS );
	glTexCoord2d(0.0,0.0); glVertex3d(0.0, 0.0, 0);
	glTexCoord2d(1.0,0.0); glVertex3d(30.0, 0.0, 0);
	glTexCoord2d(1.0,1.0); glVertex3d(30.0, 30.0, 0);
	glTexCoord2d(0.0,1.0); glVertex3d(0.0, 30.0, 0);
	glEnd();
	
	glFlush();
	
    glPopMatrix();
    
    // Flush out any unfinished rendering before swapping.
    glFinish();
	
	int32 texId = testSprite->GetTexture()->id;
	glBindTexture(GL_TEXTURE_2D, texId);
	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	
	for (int w = 0; w < width; ++w)
		for (int h = 0; h < height; ++h)
		{
			int k = h * width + w;
			data[k * 4 + 0] = 0;
			data[k * 4 + 1] = (float)w / (float)width * 255.0f;
			data[k * 4 + 2] = (float)h / (float)height * 255.0f;
			data[k * 4 + 3] = 255;
		}
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,  data);
	
	testSprite->SetPosition(x, 200);
	testSprite->Draw();

	x += dx;
	if (x + 200 >= RenderManager::Instance()->GetScreenWidth())
	{
		dx = -dx;
	}
	else if (x < 0)
	{
		dx = -dx;
	}

	testSprite->SetPosition(x, 200);
	testSprite->Draw();
	
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ZERO);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	
	glBindTexture(GL_TEXTURE_2D, testSprite->GetTexture()->id);
		
	const int width = 16;
	const int height = 16;
	
	uint8 data[width * height * 4];
	
	for (int w = 0; w < width; ++w)
		for (int h = 0; h < height; ++h)
		{
			int k = h * width + w;
			data[k * 4 + 0] = 0;
			data[k * 4 + 1] = (float)w / (float)width * 255.0f;
			data[k * 4 + 2] = (float)h / (float)height * 255.0f;
			data[k * 4 + 3] = 255;
		}
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,  data);
	
	
	glEnable( GL_DEPTH_TEST );
    glDisable( GL_CULL_FACE );
	
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
					GL_LINEAR_MIPMAP_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
	
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

	glColor4f(1, 1, 1, 1);
	glBegin( GL_QUADS );
	glTexCoord2d(0.0,0.0); glVertex2d(0.0,0.0);
	glTexCoord2d(1.0,0.0); glVertex2d(30.0,0.0);
	glTexCoord2d(1.0,1.0); glVertex2d(30.0,30.0);
	glTexCoord2d(0.0,1.0); glVertex2d(0.0,30.0);
	glEnd();
	
	
	
	RenderManager::Instance()->SetColor(1, 0, 0, 1);
	RenderManager::Instance()->FillRect(Rect2f(x, 10, 100, 100));
	RenderManager::Instance()->SetColor(1, 1, 0, 1);
	RenderManager::Instance()->FillRect(Rect2f(x, 100, 50, 50));
	x += dx;
	if (x + 200 >= RenderManager::Instance()->GetScreenWidth())
	{
		dx = -dx;
	}
	else if (x < 0)
	{
		dx = -dx;
	 }	*/

}
