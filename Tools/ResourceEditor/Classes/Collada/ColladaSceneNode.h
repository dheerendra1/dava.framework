#ifndef __COLLADALOADER_COLLADASCENENODE_H__
#define __COLLADALOADER_COLLADASCENENODE_H__

#include "ColladaIncludes.h"
#include "ColladaMeshInstance.h"
#include "ColladaLight.h"
#include "ColladaCamera.h"

#include "Scene3D/SceneNodeAnimation.h"

namespace DAVA
{

class ColladaSceneNode
{
public:
	ColladaSceneNode(ColladaScene * scene, FCDSceneNode * _node);
	~ColladaSceneNode();
	
	static SceneNodeAnimationKey ExportAnimationKey(FCDSceneNode * originalNode, float32 t);
	static SceneNodeAnimation * ExportNodeAnimation(FCDSceneNode * originalNode, float32 startTime, float32 endTime, float32 fps);
	static FMMatrix44 CalculateTransformForTime(FCDSceneNode * originalNode, float32 time);
	static bool IsAnimated(FCDSceneNode * originalNode);
	
	ColladaSceneNode * FindNode(const fstring & daeId);
	void SetAnimation(SceneNodeAnimation * sceneNodeAnimation, bool recursive = false);
	

	void AddNode(ColladaSceneNode * node);
	void AddMeshInstance(ColladaMeshInstance * mesh);
	void AddLight(ColladaLight * light);
	void AddCamera(ColladaCamera * cam);

	// skeletal animation functions & variables
	void MarkJoint();
	bool isJoint;
	Matrix4 inverse0;

	// recursive light setup function
	void PreProcessLights(ColladaLightState & state);

	// recursive render scene function
	void Render(Matrix4 currentMatrix);

	void UpdateTransforms(float t);
		
	Matrix4							localTransform; 
	Matrix4							worldTransform;

	std::vector<ColladaSceneNode*>	childs;	
	std::vector<ColladaMeshInstance *> meshInstances;
	std::vector<ColladaLight *> lights;
	std::vector<ColladaCamera *> cameras;

	FCDSceneNode					* originalNode;
	
	ColladaScene * scene;
	ColladaSceneNode * parent;
	
	SceneNodeAnimation * animation;
	
	// id written to .sce file 
	int32 writtenNodeId;
};

};

#endif // __COLLADALOADER_COLLADASCENENODE_H__


