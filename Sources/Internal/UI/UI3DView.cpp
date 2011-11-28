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

#include "UI/UI3DView.h"
#include "Scene3D/Scene.h"
#include "Render/RenderManager.h"
#include "Render/RenderHelper.h"
#include "Core/Core.h"

namespace DAVA 
{


UI3DView::UI3DView(const Rect &rect, bool rectInAbsoluteCoordinates)
    :   UIControl(rect, rectInAbsoluteCoordinates)
    ,   scene(0)
{

}

UI3DView::~UI3DView()
{
    SafeRelease(scene);
}


void UI3DView::SetScene(Scene * _scene)
{
    scene = SafeRetain(_scene);
    //
    for (int32 k = 0; k < scene->GetCameraCount(); ++k)
    {
        scene->GetCamera(k)->SetAspect(size.dy / size.dx);
    }
}

Scene * UI3DView::GetScene() const
{
    return scene;
}

void UI3DView::AddControl(UIControl *control)
{
    DVASSERT(0 && "UI3DView do not support children");
}

void UI3DView::SystemDraw(const UIGeometricData & geometricData)
{
    UIGeometricData drawData;
    drawData.position = relativePosition;
    drawData.size = size;
    drawData.pivotPoint = pivotPoint;
    drawData.scale = scale;
    drawData.angle = angle;
    drawData.AddToGeometricData(geometricData);

    const Rect & viewportRect = drawData.GetUnrotatedRect();
    
    
    if (debugDrawEnabled)
    {//TODO: Add debug draw for rotated controls
        RenderManager::Instance()->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
        RenderHelper::Instance()->DrawRect(viewportRect);
        RenderManager::Instance()->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
    }

    //glViewport(viewportRect.x, viewportRect.y, viewportRect.dx, viewportRect.dy);
    RenderManager::Instance()->PushDrawMatrix();
	RenderManager::Instance()->PushMappingMatrix();
    Matrix4 modelViewSave = RenderManager::Instance()->GetMatrix(RenderManager::MATRIX_MODELVIEW);
    Matrix4 projectionSave = RenderManager::Instance()->GetMatrix(RenderManager::MATRIX_PROJECTION);

    RenderManager::Instance()->SetViewport(viewportRect);

//  Not required because Scene should setup it state before draw
//    RenderManager::Instance()->EnableDepthWrite(true);
//    RenderManager::Instance()->EnableDepthTest(true);
//    RenderManager::Instance()->EnableBlending(false);
    
    Draw(geometricData);
       

//    RenderManager::Instance()->EnableDepthTest(false);
//    RenderManager::Instance()->EnableDepthWrite(false);
//    RenderManager::Instance()->EnableBlending(true);
    
    /*
        Restore render orientation
     */
    RenderManager::Instance()->SetViewport(Rect(0.0f, 0.0f, -1.0f, -1.0f));
    RenderManager::Instance()->SetRenderOrientation(Core::Instance()->GetScreenOrientation());

    RenderManager::Instance()->Reset(); // Boroda: Do we need this??
    
    //RenderManager::Instance()->SetState(RenderStateBlock::DEFAULT_2D_STATE_BLEND);
    
    RenderManager::Instance()->SetMatrix(RenderManager::MATRIX_MODELVIEW, modelViewSave);
    RenderManager::Instance()->SetMatrix(RenderManager::MATRIX_PROJECTION, projectionSave);
	RenderManager::Instance()->PopDrawMatrix();
	RenderManager::Instance()->PopMappingMatrix();
}
    
void UI3DView::Update(float32 timeElapsed)
{
    if (scene)
        scene->Update(timeElapsed);
}

void UI3DView::Draw(const UIGeometricData & geometricData)
{
    if (scene)
        scene->Draw();
}

}