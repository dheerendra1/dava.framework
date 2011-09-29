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
#include "Scene3D/Scene.h"

#include "Render/Texture.h"
#include "Render/Material.h"
#include "Render/3D/StaticMesh.h"
#include "Render/3D/AnimatedMesh.h"
#include "Scene3D/SceneNodeAnimationList.h"
#include "Scene3D/SceneFile.h"

namespace DAVA 
{
Scene::Scene()
	:   SceneNode(0)
    ,   currentCamera(0)
    ,   clipCamera(0)
{
}

Scene::~Scene()
{
    
	for (Map<String, SceneNode*>::iterator t = rootNodes.begin(); t != rootNodes.end(); ++t)
	{
		SceneNode * obj = t->second;
		obj->Release();
	}
	rootNodes.clear();
    
	for (Vector<Texture*>::iterator t = textures.begin(); t != textures.end(); ++t)
	{
		Texture * obj = *t;
		obj->Release();
	}
	textures.clear();

	for (Vector<Material*>::iterator t = materials.begin(); t != materials.end(); ++t)
	{
		Material * obj = *t;
		obj->Release();
	}
	materials.clear();

	for (Vector<StaticMesh*>::iterator t = staticMeshes.begin(); t != staticMeshes.end(); ++t)
	{
		StaticMesh * obj = *t;
		obj->Release();
	}
	staticMeshes.clear();

	for (Vector<AnimatedMesh*>::iterator t = animatedMeshes.begin(); t != animatedMeshes.end(); ++t)
	{
		AnimatedMesh * obj = *t;
		obj->Release();
	}
	animatedMeshes.clear();
	
	for (Vector<Camera*>::iterator t = cameras.begin(); t != cameras.end(); ++t)
	{
		Camera * obj = *t;
		obj->Release();
	}
	cameras.clear();
    
    SafeRelease(currentCamera);
    SafeRelease(clipCamera);
}

// 
void Scene::AddTexture(Texture * texture)
{
	if (texture)
	{
		texture->Retain();
		textures.push_back(texture);
	}
};

	
void Scene::RemoveTexture(Texture * texture)
{
	for (Vector<Texture*>::iterator t = textures.begin(); t != textures.end(); ++t)
	{
		if (*t == texture)
		{
			texture->Release();
			textures.erase(t);
			break;
		}
	}		
};
	
Texture * Scene::GetTexture(int32 index)
{
	if (index >= (int32)textures.size())
		return NULL;
	
	return textures[index];
}

void Scene::AddMaterial(Material * material)
{
	if (material)
	{
		material->Retain();
		materials.push_back(material);
	}
}

void Scene::RemoveMaterial(Material * material)
{
}

Material * Scene::GetMaterial(int32 index)
{
	return materials[index];
}

void Scene::AddStaticMesh(StaticMesh * mesh)
{
	if (mesh)
	{
		mesh->Retain();
		staticMeshes.push_back(mesh);
	}
}
	
void Scene::RemoveStaticMesh(StaticMesh * mesh)
{
}

StaticMesh * Scene::GetStaticMesh(int32 index)
{
	return staticMeshes[index];
}
	
void Scene::AddAnimatedMesh(AnimatedMesh * mesh)
{
	if (mesh)
	{
		mesh->Retain();
		animatedMeshes.push_back(mesh);
	}	
}

void Scene::RemoveAnimatedMesh(AnimatedMesh * mesh)
{
	
}

AnimatedMesh * Scene::GetAnimatedMesh(int32 index)
{
	return animatedMeshes[index];
}
	
void Scene::AddAnimation(SceneNodeAnimationList * animation)
{
	if (animation)
	{
		animation->Retain();
		animations.push_back(animation);
	}
}

SceneNodeAnimationList * Scene::GetAnimation(int32 index)
{
	return animations[index];
}
	
SceneNodeAnimationList * Scene::GetAnimation(const String & name)
{
	int32 size = (int32)animations.size();
	for (int32 k = 0; k < size; ++k)
	{
		SceneNodeAnimationList * node = animations[k];
		if (node->name == name)
			return node;
	}
	return 0;
}
	
	
	
void Scene::AddCamera(Camera * camera)
{
	if (camera)
	{
		camera->Retain();
		cameras.push_back(camera);
	}
}

Camera * Scene::GetCamera(int32 n)
{
	if (n >= 0 && n < (int32)cameras.size())
		return cameras[n];
	
	return NULL;
}


void Scene::AddRootNode(SceneNode *node, const String &rootNodePath)
{
    rootNodes[rootNodePath] = SafeRetain(node);
}

SceneNode *Scene::GetRootNode(const String &rootNodePath)
{
	Map<String, SceneNode*>::const_iterator it;
	it = rootNodes.find(rootNodePath);
	if (it != rootNodes.end())
	{
		return it->second;
	}
	SceneFile * file = new SceneFile();
	file->SetDebugLog(true);
	file->LoadScene(rootNodePath.c_str(), this);
	SafeRelease(file);

	it = rootNodes.find(rootNodePath);
	if (it != rootNodes.end())
	{
		return it->second;
	}
    return rootNodes[rootNodePath];
}

void Scene::SetupTestLighting()
{
#ifdef __DAVAENGINE_IPHONE__
//	glShadeModel(GL_SMOOTH);
//	// enable lighting
//	glEnable(GL_LIGHTING);
//	glEnable(GL_NORMALIZE);
//	
//	// deactivate all lights
//	for (int i=0; i<8; i++)  glDisable(GL_LIGHT0 + i);
//	
//	// ambiental light to nothing
//	GLfloat ambientalLight[]= {0.2f, 0.2f, 0.2f, 1.0f};
//	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientalLight);
//	
////	GLfloat light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };  // delete
//	//GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
//	GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
//	
//	GLfloat light_diffuse[4];
//	light_diffuse[0]=1.0f;
//	light_diffuse[1]=1.0f;
//	light_diffuse[2]=1.0f;
//	light_diffuse[3]=1.0f;
//	
//	GLfloat lightPos[] = { 0.0f, 0.0f, 1.0f, 0.0f };
//	
//	// activate this light
//	glEnable(GL_LIGHT0);
//	
//	//always position 0,0,0 because light  is moved with transformations
//	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
//	
//	// colors 
//	glLightfv(GL_LIGHT0, GL_AMBIENT, light_diffuse); // now like diffuse color
//	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
//	glLightfv(GL_LIGHT0, GL_SPECULAR,light_specular);
//	
//	//specific values for this light
//	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1);
//	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0);
//	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0);
//	
//	//other values
//	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 30.0f);
//	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 0.0f);
//	GLfloat spotdirection[] = { 0.0f, 0.0f, -1.0f, 0.0f }; // irrelevant for this light (I guess)
//	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotdirection); 
#endif
}

void Scene::Update(float timeElapsed)
{
	int32 size = (int32)animations.size();
	for (int32 animationIndex = 0; animationIndex < size; ++animationIndex)
	{
		SceneNodeAnimationList * anim = animations[animationIndex];
		anim->Update(timeElapsed);
	}
	
	SceneNode::Update(timeElapsed);
	
	size = (int32)animatedMeshes.size();
	for (int32 animatedMeshIndex = 0; animatedMeshIndex < size; ++animatedMeshIndex)
	{
		AnimatedMesh * mesh = animatedMeshes[animatedMeshIndex];
		mesh->Update(timeElapsed);
	}
}		

void Scene::Draw()
{
    //currentCamera->GetFrustum()->DebugDraw();
    
	glCullFace(GL_FRONT);
	SetupTestLighting();

    if (currentCamera)
    {
        currentCamera->Set();
    }
	SceneNode::Draw();
	
    glCullFace(GL_FRONT_AND_BACK);
	
//	for (int k = 0; k < staticMeshes.size(); ++k)
//	{
//		staticMeshes[k]->Draw();
//	}
}

	
void Scene::StopAllAnimations(bool recursive )
{
	int32 size = (int32)animations.size();
	for (int32 animationIndex = 0; animationIndex < size; ++animationIndex)
	{
		SceneNodeAnimationList * anim = animations[animationIndex];
		anim->StopAnimation();
	}
	SceneNode::StopAllAnimations(recursive);
}
    
    
void Scene::SetCurrentCamera(Camera * _camera)
{
    SafeRelease(currentCamera);
    currentCamera = SafeRetain(_camera);
    SafeRelease(clipCamera);
    clipCamera = SafeRetain(_camera);
}

Camera * Scene::GetCurrentCamera() const
{
    return currentCamera;
}

void Scene::SetClipCamera(Camera * _camera)
{
    SafeRelease(clipCamera);
    clipCamera = SafeRetain(_camera);
}

Camera * Scene::GetClipCamera() const
{
    return clipCamera;
}

};




