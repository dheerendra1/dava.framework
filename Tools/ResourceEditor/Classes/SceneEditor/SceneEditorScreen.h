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
 
 
    * Created by Vitaliy Borodovsky
*/

#ifndef __SCENE_EDITOR_SCREEN_H__
#define __SCENE_EDITOR_SCREEN_H__

#include "DAVAEngine.h"
#include "CameraController.h"
#include "PropertyPanel.h"
#include "CameraPanel.h"
#include "EditMatrixControl.h"

using namespace DAVA;

class SceneEditorScreen : public UIScreen, public UIHierarchyDelegate, public UIFileSystemDialogDelegate
{
public:
	void ButtonPressed(BaseObject * owner, void * v);
	
	virtual void LoadResources();
	virtual void UnloadResources();
	virtual void WillAppear();
	virtual void WillDisappear();
	
	virtual void Update(float32 timeElapsed);
	virtual void Draw(const UIGeometricData &geometricData);
	virtual void Input(UIEvent * touch);
    
    
    void CreateTopMenu();
    void ReleaseTopMenu();
    
	UIButton * openButton;
    UIButton * convertButton;
    void OnTopMenuOpenPressed(BaseObject * obj, void *, void *);
    void OnTopMenuConvertPressed(BaseObject * obj, void *, void *);
    
    
    virtual bool IsNodeExpandable(UIHierarchy *forHierarchy, void *forNode);
    virtual int32 ChildrenCount(UIHierarchy *forHierarchy, void *forParent);
    virtual void *ChildAtIndex(UIHierarchy *forHierarchy, void *forParent, int32 index);
    virtual UIHierarchyCell *CellForNode(UIHierarchy *forHierarchy, void *node);
    virtual void OnCellSelected(UIHierarchy *forHierarchy, UIHierarchyCell *selectedCell);

    
    UIHierarchy * sceneTree;
    
	// 3d engine scene
	Scene * scene;
	Camera * activeCamera;
    
	float32 currentTankAngle;
	bool inTouch;
	Vector2 touchStart;
	Vector2 touchCurrent;
	float32 touchTankAngle;
	float32 rotationSpeed;
	
	float32 startRotationInSec;

	Vector3 originalCameraPosition;

	UIJoypad * positionJoypad;
    UI3DView * scene3dView;
    
    // Node preview information
    SceneNode * selectedNode;
    Matrix4 selectedNodeLocalTransform;
    Matrix4 selectedNodeWorldTransform;

    PropertyPanel * activePropertyPanel;
    EditMatrixControl * localMatrixControl;
    EditMatrixControl * worldMatrixControl;

    UIStaticText * nodeName;
    UIStaticText * nodeCenter;
    UIStaticText * nodeBoundingBoxMin;
    UIStaticText * nodeBoundingBoxMax;
    UIButton * lookAtButton;
    UIButton * removeNodeButton;
    UIButton * enableDebugFlagsButton;
    
    void OnLookAtButtonPressed(BaseObject * obj, void *, void *);
    void OnRemoveNodeButtonPressed(BaseObject * obj, void *, void *);
    void OnEnableDebugFlagsPressed(BaseObject * obj, void *, void *);

    UIFileSystemDialog * fileSystemDialog;
    enum {
        DIALOG_OPERATION_CONVERT_DAE = 1,
        DIALOG_OPERATION_OPEN_SCE,
    };
    uint32 fileSystemDialogOpMode;
    
    void OnFileSelected(UIFileSystemDialog *forDialog, const String &pathToFile);
    void OnFileSytemDialogCanceled(UIFileSystemDialog *forDialog);
    
    CameraPanel * cameraPanel;
    
    WASDCameraController * cameraController;
    UIStaticText * cameraInfo;
    UIStaticText * clipCameraInfo;
        
};

#endif // __SCENE_EDITOR_SCREEN_H__