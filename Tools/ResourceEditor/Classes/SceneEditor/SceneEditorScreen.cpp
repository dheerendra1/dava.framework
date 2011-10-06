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
#include "./../Collada/ColladaConvert.h"

void SceneEditorScreen::LoadResources()
{
    //RenderManager::Instance()->EnableOutputDebugStatsEveryNFrame(30);

    GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
    GetBackground()->SetColor(Color(0.7f, 0.7f, 0.7f, 1.0f));

    
    scene = new Scene();

    
//    SceneFile * file = new SceneFile();
//	file->LoadScene("~res:/Scenes/vit/scene.sce", scene);
//    scene->AddNode(scene->GetRootNode("~res:/Scenes/vit/scene.sce"));
//	SafeRelease(file);
        
    
	scene3dView = 0;
    scene3dView = new UI3DView(Rect(200, 100, 480, 320));
    scene3dView->SetDebugDraw(true);
    scene3dView->SetScene(scene);
    scene3dView->SetInputEnabled(false);
    AddControl(scene3dView);


    Camera * cam = new Camera(scene);
    scene->AddCamera(cam);
    cam->Setup(70.0f, 480.0f / 320.0f, 1.0f, 5000.0f); 
    scene->SetCurrentCamera(cam);
    cam->SetDebugFlags(SceneNode::DEBUG_DRAW_ALL);
    cam->SetUp(Vector3(0.0f, 0.0f, 1.0f));
    
//    Camera * cam = scene->GetCamera(0);
//    cam->Setup(83.0f, 480.0f / 320.0f, 1.0f, 5000.0f); 
//    scene->SetCurrentCamera(cam);
//    //cam->SetFOV(90.0f);
//    cam->SetDebugFlags(SceneNode::DEBUG_DRAW_ALL);
//    
//    Camera * cam2 = scene->GetCamera(0);
//    scene->SetClipCamera(cam2);
    
    // 483, -2000, 119
    LandscapeNode * node = new LandscapeNode(scene);
    AABBox3 box(Vector3(198, 201, 0), Vector3(-206, -203, 22.7f));
    //box.min += cam->GetPosition();
    //box.max += cam->GetPosition();
    //box.min -= Vector3(512, 512, 0);
    //box.max = Vector3(512, 512, 0);
    
    //node->SetDebugFlags(LandscapeNode::DEBUG_DRAW_ALL);
    node->BuildLandscapeFromHeightmapImage("~res:/Landscape/hmp2_1.png", box);

    Texture::EnableMipmapGeneration();
    Texture * tex = Texture::CreateFromFile("~res:/Landscape/tex3.png");
    node->SetTexture(LandscapeNode::TEXTURE_BASE, tex);
    SafeRelease(tex);
    
    Texture * detailTex = Texture::CreateFromFile("~res:/Landscape/detail_gravel.png");
    node->SetTexture(LandscapeNode::TEXTURE_DETAIL, detailTex);

    SafeRelease(detailTex);
    Texture::DisableMipmapGeneration();
    
    node->SetName("landscapeNode");
    scene->AddNode(node);
    
    
    sceneTree = new UIHierarchy(Rect(0, 100, 200, size.y - 120));
    sceneTree->SetCellHeight(20);
    sceneTree->SetDelegate(this);
    sceneTree->SetClipContents(true);
    AddControl(sceneTree);
    
    sceneTree->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
    sceneTree->GetBackground()->SetColor(Color(0.92f, 0.92f, 0.92f, 1.0f));
    
    selectedNode = 0;
    
    cameraController = new WASDCameraController(40);
    cameraController->SetCamera(cam);
    
    
    activePropertyPanel = 0;
    cameraPanel = 0;
    
    localMatrixControl = new EditMatrixControl(Rect(0, 0, 300, 100));
    worldMatrixControl = new EditMatrixControl(Rect(0, 0, 300, 100));

    Font *f = FTFont::Create("~res:/Fonts/MyriadPro-Regular.otf");
    f->SetSize(12);
    f->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
    
    
    lookAtButton = new UIButton(Rect(0, 0, 300, 30));
    lookAtButton->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    lookAtButton->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    lookAtButton->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    lookAtButton->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    lookAtButton->SetStateDrawType(UIControl::STATE_DISABLED, UIControlBackground::DRAW_FILL);
    lookAtButton->GetStateBackground(UIControl::STATE_DISABLED)->SetColor(Color(0.2, 0.2, 0.2, 0.2));
    lookAtButton->SetStateFont(UIControl::STATE_NORMAL, f);
    lookAtButton->SetStateText(UIControl::STATE_NORMAL, L"Look At Object");
    
    removeNodeButton = dynamic_cast<UIButton*>(lookAtButton->Clone());
    removeNodeButton->SetStateFont(UIControl::STATE_NORMAL, f);
    removeNodeButton->SetStateText(UIControl::STATE_NORMAL, L"Remove Object");
    SafeRelease(f);
    
    lookAtButton->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &SceneEditorScreen::OnLookAtButtonPressed));
    removeNodeButton->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &SceneEditorScreen::OnRemoveNodeButtonPressed));

    
    activePropertyPanel = new PropertyPanel(Rect(720, 100, 300, size.y - 120));
    
    nodeName = SafeRetain(activePropertyPanel->AddHeader(L"Node name:"));
    
    activePropertyPanel->AddHeader(L"Local Matrix:");
    activePropertyPanel->AddPropertyControl(localMatrixControl);
    activePropertyPanel->AddHeader(L"World Matrix:");
    activePropertyPanel->AddPropertyControl(worldMatrixControl);
    nodeBoundingBoxMin = SafeRetain(activePropertyPanel->AddHeader(L"-"));
    nodeBoundingBoxMax = SafeRetain(activePropertyPanel->AddHeader(L"-"));
    activePropertyPanel->AddPropertyControl(lookAtButton);
    activePropertyPanel->AddPropertyControl(removeNodeButton);
    
    AddControl(activePropertyPanel);
    
    fileSystemDialog = new UIFileSystemDialog("~res:/Fonts/MyriadPro-Regular.otf");
    fileSystemDialog->SetDelegate(this);
    fileSystemDialog->SetCurrentDir("/Sources/dava.framework/Tools/Bin");
    
    CreateTopMenu();
}

