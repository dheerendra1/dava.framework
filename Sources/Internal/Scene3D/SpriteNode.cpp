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

#include "Scene3D/SpriteNode.h"
#include "Scene3D/Scene.h"
#include "Render/RenderManager.h"
#include "Render/RenderHelper.h"
#include "Render/2D/Sprite.h"
namespace DAVA 
{

SpriteNode::SpriteNode(Scene * _scene, const String &pathToSprite, int32 _frame
                       , const Vector2 &reqScale, const Vector2 &pivotPoint)
:   SceneNode(_scene)
{
    sprite = Sprite::Create(pathToSprite);
    sprScale = reqScale;
    sprPivot = pivotPoint;
    frame = _frame;
    for (int i = 0; i < sprite->GetFrameCount(); i++) 
    {
        CreateMeshFromSprite(i);
    }
    renderData = new RenderDataObject();
    renderData->SetStream(EVF_VERTEX, TYPE_FLOAT, 3, 0, verts.data());
    renderData->SetStream(EVF_TEXCOORD0, TYPE_FLOAT, 2, 0, textures.data());

}

SpriteNode::SpriteNode(Scene * _scene, Sprite *spr, int32 _frame
           , const Vector2 &reqScale, const Vector2 &pivotPoint)
:   SceneNode(_scene)
{
    sprScale = reqScale;
    sprPivot = pivotPoint;
    sprite = SafeRetain(spr);
    frame = _frame;
    for (int i = 0; i < sprite->GetFrameCount(); i++) 
    {
        CreateMeshFromSprite(i);
    }
    renderData = new RenderDataObject();
    renderData->SetStream(EVF_VERTEX, TYPE_FLOAT, 3, 0, verts.data());
    renderData->SetStream(EVF_TEXCOORD0, TYPE_FLOAT, 2, 0, textures.data());
    type = TYPE_OBJECT;
}

SpriteNode::~SpriteNode()
{
    SafeRelease(sprite);
    SafeRelease(renderData);
}

void SpriteNode::SetFrame(int32 newFrame)
{
    frame = newFrame;
}

int32 SpriteNode::GetFrame()
{
    return frame;
}


void SpriteNode::CreateMeshFromSprite(int32 frameToGen)
{
    float32 x0 = sprite->GetRectOffsetValueForFrame(frameToGen, Sprite::X_OFFSET_TO_ACTIVE) - sprPivot.x;
    float32 y0 = sprite->GetRectOffsetValueForFrame(frameToGen, Sprite::Y_OFFSET_TO_ACTIVE) - sprPivot.y;
    float32 x1 = x0 + sprite->GetRectOffsetValueForFrame(frameToGen, Sprite::ACTIVE_WIDTH);
    float32 y1 = y0 + sprite->GetRectOffsetValueForFrame(frameToGen, Sprite::ACTIVE_HEIGHT);
    x0 *= sprScale.x;
    x1 *= sprScale.y;
    y0 *= sprScale.x;
    y1 *= sprScale.y;

        //triangle 1
        //0, 0
    verts.push_back(x0);
    verts.push_back(y0);
    verts.push_back(0);
    
        //1, 0
    verts.push_back(x1);
    verts.push_back(y0);
    verts.push_back(0);
    
    
        //0, 1
    verts.push_back(x0);
    verts.push_back(y1);
    verts.push_back(0);
    
        //1, 1
    verts.push_back(x1);
    verts.push_back(y1);
    verts.push_back(0);


    float32 *pT = sprite->GetTextureVerts(frameToGen);
    for (int i = 0; i < 2*4; i++) 
    {
        textures.push_back(*pT);
        pT++;
    }
}
    
void SpriteNode::SetType(eType _type)
{
    type = _type;
}
    
SpriteNode::eType SpriteNode::GetType()
{
    return type;
}


void SpriteNode::Draw()
{
	if (!visible)return;
    
    // Get current modelview matrix, and in this case it's always a camera matrix
	Matrix4 cameraMatrix = RenderManager::Instance()->GetMatrix(RenderManager::MATRIX_MODELVIEW); 
    Matrix4 meshFinalMatrix;
    
    switch(type)
    {
        case TYPE_OBJECT:
        {
            meshFinalMatrix = worldTransform * cameraMatrix;
            break;
        };
        case TYPE_BILLBOARD:
        {
            Matrix4 inverse(Matrix4::IDENTITY);
            
            inverse._00 = cameraMatrix._00;
            inverse._01 = cameraMatrix._10;
            inverse._02 = cameraMatrix._20;
            
            inverse._10 = cameraMatrix._01;
            inverse._11 = cameraMatrix._11;
            inverse._12 = cameraMatrix._21;
            
            inverse._20 = cameraMatrix._02;
            inverse._21 = cameraMatrix._12;
            inverse._22 = cameraMatrix._22;
            
            meshFinalMatrix = inverse * worldTransform * cameraMatrix;
            break;
        };
        case TYPE_BILLBOARD_TO_CAMERA:
        {
            Camera * camera = scene->GetCurrentCamera();
            Vector3 look = camera->GetPosition() - Vector3(0.0f, 0.0f, 0.0f) * worldTransform; 
            look.Normalize();
            Vector3 right = CrossProduct(camera->GetUp(), look);
            Vector3 up = CrossProduct(look, right);

            Matrix4 matrix = Matrix4::IDENTITY;
            matrix._00 = right.x;
            matrix._10 = right.y;
            matrix._20 = right.z;
            
            matrix._01 = up.x;
            matrix._11 = up.y;
            matrix._21 = up.z;

            matrix._02 = look.x;
            matrix._12 = look.y;
            matrix._22 = look.z;
            
            meshFinalMatrix = matrix * worldTransform * cameraMatrix;

//            left.x = cameraTransform._00;
//            left.y = cameraTransform._10;
//            left.z = cameraTransform._20;
//            up.x = cameraTransform._01;
//            up.y = cameraTransform._11;
//            up.z = cameraTransform._21;
//            target.x = position.x - cameraTransform._02;
//            target.y = position.y - cameraTransform._12;
//            target.z = position.z - cameraTransform._22;
            break;
        };   
    }
    

        //TODO: Add billboards mode
//    {//billboarding
//        Vector3 camDir = scene->GetCamera()->GetDirection();
//        Vector3 right;
//        Vector3 up(0.f,0.f,1.f);
//        right = up.CrossProduct(camDir);
//        up = camDir.CrossProduct(right);
////        up.Normalize();
////        right.Normalize();
//        
//        Matrix4 bm;
//        
//        bm._00 = right.x;
//        bm._10 = right.y;
//        bm._20 = right.z;
//        bm._30 = 0.0f;
//        
//        bm._01 = up.x;
//        bm._11 = up.y;
//        bm._21 = up.z;
//        bm._31 = 0.0f;
//        
//        bm._02 = camDir.x;
//        bm._12 = camDir.y;
//        bm._22 = camDir.z;
//        bm._32 = 0.0f;
//        
//        bm._03 = 0.0f;
//        bm._13 = 0.0f;
//        bm._23 = 0.0f;
//        bm._33 = 1.0f;
//        
//        meshFinalMatrix = bm * meshFinalMatrix;
////        meshFinalMatrix = bm * prevMatrix;
////        meshFinalMatrix = worldTransform * meshFinalMatrix;
//    }
    
    RenderManager::Instance()->SetMatrix(RenderManager::MATRIX_MODELVIEW, meshFinalMatrix);
    RenderManager::Instance()->SetRenderEffect(RenderManager::TEXTURE_MUL_FLAT_COLOR);

    
    
    
    RenderManager::Instance()->SetColor(1, 1, 1, 1);
    RenderManager::Instance()->EnableBlending(true);
    RenderManager::Instance()->EnableTexturing(true);//TODO: Move all this code to the RenderState node
    RenderManager::Instance()->EnableDepthTest(false);
    RenderManager::Instance()->EnableDepthWrite(false);
    RenderManager::Instance()->SetTexture(sprite->GetTexture(frame));
//	RenderManager::Instance()->FlushState();
    
    RenderManager::Instance()->SetRenderData(renderData);

    
    RenderManager::Instance()->DrawArrays(PRIMITIVETYPE_TRIANGLESTRIP, frame * 4, 4);


    
//    glDisableClientState(GL_COLOR_ARRAY);
    RenderManager::Instance()->EnableTexturing(true);
    RenderManager::Instance()->EnableBlending(false);
    RenderManager::Instance()->EnableDepthTest(true);
    RenderManager::Instance()->EnableDepthWrite(true);
    
    if (debugFlags & DEBUG_DRAW_ALL)
    {
        AABBox3 box(Vector3(-0.5f, -0.5f, -0.5f), Vector3(0.5f, 0.5f, 0.5f));
        RenderHelper::Instance()->DrawBox(box);
    }
    
    RenderManager::Instance()->SetMatrix(RenderManager::MATRIX_MODELVIEW, cameraMatrix);
}
    
    

};










