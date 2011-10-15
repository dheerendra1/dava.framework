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
	//RenderManager::Instance()->SetTexture(textures[frameTextureIndex[frame]]);

	PolygonGroup * group = polygroups[index];
#if 0
	//	glColor4f(1.0f, 1.0f, 0.0f, 255.0f);
	glVertexPointer(3, GL_FLOAT, group->vertexStride, group->vertexArray);
	//	glNormalPointer(GL_FLOAT, cube->vertexStride, cube->normalArray);
	glColorPointer(4, GL_UNSIGNED_BYTE, group->vertexStride, group->colorArray);
	//	glTexCoordPointer(2, GL_FLOAT, cube->vertexStride, cube->textureCoordArray[0]);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	//	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	//	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glDrawElements(GL_TRIANGLES, group->indexCount, GL_UNSIGNED_SHORT, group->indexArray);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	//	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	//	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

#else
/*
	RenderManager::Instance()->SetVertexPointer(3, TYPE_FLOAT, group->vertexStride, group->vertexArray);
//	RenderManager::Instance()->SetNormalPointer(GL_FLOAT, group->vertexStride, group->normalArray);
//	RenderManager::Instance()->SetColorPointer(4, GL_UNSIGNED_BYTE, group->vertexStride, group->colorArray);
	RenderManager::Instance()->SetTexCoordPointer(2, TYPE_FLOAT, group->vertexStride, group->textureCoordArray[0]);
	
	RenderManager::Instance()->EnableVertexArray(true);
	RenderManager::Instance()->EnableTextureCoordArray(true);
*/
	// TODO replace with calls to RenderManager
	//glEnableClientState(GL_NORMAL_ARRAY);
	//glEnableClientState(GL_COLOR_ARRAY);

	// TODO set material from interface
	//Material * mat = scene->GetMaterial(index);


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
    
    
    
    RenderManager::Instance()->SetRenderEffect(RenderManager::TEXTURE_MUL_FLAT_COLOR);
	if (material)
	{

		if (material->diffuseTexture)
		{
			RenderManager::Instance()->SetTexture(material->diffuseTexture);
            RenderManager::Instance()->SetRenderData(group->renderDataObject);

        }
        
        if (material->hasOpacity)
        {
            //RenderManager::Instance()->EnableBlending(true);
            //RenderManager::Instance()->SetBlendMode(BLEND_ONE, BLEND_ONE_MINUS_SRC_ALPHA);
            glAlphaFunc ( GL_GREATER, 0.5 );
            glEnable ( GL_ALPHA_TEST ) ;
            glDisable( GL_CULL_FACE );
        }else
        {
            RenderManager::Instance()->EnableBlending(false);
        }
		
		//glEnable(GL_COLOR_MATERIAL);
#if 0
		//glDisable(GL_CULL_FACE);
		//glEnable(GL_COLOR_MATERIAL);
		//glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material->shininess);
		//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material->ambient.data);
		//glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material->diffuse.data);
		//glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material->specular.data);
		//glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material->emission.data);
		
		//glFrontFace(GL_CW);
		
		
		glMaterialf(GL_FRONT, GL_SHININESS, material->shininess);
		glMaterialfv(GL_FRONT, GL_AMBIENT, material->ambient.data);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, material->diffuse.data);
		glMaterialfv(GL_FRONT, GL_SPECULAR, material->specular.data);
		glMaterialfv(GL_FRONT, GL_EMISSION, material->emission.data);
		
		
		/*
		glMaterialf(GL_BACK, GL_SHININESS, material->shininess);
		glMaterialfv(GL_BACK, GL_AMBIENT, material->ambient.data);
		glMaterialfv(GL_BACK, GL_DIFFUSE, material->diffuse.data);
		glMaterialfv(GL_BACK, GL_SPECULAR, material->specular.data);
		glMaterialfv(GL_BACK, GL_EMISSION, material->emission.data);
		 */
		
		//glDisable(GL_COLOR_MATERIAL);
#endif
	}

	// render
	//RenderManager::Instance()->DrawArrays(GL_TRIANGLES, );
	//RenderManager::Instance()->FlushState();
	RenderManager::Instance()->DrawElements(PRIMITIVETYPE_TRIANGLELIST, group->indexCount, EIF_16, group->indexArray);
    
    
    if (material && material->hasOpacity)
    {
        glDisable ( GL_ALPHA_TEST ) ;
        glEnable(GL_CULL_FACE);
    }
#endif
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