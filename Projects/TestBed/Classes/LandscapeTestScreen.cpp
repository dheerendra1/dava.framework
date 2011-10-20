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
#include "LandscapeTestScreen.h"

void LandscapeTestScreen::LoadResources()
{
//    RenderManager::Instance()->EnableOutputDebugStatsEveryNFrame(30);
    RenderManager::Instance()->SetFPS(30.0);
	scene = new Scene();
    
    
	scene3dView = 0;
    scene3dView = new UI3DView(Rect(0, 0, 480, 320));
    //scene3dView->SetDebugDraw(true);
    scene3dView->SetScene(scene);
    scene3dView->SetInputEnabled(false);
    AddControl(scene3dView);
    
    
    camera = new Camera(scene);
    scene->AddCamera(camera);
    camera->Setup(70.0f, 480.0f / 320.0f, 1.0f, 5000.0f); 
    scene->SetCurrentCamera(camera);
    camera->SetDebugFlags(SceneNode::DEBUG_DRAW_ALL);
    camera->SetUp(Vector3(0.0f, 0.0f, 1.0f));
    camera->SetPosition(Vector3(0.0f, 0.0f, 10.0f));
    scene->SetCurrentCamera(camera);
 
    LandscapeNode * node = new LandscapeNode(scene);
    AABBox3 box(Vector3(198, 201, 0), Vector3(-206, -203, 22.7f));
    
    //node->SetDebugFlags(LandscapeNode::DEBUG_DRAW_ALL);
#if 1
    node->BuildLandscapeFromHeightmapImage(LandscapeNode::RENDERING_MODE_DETAIL_SHADER, "~res:/Landscape/hmp2_1.png", box);
    
    Texture::EnableMipmapGeneration();
    node->SetTexture(LandscapeNode::TEXTURE_TEXTURE0, "~res:/Landscape/tex3.png");
    node->SetTexture(LandscapeNode::TEXTURE_DETAIL, "~res:/Landscape/detail_gravel.png");
    Texture::DisableMipmapGeneration();
#else
    node->BuildLandscapeFromHeightmapImage(LandscapeNode::RENDERING_MODE_BLENDED_SHADER, "~res:/Landscape/hmp2_1.png", box);
    
    Texture::EnableMipmapGeneration();
    node->SetTexture(LandscapeNode::TEXTURE_TEXTURE0, "~res:/Landscape/blend/d.png");
    node->SetTexture(LandscapeNode::TEXTURE_TEXTURE1, "~res:/Landscape/blend/s.png");
    node->SetTexture(LandscapeNode::TEXTURE_TEXTUREMASK, "~res:/Landscape/blend/mask.png");
    Texture::DisableMipmapGeneration();
#endif
    
    node->SetName("landscapeNode");
    scene->AddNode(node);
    SafeRelease(node);
    
    SceneFile * file = new SceneFile();
    file->SetDebugLog(true);
    file->LoadScene("~res:/Scenes/level0/scene.sce", scene);
    scene->AddNode(scene->GetRootNode("~res:/Scenes/level0/scene.sce"));
    SafeRelease(file);

	inTouch = false;
    
	scene3dView = 0;
    scene3dView = new UI3DView(Rect(0, 0, 480, 320));
    scene3dView->SetInputEnabled(false);
    scene3dView->SetScene(scene);
    
    AddControl(scene3dView);
    
    viewXAngle = 0;
    viewYAngle = 0; 
    
	positionJoypad = new UIJoypad(Rect(0, 320 - 80, 80, 80));
    positionJoypad->GetBackground()->SetSprite("~res:/Gfx/Joypad/joypad", 0);
    positionJoypad->SetStickSprite("~res:/Gfx/Joypad/joypad", 1);
	AddControl(positionJoypad);

    angleJoypad = new UIJoypad(Rect(480 - 80, 320 - 80, 80, 80));
    angleJoypad->GetBackground()->SetSprite("~res:/Gfx/Joypad/joypad", 0);
    angleJoypad->SetStickSprite("~res:/Gfx/Joypad/joypad", 1);
	AddControl(angleJoypad);
}  

void LandscapeTestScreen::UnloadResources()
{
    SafeRelease(scene3dView);
	SafeRelease(scene);
}

void LandscapeTestScreen::Input(UIEvent * touch)
{

//	if (touch->phase == UIEvent::PHASE_BEGAN)
//	{
//		inTouch = true;	
//		touchStart = touch->point;
//		touchTankAngle = currentTankAngle;
//	}
//	
//	if (touch->phase == UIEvent::PHASE_DRAG)
//	{
//		touchCurrent = touch->point;
//		
//		float32 dist = (touchCurrent.x - touchStart.x);
//		//Logger::Debug("%f, %f", currentTankAngle, dist);
//		currentTankAngle = touchTankAngle + dist;
//	}
//	
//	if (touch->phase == UIEvent::PHASE_ENDED)
//	{
//		touchCurrent = touch->point;
//		rotationSpeed = (touchCurrent.x - touchStart.x);
//		inTouch = false;
//		startRotationInSec = 0.0f;
//	}
    
//    if (touch->phase == UIEvent::PHASE_BEGAN) 
//    {
//        oldTouchPoint = touch->point;
//    }
//    else if(touch->phase == UIEvent::PHASE_DRAG || touch->phase == UIEvent::PHASE_ENDED)
//    {
//        Vector2 dp = oldTouchPoint - touch->point;
//        viewXAngle += dp.x * 0.5f;
//        viewYAngle += dp.y * 0.5f;
//        oldTouchPoint = touch->point;
//        //ClampAngles();
//        //LOG_AS_FLOAT(viewXAngle);
//        //LOG_AS_FLOAT(viewYAngle);
//    }
}

void LandscapeTestScreen::Update(float32 timeElapsed)
{
    Vector2 angleJoypadPos = angleJoypad->GetDigitalPosition();   
    viewXAngle += angleJoypadPos.x * timeElapsed * 25.0f;
    viewYAngle += angleJoypadPos.y * timeElapsed * 25.0f;
    
    aimUser.Identity();
    Matrix4 mt, mt2;
    mt.CreateTranslation(Vector3(0,10,0));
    aimUser *= mt;
    mt.CreateRotation(Vector3(0,0,1), DegToRad(viewXAngle));
    mt2.CreateRotation(Vector3(1,0,0), DegToRad(viewYAngle));
    mt2 *= mt;
    aimUser *= mt2;
    
    Vector3 dir = Vector3() * aimUser;
    
    Vector2 joypadPos = positionJoypad->GetDigitalPosition();
    
    Vector3 pos = camera->GetPosition();
    pos += -joypadPos.y * dir * timeElapsed * 4;
    //pos.y += joypadPos.y * dir.y;

    camera->SetPosition(pos);
    camera->SetDirection(dir);
}

void LandscapeTestScreen::Draw(const UIGeometricData &geometricData)
{
    RenderManager::Instance()->ClearDepthBuffer();
}

