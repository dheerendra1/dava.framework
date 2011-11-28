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

#include "Render/3D/StaticMesh.h"
#include "Scene3D/Scene.h"
#include "Render/RenderManager.h"
#include "FileSystem/Logger.h"

namespace DAVA 
{

StaticMesh::StaticMesh(Scene * _scene)
	: SceneNode(_scene)
{
	
}
	
StaticMesh::~StaticMesh()
{
	for (int32 p = 0; p < (int32)polygroupCount; ++p)
	{
        SafeRelease(polygroups[p]);
    }
    polygroups.clear();
}
	
void StaticMesh::Create(uint32 _polygroupCount)
{
	polygroupCount = _polygroupCount;
	for (int32 p = 0; p < (int32)polygroupCount; ++p)
	{
		polygroups.push_back(new PolygonGroup());
	}
}

//int32 StaticMesh::Release()
//{	
//}
	

void StaticMesh::DrawPolygonGroup(int32 index, Material * material)
{
//    Matrix4 proj = RenderManager::Instance()->GetMatrix(RenderManager::MATRIX_PROJECTION);
//    Matrix4 glMatrixProj;
//    glGetFloatv(GL_PROJECTION_MATRIX, glMatrixProj.data);
//    
//    LOG_AS_MATRIX4(proj);
//    LOG_AS_MATRIX4(glMatrixProj);
//    
//    Matrix4 modelView = RenderManager::Instance()->GetMatrix(RenderManager::MATRIX_MODELVIEW);
//    Matrix4 glModelView;
//    glGetFloatv(GL_MODELVIEW_MATRIX, glModelView.data);
//    
//    LOG_AS_MATRIX4(modelView);
//    LOG_AS_MATRIX4(glModelView);
    
    
    PolygonGroup * group = polygroups[index];
    
    RenderManager::Instance()->SetRenderEffect(RenderManager::TEXTURE_MUL_FLAT_COLOR);
    RenderManager::Instance()->SetRenderData(group->renderDataObject);
#if 1 // old material code
    if (material)
	{

		if (material->textures[Material::TEXTURE_DIFFUSE])
		{
			RenderManager::Instance()->SetTexture(material->textures[Material::TEXTURE_DIFFUSE]);
        }
        
        if (material->hasOpacity)
        {
            RenderManager::Instance()->SetRenderEffect(RenderManager::TEXTURE_MUL_FLAT_COLOR_ALPHA_TEST);
            //RenderManager::Instance()->EnableCulling(false);
            RenderManager::Instance()->SetState(RenderStateBlock::DEFAULT_3D_STATE | RenderStateBlock::STATE_CULL);
        }
	}
#else 
    if (material)
	{
        RenderManager::Instance()->SetShader(material->shader);

		if (material->textures[Material::TEXTURE_DIFFUSE])
		{
            RenderManager::Instance()->SetRenderData(group->renderDataObject);
			RenderManager::Instance()->SetTexture(material->textures[Material::TEXTURE_DIFFUSE]);
        }
        
        if (material->textures[Material::TEXTURE_DECAL])
        {
			RenderManager::Instance()->SetTexture(material->textures[Material::TEXTURE_DECAL], 1);
        }
        
        if (material->hasOpacity)
        {
            RenderManager::Instance()->EnableCulling(false);
        }
	}

#endif

	// render

	RenderManager::Instance()->DrawElements(PRIMITIVETYPE_TRIANGLELIST, group->indexCount, EIF_16, group->indexArray);
    
    
    RenderManager::Instance()->SetTexture(0, 1); // clear texture block 1
    //RenderManager::Instance()->EnableCulling(true);
    RenderManager::Instance()->SetState(RenderStateBlock::DEFAULT_3D_STATE);
}

void StaticMesh::Draw()
{
#ifdef __DAVAENGINE_IPHONE__
	glPushMatrix();
	
	for (uint32 k = 0; k < polygroupCount; ++k)
	{	
		DrawPolygonGroup(k, NULL);
	}
	glPopMatrix();
#endif
	SceneNode::Draw();
}
	
	

};