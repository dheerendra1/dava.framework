/*
    Copyright (c) 2008, DAVA Consulting, LLC
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  */

#include "SceneEditorScreen.h"

void SceneEditorScreen::LoadResources()
{
	scene = new Scene();
	SceneFile * file = new SceneFile();
	//file->SetDebugLog(true);

	file->LoadScene("~res:/Scenes/M3/M3.sce", scene);
    scene->AddNode(scene->GetRootNode("~res:/Scenes/M3/M3.sce"));

	SafeRelease(file);
    
    GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
    GetBackground()->SetColor(Color(0.7f, 0.7f, 0.7f, 1.0f));
    
    
    
//    MeshInstanceNode *turretN = (MeshInstanceNode*)scene->FindByName("node-lod0_turret_02")->FindByName("instance_0");
//    turretN->localTransform.CreateScale(Vector3(0.7, 0.7, 0.7));
//	turretN->localTransform.CreateRotation(Vector3(0,0,1), DegToRad(90));
//    turretN->SetDebugFlags(MeshInstanceNode::DEBUG_DRAW_AABBOX | MeshInstanceNode::DEBUG_DRAW_LOCAL_AXIS);

	currentTankAngle = 0.0f;
	inTouch = false;
	startRotationInSec = 0.0f;
	rotationSpeed = 8.0f;
    
	//  originalCameraPosition = scene->GetCamera(0)->GetPosition();
    //	positionJoypad = new UIJoypad(Rect(0, 320 - 80, 80, 80));
    //	positionJoypad->GetBackground()->SetSprite("~res:/Gfx/Joypad/joypad", 0);
    //	positionJoypad->SetStickSprite("~res:/Gfx/Joypad/joypad", 1);
    //	
    //	AddControl(positionJoypad);
    
	scene3dView = 0;
    scene3dView = new UI3DView(Rect(200, 100, 520, 480));
    scene3dView->SetDebugDraw(true);
    scene3dView->SetScene(scene);
    Camera * cam = scene->GetCamera(0);
    scene->SetCamera(cam);
    AddControl(scene3dView);
    
    
    
    hierarchy = new UIHierarchy(Rect(0, 100, 200, size.y));
    hierarchy->SetCellHeight(20);
    hierarchy->SetDelegate(this);
    hierarchy->SetClipContents(true);
    AddControl(hierarchy);
    
    hierarchy->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
    hierarchy->GetBackground()->SetColor(Color(0.92f, 0.92f, 0.92f, 1.0f));
    
    selectedNode = 0;
}

void SceneEditorScreen::UnloadResources()
{
    SafeRelease(scene3dView);
    SafeRelease(hierarchy);
	SafeRelease(scene);
}

void SceneEditorScreen::WillAppear()
{
}

void SceneEditorScreen::WillDisappear()
{
	
}

void SceneEditorScreen::Input(UIEvent * touch)
{
	if (touch->phase == UIEvent::PHASE_BEGAN)
	{
		inTouch = true;	
		touchStart = touch->point;
		touchTankAngle = currentTankAngle;
	}
	
	if (touch->phase == UIEvent::PHASE_DRAG)
	{
		touchCurrent = touch->point;
		
		float32 dist = (touchCurrent.x - touchStart.x);
		//Logger::Debug("%f, %f", currentTankAngle, dist);
		currentTankAngle = touchTankAngle + dist;
	}
	
	if (touch->phase == UIEvent::PHASE_ENDED)
	{
		touchCurrent = touch->point;
		rotationSpeed = (touchCurrent.x - touchStart.x);
		inTouch = false;
		startRotationInSec = 0.0f;
	}
}

void SceneEditorScreen::Update(float32 timeElapsed)
{
    
//    Camera * cam = scene->GetCamera(0);
//    Vector3 pos = cam->GetPosition();
//    cam->SetPosition(pos - cam->GetDirection());
    
	startRotationInSec -= timeElapsed;
	if (startRotationInSec < 0.0f)
		startRotationInSec = 0.0f;
    
	if (startRotationInSec == 0.0f)
	{
		if (Abs(rotationSpeed) > 8.0)
		{
			rotationSpeed = rotationSpeed * 0.8f;
		}
		
		currentTankAngle += timeElapsed * rotationSpeed;
	}
}

void SceneEditorScreen::Draw(const UIGeometricData &geometricData)
{
    UIScreen::Draw(geometricData);
    
    RenderManager::Instance()->ClearDepthBuffer();
    //glClearColor(0.0, 0.0, 0.0, 1.0f);
    //glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}


bool SceneEditorScreen::IsNodeExpandable(UIHierarchy *forHierarchy, void *forNode)
{
    if (forNode) 
    {
        return ((SceneNode*)forNode)->GetChildrenCount() > 0;
    }
    
    return scene->GetChildrenCount() > 0;
}

int32 SceneEditorScreen::ChildrenCount(UIHierarchy *forHierarchy, void *forParent)
{
    if (forParent) 
    {
        return ((SceneNode*)forParent)->GetChildrenCount();
    }
    
    return scene->GetChildrenCount();
}


void *SceneEditorScreen::ChildAtIndex(UIHierarchy *forHierarchy, void *forParent, int32 index)
{
    if (forParent) 
    {
        return ((SceneNode*)forParent)->GetChild(index);
    }
    
    return scene->GetChild(index);
}

UIHierarchyCell *SceneEditorScreen::CellForNode(UIHierarchy *forHierarchy, void *node)
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
    c->text->SetText(StringToWString(n->name));
    c->text->SetAlign(ALIGN_LEFT|ALIGN_VCENTER);
    SafeRelease(fnt);
    
    Color color(0.1, 0.5, 0.05, 1.0);
	c->openButton->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
	c->openButton->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
	c->openButton->SetStateDrawType(UIControl::STATE_HOVER, UIControlBackground::DRAW_FILL);
	c->openButton->GetStateBackground(UIControl::STATE_NORMAL)->color = color;
	c->openButton->GetStateBackground(UIControl::STATE_HOVER)->color = color + 0.1;
	c->openButton->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->color = color + 0.3;
 
    return c;//returns cell
}


void SceneEditorScreen::OnCellSelected(UIHierarchy *forHierarchy, UIHierarchyCell *selectedCell)
{
    UIHierarchyNode * hNode = selectedCell->GetNode();
    SceneNode * node = dynamic_cast<SceneNode*>((BaseObject*)hNode->GetUserNode());
    if (node)
    {
        MeshInstanceNode * prevSelMesh = dynamic_cast<MeshInstanceNode*>(selectedNode);
        if (prevSelMesh)
        {
            prevSelMesh->SetDebugFlags(0);
        }   
        selectedNode = node;
        MeshInstanceNode * mesh = dynamic_cast<MeshInstanceNode*>(node);
        if (mesh)
        {
            mesh->SetDebugFlags(MeshInstanceNode::DEBUG_DRAW_AABBOX | MeshInstanceNode::DEBUG_DRAW_LOCAL_AXIS);
        }
        
//        MeshInstanceNode *turretN = (MeshInstanceNode*)scene->FindByName("node-lod0_turret_02")->FindByName("instance_0");
//        //    turretN->localTransform.CreateScale(Vector3(0.7, 0.7, 0.7));
//        turretN->localTransform.CreateRotation(Vector3(0,0,1), DegToRad(90));
//        turretN->SetDebugFlags();

    
    }
}


