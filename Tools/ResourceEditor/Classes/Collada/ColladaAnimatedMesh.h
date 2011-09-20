#ifndef __COLLADALOADER_COLLADAANIMATEDMESH_H__
#define __COLLADALOADER_COLLADAANIMATEDMESH_H__

#include "ColladaIncludes.h"
#include "ColladaSceneNode.h"
#include "ColladaMesh.h"

namespace DAVA
{

class ColladaAnimatedMesh
{
public:
	struct Joint
	{
		FCDSkinControllerJoint	* joint;
		FCDSkinControllerJoint	* parentJoint;
		ColladaSceneNode		* node;
		int						index;
		int						parentIndex;

		// original collada matrices
		FMMatrix44				colladaInverse0;
		FMMatrix44				colladaLocalTransform;
		FMMatrix44				colladaWorldTransform;

		Matrix4	inverse0;
		Matrix4	localTransform;
		Matrix4	worldTransform;
		
		Quaternion	localQuat;
		Vector3		localTranslation;
	};


	ColladaAnimatedMesh(FCDController * animationController);
	~ColladaAnimatedMesh();

	void MarkJoints(ColladaSceneNode * node);
	void UpdateSkinnedMesh(float time);

	std::vector<Joint>						joints;
	std::vector<ColladaVertexWeight>		vertexWeights;
	Matrix4									bindShapeMatrix;
	FMMatrix44								colladaBindShapeMatrix;

	ColladaSceneNode	* sceneRootNode;
	FCDController		* controller;

	ColladaMesh			* mesh; 
private:
	void BuildJointsHierarhy(ColladaSceneNode * node, Joint * parentJoint);
};

};

#endif // __COLLADALOADER_COLLADACAMERA_H__




