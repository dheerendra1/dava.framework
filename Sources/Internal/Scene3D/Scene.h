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

/**
    \defgroup scene3d 3D Engine
  */  
    
class Texture;
class Material;
class StaticMesh;
class AnimatedMesh;
class SceneNodeAnimationList;
	
/** 
    \ingroup scene3d
    \brief This class is a code of our 3D Engine scene graph. 
    To visualize any 3d scene you'll need to create Scene object. 
    Scene have visible hierarchy and invisible root nodes. You can add as many root nodes as you want, and do not visualize them.
    For example you can have multiple scenes, load them to one scene, and show each scene when it will be required. 
 
 
 */
class Scene : public SceneNode
{
public:	
    
    struct LodLayer
    {
        float32 nearDistance;
        float32 farDistance;

        float32 nearDistanceSq;
        float32 farDistanceSq;
    };
    
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
    
    
    /**
        \brief Function to add root node.
        \param[in] node node you want to add
        \param[in] rootNodePath path of this root node
     */

    void AddRootNode(SceneNode *node, const String &rootNodePath);

	/**
        \brief Get root node by path.
        This function can be used when you want to get a node and add it to real scene.  
        \code
        SceneNode * node = scene->GetRootNode("~res:/Scenes/level0.sce");
        scene->AddNode(node);
        \endcode
     */
    
    SceneNode *GetRootNode(const String &rootNodePath);
    
    /**
        \brief Release root node by name.
        \param[in] rootNodePath root node path you want to release.
     */
    void ReleaseRootNode(const String &rootNodePath);
    
    /**
        \brief Release root node by pointer to this node.
        \param[in] nodeToRelease root node pointer you want to release.
     */
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
    
    /**
        \brief Registers LOD layer into the scene.
        \param[in] nearDistance near view distance fro the layer
        \param[in] farDistance far view distance fro the layer
        \returns Serial number of the layer
	 */
    int32 RegisterLodLayer(float32 nearDistance, float32 farDistance);
    
    inline int32 GetLodLayersCount();
    inline float32 GetLodLayerNear(int32 layerNum);
    inline float32 GetLodLayerFar(int32 layerNum);
    inline float32 GetLodLayerNearSquare(int32 layerNum);
    inline float32 GetLodLayerFarSquare(int32 layerNum);

    
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

    // Vector<SceneNode*> alphaObjectQueue;
    
    Vector<LodLayer> lodLayers;

    
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

    
int32 Scene::GetLodLayersCount()
{
    return lodLayers.size();
}

float32 Scene::GetLodLayerNear(int32 layerNum)
{
    return lodLayers[layerNum].nearDistance;
}

float32 Scene::GetLodLayerFar(int32 layerNum)
{
    return lodLayers[layerNum].farDistance;
}

float32 Scene::GetLodLayerNearSquare(int32 layerNum)
{
    return lodLayers[layerNum].nearDistanceSq;
}

float32 Scene::GetLodLayerFarSquare(int32 layerNum)
{
    return lodLayers[layerNum].farDistanceSq;
}
    

};




#endif // __DAVAENGINE_SCENE_H__