void SceneEditorScreen::UnloadResources()
{
    ReleaseTopMenu();
    
    SafeRelease(nodeName);
    SafeRelease(nodeBoundingBoxMin);
    SafeRelease(nodeBoundingBoxMax);
    SafeRelease(lookAtButton);
    SafeRelease(removeNodeButton);
    
    
    SafeRelease(cameraController);
    
    SafeRelease(scene3dView);
    SafeRelease(sceneTree);
	SafeRelease(scene);
}

void SceneEditorScreen::CreateTopMenu()
{
    Font *f = FTFont::Create("~res:/Fonts/MyriadPro-Regular.otf");
    f->SetSize(12);
    f->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
    
    openButton = new UIButton(Rect(0, 0, 150, 30));
    openButton->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    openButton->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    openButton->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    openButton->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    openButton->SetStateDrawType(UIControl::STATE_DISABLED, UIControlBackground::DRAW_FILL);
    openButton->GetStateBackground(UIControl::STATE_DISABLED)->SetColor(Color(0.2, 0.2, 0.2, 0.2));
    openButton->SetStateFont(UIControl::STATE_NORMAL, f);
    openButton->SetStateText(UIControl::STATE_NORMAL, L"Open (.sce)");
    openButton->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &SceneEditorScreen::OnTopMenuOpenPressed));
    
    AddControl(openButton);
    
    convertButton = new UIButton(Rect(openButton->GetRect().x + openButton->GetRect().dx + 4.0f, 0, 150, 30));
    convertButton->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    convertButton->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    convertButton->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    convertButton->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    convertButton->SetStateDrawType(UIControl::STATE_DISABLED, UIControlBackground::DRAW_FILL);
    convertButton->GetStateBackground(UIControl::STATE_DISABLED)->SetColor(Color(0.2, 0.2, 0.2, 0.2));
    convertButton->SetStateFont(UIControl::STATE_NORMAL, f);
    convertButton->SetStateText(UIControl::STATE_NORMAL, L"Convert (.dae)");
    convertButton->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &SceneEditorScreen::OnTopMenuConvertPressed));
    
    AddControl(convertButton);
    SafeRelease(f);
}  

void SceneEditorScreen::ReleaseTopMenu()
{
    SafeRelease(openButton);
    SafeRelease(convertButton);
}

void SceneEditorScreen::OnTopMenuOpenPressed(BaseObject * obj, void *, void *)
{
    fileSystemDialog->SetExtensionFilter(".sce");
    fileSystemDialog->Show(this);
    fileSystemDialogOpMode = DIALOG_OPERATION_OPEN_SCE;
}

void SceneEditorScreen::OnTopMenuConvertPressed(BaseObject * obj, void *, void *)
{
    fileSystemDialog->SetExtensionFilter(".dae");
    fileSystemDialog->Show(this);
    fileSystemDialogOpMode = DIALOG_OPERATION_CONVERT_DAE;
}

void SceneEditorScreen::OnFileSelected(UIFileSystemDialog *forDialog, const String &pathToFile)
{
    switch (fileSystemDialogOpMode) {
        case DIALOG_OPERATION_CONVERT_DAE:
        {
            ConvertDaeToSce(pathToFile);
        }
        break;
        case DIALOG_OPERATION_OPEN_SCE:
        {
            SceneFile * file = new SceneFile();
            file->SetDebugLog(true);
            file->LoadScene(pathToFile.c_str(), scene);
            scene->AddNode(scene->GetRootNode(pathToFile));
            SafeRelease(file);
            
            if (scene->GetCamera(0))
            {
                scene->SetCurrentCamera(scene->GetCamera(0));
                cameraController->SetCamera(scene->GetCamera(0));
                
            }
            sceneTree->Refresh();
        }
        break;
        default:
            break;
    }
}

