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
#include "AnimationTestScreen.h"
#include "TestScreen.h"
#include "TouchEffectsControl.h"
#include "Scene3D/RotatingCubeNode.h"

void AnimationTestScreen::LoadResources()
{
    RenderManager::Instance()->EnableOutputDebugStatsEveryNFrame(30);
    RenderManager::Instance()->SetFPS(30.0);
	scene = new Scene();
    
    
//    cam = new Camera(scene);
//    scene->AddCamera(cam);
//    //cam->SetFOV(75.0f);
//    cam->Setup(60.0f, 320.0f / 480.0f, 1.0f, 3000.0f, false);
//    cam->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
//    cam->SetDirection(Vector3(1.0f, 0.0f, 0.0f));
//    cam->SetUp(Vector3(0.0f, 0.0f, 1.0f));
//
//    scene->SetCurrentCamera(cam);

	SceneFile * file = new SceneFile();
	//file->SetDebugLog(true);
	file->LoadScene("~res:/Scenes/vit/scene.sce", scene);
    scene->AddNode(scene->GetRootNode("~res:/Scenes/vit/scene.sce"));

//	file->LoadScene("~res:/Scenes/garage_lit/hungar.sce", scene);
//  scene->AddNode(scene->GetRootNode("~res:/Scenes/garage_lit/hungar.sce"));
//    file->LoadScene("~res:/Scenes/level_test/level_test_3.sce", scene);
//    scene->AddNode(scene->GetRootNode("~res:/Scenes/level_test/level_test_3.sce"));
    
	SafeRelease(file);

    LandscapeNode * node = new LandscapeNode(scene);
    AABBox3 box(Vector3(198, 201, 0), Vector3(-206, -203, 22.7f));
    //box.min += cam->GetPosition();
    //box.max += cam->GetPosition();
    //box.min -= Vector3(512, 512, 0);
    //box.max = Vector3(512, 512, 0);
    
    //node->SetDebugFlags(LandscapeNode::DEBUG_DRAW_ALL);
    node->BuildLandscapeFromHeightmapImage("~res:/Landscape/hmp2_1.png", box);
    
    Texture * tex = Texture::CreateFromFile("~res:/Landscape/tex3.png");
    node->SetTexture(LandscapeNode::TEXTURE_BASE, tex);

    Texture * detailTex = Texture::CreateFromFile("~res:/Landscape/detail.png");
    node->SetTexture(LandscapeNode::TEXTURE_DETAIL, detailTex);

    SafeRelease(detailTex);
    SafeRelease(tex);
    
    node->SetName("landscapeNode");
    scene->AddNode(node);


	currentTankAngle = 0.0f;
	inTouch = false;
	startRotationInSec = 0.0f;
	rotationSpeed = 8.0f;   
    

	scene3dView = 0;
    scene3dView = new UI3DView(Rect(0, 0, 480, 320));
    scene3dView->SetInputEnabled(false);
    scene3dView->SetScene(scene);
    
    cam = scene->GetCamera(0);
    cam->Setup(83.0f, 320.0f / 480.0f, 1.0f, 5000.0f); 

    //cam->Setup(60.0f, 
    scene->SetCurrentCamera(cam);
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

void AnimationTestScreen::UnloadResources()
{
    //SafeRelease(node);
    SafeRelease(scene3dView);
    SafeRelease(hierarchy);
	SafeRelease(scene);
}

void AnimationTestScreen::WillAppear()
{
}

void AnimationTestScreen::WillDisappear()
{
	
}

void AnimationTestScreen::Input(UIEvent * touch)
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

void AnimationTestScreen::Update(float32 timeElapsed)
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
    
    Vector3 pos = cam->GetPosition();
    pos += -joypadPos.y * dir * timeElapsed * 4;
    //pos.y += joypadPos.y * dir.y;

    cam->SetPosition(pos);
    cam->SetDirection(dir);
    
    
//    Camera * cam = scene->GetCamera(0);
//
//    Vector3 pos = cam->GetPosition();
//    cam->SetPosition(pos - cam->GetDirection());
//    
//	startRotationInSec -= timeElapsed;
//	if (startRotationInSec < 0.0f)
//		startRotationInSec = 0.0f;
//    
//	if (startRotationInSec == 0.0f)
//	{
//		if (Abs(rotationSpeed) > 8.0)
//		{
//			rotationSpeed = rotationSpeed * 0.8f;
//		}
//		
//		currentTankAngle += timeElapsed * rotationSpeed;
//	}
}

void AnimationTestScreen::Draw(const UIGeometricData &geometricData)
{
    //glClearColor(0.0, 0.0, 0.0, 1.0f);
    //glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    RenderManager::Instance()->ClearDepthBuffer();
}


bool AnimationTestScreen::IsNodeExpandable(UIHierarchy *forHierarchy, void *forNode)
{
    if (forNode) 
    {
        return ((SceneNode*)forNode)->GetChildrenCount() > 0;
    }
    
    return scene->GetChildrenCount() > 0;
}

int32 AnimationTestScreen::ChildrenCount(UIHierarchy *forHierarchy, void *forParent)
{
    if (forParent) 
    {
        return ((SceneNode*)forParent)->GetChildrenCount();
    }
    
    return scene->GetChildrenCount();
}


void *AnimationTestScreen::ChildAtIndex(UIHierarchy *forHierarchy, void *forParent, int32 index)
{
    if (forParent) 
    {
        return ((SceneNode*)forParent)->GetChild(index);
    }
    
    return scene->GetChild(index);
}

UIHierarchyCell *AnimationTestScreen::CellForNode(UIHierarchy *forHierarchy, void *node)
{
    UIHierarchyCell *c = forHierarchy->GetReusableCell("Node cell"); //try to get cell from the reusable cells store
    if(!c)
    { //if cell of requested type isn't find in the store create new cell
        c = new UIHierarchyCell(Rect(0, 0, 200, 15), "Node cell");
    }
        //fill cell whith data
    Font *fnt;
    fnt = FTFont::Create("~res:/Fonts/MyriadPro-Regular.otf");
//    fnt = GraphicsFont::Create("~res:/Fonts/korinna.def", "~res:/Gfx/Fonts2/korinna");
    fnt->SetSize(12);
    
    SceneNode *n = (SceneNode *)node;
    
    c->text->SetFont(fnt);
    c->text->SetText(StringToWString(n->GetName()));
    c->text->SetAlign(ALIGN_LEFT|ALIGN_VCENTER);
    SafeRelease(fnt);
    
    Color color(0.1f, 0.5f, 0.05f, 1.0f);
	c->openButton->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
	c->openButton->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
	c->openButton->SetStateDrawType(UIControl::STATE_HOVER, UIControlBackground::DRAW_FILL);
	c->openButton->GetStateBackground(UIControl::STATE_NORMAL)->color = color;
	c->openButton->GetStateBackground(UIControl::STATE_HOVER)->color = color + 0.1f;
	c->openButton->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->color = color + 0.3f;
 
    return c;//returns cell
}


void AnimationTestScreen::OnCellSelected(UIHierarchy *forHierarchy, UIHierarchyCell *selectedCell)
{
}


