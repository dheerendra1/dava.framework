#include "stdafx.h"
#include "ColladaAnimatedMesh.h"

namespace DAVA
{

ColladaAnimatedMesh::ColladaAnimatedMesh(FCDController * animationController )
{
	mesh = 0;
	controller = animationController;
	bool isMorph = animationController->IsMorph();
	bool isSkin = animationController->IsSkin();

	if (isSkin)
	{
		FCDSkinController * skinController = animationController->GetSkinController();
		int jointCount = skinController->GetJointCount();
		FCDSkinControllerJoint * origJoints = skinController->GetJoints();

		joints.resize(jointCount);
		for (int j = 0; j < jointCount;  ++j)
		{
			joints[j].joint = &origJoints[j];
			joints[j].parentJoint = 0;
			joints[j].node = 0;
			joints[j].index = j;
			joints[j].parentIndex = -1;
		
			FMMatrix44 bindPoseInverse = joints[j].joint->GetBindPoseInverse();
			joints[j].colladaInverse0 = bindPoseInverse;
			joints[j].inverse0 = ConvertMatrix(joints[j].colladaInverse0);
			
			printf("%s, ", joints[j].joint->GetId().c_str());
		}

		colladaBindShapeMatrix = skinController->GetBindShapeTransform();
		bindShapeMatrix = ConvertMatrix(colladaBindShapeMatrix);
		
		printf("- controller: %s influence: %d influence-entity: %s\n", animationController->GetDaeId().c_str(), skinController->GetInfluenceCount(), skinController->GetTarget()->GetDaeId().c_str());

		vertexWeights.resize(skinController->GetInfluenceCount());
		int maxJoints = 0;
		for (int index = 0; index < skinController->GetInfluenceCount(); ++index)
		{
			FCDSkinControllerVertex * vert = skinController->GetVertexInfluence(index);
			vertexWeights[index].jointCount = 0;
			for (int pp = 0; pp < vert->GetPairCount(); ++pp)
			{
				FCDJointWeightPair * jointWeightPar = vert->GetPair(pp);
				
				vertexWeights[index].AddWeight(jointWeightPar->jointIndex, jointWeightPar->weight);
			}
			if (vertexWeights[index].jointCount > maxJoints)
				maxJoints = vertexWeights[index].jointCount;

			vertexWeights[index].Normalize();
		}
		printf("- max joints: %d\n", maxJoints);
		
		mesh = new ColladaMesh( animationController->GetBaseGeometry()->GetMesh(), &(vertexWeights.front()));
	}
}
	
void PrintMatrix(Matrix4 & m, bool finishLine = true)
{
	for (int k = 0; k < 16; ++k)
		printf("%0.4f ", m.data[k]); 
	if (finishLine)
		printf("\n");
}		

void ColladaAnimatedMesh::UpdateSkinnedMesh( float32 time )
{
	if (mesh == 0)return;
	if (joints[0].node == 0)return;
	
//	for (int k = 0; k < (int)joints.size(); ++k)
//	{
//		if (joints[k].node)
//		{
//			joints[k].colladaLocalTransform = joints[k].node->originalNode->CalculateLocalTransform();
//			joints[k].colladaWorldTransform = joints[k].node->originalNode->CalculateWorldTransform();
//		}
//	}
	for (int k = 0; k < (int)joints.size(); ++k)
	{
		Joint & currentJoint = joints[k];
		if (joints[k].node)
		{
			//FMMatrix44 colladaLocalTransform = joints[k].node->originalNode->CalculateLocalTransform();
			//joints[k].localTransform = ConvertMatrix(colladaLocalTransform);
			joints[k].localTransform = joints[k].node->localTransform;
			// code to compute worldMatrixes using localMatrixes
//			FMMatrix44 colladaWorldTransform = joints[k].node->originalNode->CalculateWorldTransform();
//			Matrix4 worldTransform = ConvertMatrix(colladaWorldTransform);
//			if (currentJoint.parentIndex != -1)
//			{
//				Joint & parentJoint = joints[currentJoint.parentIndex];
//			
//				joints[k].worldTransform =  joints[k].localTransform * parentJoint.worldTransform;
//			}else
//			{
//				FMMatrix44 realWorldTransform = joints[k].node->originalNode->CalculateWorldTransform();
//				// get parent joint world transform
//				joints[k].worldTransform = ConvertMatrix(realWorldTransform); ////joints[k].localTransform;
//			}

			//printf("joint: %d: %s p:%d isEq: %d\n", k, currentJoint.node->originalNode->GetDaeId().c_str(), currentJoint.parentIndex, (int)(worldTransform == joints[k].worldTransform));
			//PrintMatrix(worldTransform);
			//PrintMatrix(joints[k].worldTransform);

		
//			joints[k].localTranslation.x = joints[k].localTransform._30;
//			joints[k].localTranslation.y = joints[k].localTransform._31;
//			joints[k].localTranslation.z = joints[k].localTransform._32;
//			
//			joints[k].localQuat.Construct(joints[k].localTransform);
//
//			
//			Matrix4 localTransformTrans;
//			Matrix4 localTransformRot;
//			Matrix4 localTransformFinal;
//			localTransformTrans.CreateTranslation(joints[k].localTranslation);
//			localTransformRot = joints[k].localQuat.GetMatrix();
//			
//			joints[k].localTransform = localTransformRot * localTransformTrans;
//			
//			FMMatrix44 colladaWorldTransform = joints[k].node->originalNode->CalculateWorldTransform();
//			Matrix4 worldTransform = ConvertMatrix(colladaWorldTransform);
			if (currentJoint.parentIndex != -1)
			{
				Joint & parentJoint = joints[currentJoint.parentIndex];
			
				joints[k].worldTransform =  joints[k].localTransform * parentJoint.worldTransform;
			}else
			{
				joints[k].worldTransform = joints[k].node->worldTransform; ////joints[k].localTransform;
			}
			
		}
	}
	
	for (int poly = 0; poly < mesh->polygons.size(); ++poly)
	{
		ColladaPolygonGroup * polyGroup = mesh->polygons[poly];	
		
		for (int vi = 0; vi < (int)polyGroup->skinVerteces.size(); ++vi)
		{
			ColladaVertex & v0 = polyGroup->unoptimizedVerteces[vi];
			ColladaVertex & v = polyGroup->skinVerteces[vi];
			
			v.position = Vector3(0.0, 0.0, 0.0);

			for (int jointIndex = 0; jointIndex < v.jointCount; ++jointIndex)
			{
				//FMVector3 pos(v0.position.x, v0.position.y, v0.position.z);
				//FMMatrix44 jointT = joints[v0.joint[jointIndex]].colladaWorldTransform * joints[v0.joint[jointIndex]].colladaInverse0 * bindShapeMatrix;
				//pos = jointT.TransformCoordinate(pos);
				
				Vector3 pos0(v0.position.x, v0.position.y, v0.position.z);
				Vector3 pos;
				Matrix4 finalJointMatrix = bindShapeMatrix * joints[v0.joint[jointIndex]].inverse0 * joints[v0.joint[jointIndex]].worldTransform;
//				Matrix4 finalJointMatrix = joints[v0.joint[jointIndex]].worldTransform * joints[v0.joint[jointIndex]].inverse0 * bindShapeMatrix; 
				pos = pos0 * finalJointMatrix;
				
				float weight = v0.weight[jointIndex]; 
				
				v.position.x += pos.x * weight;
				v.position.y += pos.y * weight;
				v.position.z += pos.z * weight;
			}
		}	
	}
}

void ColladaAnimatedMesh::MarkJoints(ColladaSceneNode * node)
{
	sceneRootNode = node;
	BuildJointsHierarhy(sceneRootNode, 0);
}

void ColladaAnimatedMesh::BuildJointsHierarhy( ColladaSceneNode * node, Joint * parentJoint)
{
	static int depth = 0;

	depth++;

	for (int d = 0; d < depth; ++d)
		printf("-");
	printf("%s %s\n", node->originalNode->GetDaeId().c_str(), node->originalNode->GetSubId().c_str());


	Joint * currentJoint = 0;
	for (int j = 0; j < (int)joints.size(); ++j)
	{
		if (node->originalNode->GetSubId() == joints[j].joint->GetId())
		{
			joints[j].node = node;
			if (parentJoint != 0)
				joints[j].parentIndex = parentJoint->index;

			node->inverse0 = joints[j].inverse0;	// copy bindpos inverse matrix to node

			node->MarkJoint();
			for (int d = 0; d < depth; ++d)
				printf("-");

			currentJoint = &joints[j];
			printf("Joint founded: %s i:%d p:%d\n", node->originalNode->GetDaeId().c_str(), currentJoint->index, currentJoint->parentIndex);
			break;
		}
	}

	for (int i = 0; i < (int)node->childs.size(); i++) 
	{
		ColladaSceneNode * childNode = node->childs[i];
		BuildJointsHierarhy(childNode, currentJoint);
	}
	depth--;
}



};
