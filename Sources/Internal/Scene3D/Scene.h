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
#ifndef __DAVAENGINE_SCENE_H__
#define __DAVAENGINE_SCENE_H__

#include "Base/BaseTypes.h"
#include "Base/BaseMath.h"
#include "Render/RenderBase.h"
#include "Scene3D/SceneNode.h"
#include "Scene3D/Camera.h"


namespace DAVA
{

class Texture;
class Material;
class StaticMesh;
class AnimatedMesh;
class SceneNodeAnimationList;
	
class Scene : public SceneNode
{
public:	
	Scene();
	~Scene();
	
	void		ClearScene();
	
	
	
	void AddTexture(Texture * texture);
	void RemoveTexture(Texture * texture);
	Texture * GetTexture(int32 index);
	inline int32	GetTextureCount();
	
	void AddMaterial(Material * material);
	void RemoveMaterial(Material * material);
	Material * GetMaterial(int32 index);
	inline int32	GetMaterialCount();
	
	void AddStaticMesh(StaticMesh * mesh);
	void RemoveStaticMesh(StaticMesh * mesh);
	StaticMesh * GetStaticMesh(int32 index);
	inline int32	GetStaticMeshCount();
    
	void AddAnimatedMesh(AnimatedMesh * mesh);
	void RemoveAnimatedMesh(AnimatedMesh * mesh);
	AnimatedMesh * GetAnimatedMesh(int32 index);
	inline int32	GetAnimatedMeshCount();
	
	void AddAnimation(SceneNodeAnimationList * animation);
	SceneNodeAnimationList * GetAnimation(int32 index);
	SceneNodeAnimationList * GetAnimation(const String & name);
	inline int32 GetAnimationCount();
    
    void AddRootNode(SceneNode *node, const String &rootNodePath);
	SceneNode *GetRootNode(const String &rootNodePath);
    void ReleaseRootNode(const String &rootNodePath);
    void ReleaseRootNode(SceneNode *nodeToRelease);

	
	virtual void StopAllAnimations(bool recursive = true);
	
	virtual void	Update(float timeElapsed);
	virtual void	Draw();
	
	
	virtual void	SetupTestLighting();
	
	Camera * GetCamera(int32 n);
	void AddCamera(Camera * c);
	inline int32	GetCameraCount();
    
    void SetCurrentCamera(Camera * camera);
    Camera * GetCurrentCamera() const;
    
    /* 
        This camera is used for clipping only. If you do not call this function GetClipCamera returns currentCamera. 
        You can use SetClipCamera function if you want to test frustum clipping, and view the scene from different angles.
     */
    void SetClipCamera(Camera * clipCamera);
    Camera * GetClipCamera() const;
    
private:	
    uint64 updateTime;
    uint64 drawTime;
    uint32 nodeCounter;

    
	Vector<Texture*> textures;
	Vector<StaticMesh*> staticMeshes;
	Vector<AnimatedMesh*> animatedMeshes;
	Vector<Material*> materials;
	Vector<Camera*> cameras;
	Vector<SceneNodeAnimationList*> animations;
    Map<String, SceneNode*> rootNodes;
    
    Camera * currentCamera;
    Camera * clipCamera;

    friend class SceneNode;
};

// Inline implementation
	
int32 Scene::GetAnimationCount()
{
    return (int32)animations.size();
}
int32 Scene::GetTextureCount()
{
    return (int32)textures.size();
}

int32 Scene::GetMaterialCount()
{
    return (int32)materials.size();
}

int32 Scene::GetStaticMeshCount()
{
    return (int32)staticMeshes.size();
}

int32 Scene::GetAnimatedMeshCount()
{
    return (int32)animatedMeshes.size();
}

int32 Scene::GetCameraCount()
{
    return (int32)cameras.size();
}
};



#endif // __DAVAENGINE_SCENE_H__