void SceneEditorScreen::OnFileSytemDialogCanceled(UIFileSystemDialog *forDialog)
{
    
}

void SceneEditorScreen::WillAppear()
{
    for (int32 k = 0; k < 32; ++k)
    {
        Logger::Debug("%d clz: %d", k, CountLeadingZeros ( 1 << k));  
    }
}

void SceneEditorScreen::WillDisappear()
{
	
}

void SceneEditorScreen::Input(UIEvent * event)
{
    cameraController->Input(event);
    
    if (event->phase == UIEvent::PHASE_KEYCHAR)
    {
        if (event->keyChar == '1')
            cameraController->SetSpeed(40);
        if (event->keyChar == '2')
            cameraController->SetSpeed(80);
        if (event->keyChar == '3')
            cameraController->SetSpeed(160);
        if (event->keyChar == '4')
            cameraController->SetSpeed(320);
            
        Camera * newCamera = 0;
        if (event->keyChar == 'z')newCamera = scene->GetCamera(0);
        if (event->keyChar == 'x')newCamera = scene->GetCamera(1);
        if (event->keyChar == 'c')newCamera = scene->GetCamera(2);
        if (event->keyChar == 'v')newCamera = scene->GetCamera(3);
        if (event->keyChar == 'b')newCamera = scene->GetCamera(4);
        if (newCamera)
        {
            scene->SetCurrentCamera(newCamera);
            scene->SetClipCamera(scene->GetCamera(0));
        }
    }   
    
	if (event->phase == UIEvent::PHASE_BEGAN)
	{
		inTouch = true;	
		touchStart = event->point;
		touchTankAngle = currentTankAngle;
	}
	
	if (event->phase == UIEvent::PHASE_DRAG)
	{
		touchCurrent = event->point;
		
		float32 dist = (touchCurrent.x - touchStart.x);
		//Logger::Debug("%f, %f", currentTankAngle, dist);
		currentTankAngle = touchTankAngle + dist;
	}
	
	if (event->phase == UIEvent::PHASE_ENDED)
	{
		touchCurrent = event->point;
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
            AABBox3 bbox = mesh->GetBoundingBox();
            AABBox3 transformedBox;
            bbox.GetTransformedBox(mesh->worldTransform, transformedBox);

            mesh->SetDebugFlags(SceneNode::DEBUG_DRAW_AABBOX | SceneNode::DEBUG_DRAW_LOCAL_AXIS);
            nodeBoundingBoxMin->SetText(Format(L"Min: (%0.2f, %0.2f, %0.2f)", 
                                            transformedBox.min.x, transformedBox.min.y, transformedBox.min.z));
            nodeBoundingBoxMax->SetText(Format(L"Max: (%0.2f, %0.2f, %0.2f)", 
                                               transformedBox.max.x, transformedBox.max.y, transformedBox.max.z));
        }else
        {
            nodeBoundingBoxMin->SetText(L"Bounding Box:");
            nodeBoundingBoxMax->SetText(L"Not available for this node");
        }
        
        Camera * camera = dynamic_cast<Camera*> (node);
        if (camera)
        {
            scene->SetCurrentCamera(camera);
            Camera * cam2 = scene->GetCamera(0);
            scene->SetClipCamera(cam2);
            //cameraController->SetCamera(camera);
            
            nodeBoundingBoxMin->SetText(Format(L"fov: %f, aspect: %f", camera->GetFOV(), camera->GetAspect()));
            nodeBoundingBoxMax->SetText(Format(L"znear: %f, zfar: %f", camera->GetZNear(), camera->GetZFar()));
        }
        
//        MeshInstanceNode *turretN = (MeshInstanceNode*)scene->FindByName("node-lod0_turret_02")->FindByName("instance_0");
//        //    turretN->localTransform.CreateScale(Vector3(0.7, 0.7, 0.7));
//        turretN->localTransform.CreateRotation(Vector3(0,0,1), DegToRad(90));
//        turretN->SetDebugFlags();
        localMatrixControl->SetMatrix(&selectedNode->localTransform);
        worldMatrixControl->SetMatrix(&selectedNode->worldTransform);
        
        nodeName->SetText(StringToWString(selectedNode->GetFullName()));
    }
}

void SceneEditorScreen::OnLookAtButtonPressed(BaseObject * obj, void *, void *)
{
    MeshInstanceNode * mesh = dynamic_cast<MeshInstanceNode*>(selectedNode);
    if (mesh)
    {
        AABBox3 bbox = mesh->GetBoundingBox();
        AABBox3 transformedBox;
        bbox.GetTransformedBox(mesh->worldTransform, transformedBox);
        Vector3 center = transformedBox.GetCenter();
        scene->GetCurrentCamera()->SetTarget(center);
    }
}

void SceneEditorScreen::OnRemoveNodeButtonPressed(BaseObject * obj, void *, void *)
{
    if (selectedNode)
    {
        SceneNode * parentNode = selectedNode->GetParent();
        if (parentNode)
        {
            parentNode->RemoveNode(selectedNode);
            selectedNode = 0;
            sceneTree->Refresh();
        }
    }
}
