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
#include "Render/3D/AnimatedMesh.h"
#include "Scene3D/Scene.h"
#include "Scene3D/BoneNode.h"

namespace DAVA 
{
    
BoneAnimation::BoneAnimation()
{
    boneCount = 0;
    keys = 0;
}

BoneAnimation::~BoneAnimation()
{
    for (int32 bi = 0; bi < boneCount; ++bi)
    {
        SafeDeleteArray(keys[bi]);
    }
    SafeDeleteArray(keys);
}

BoneAnimationKey &	BoneAnimation::GetKey(int32 bone, int32 frame)
{
	if (frame >= frameCount)frame = frameCount - 1;
	
	// \TODO: We suppose that every animation frame exported
	// so we have keys for every frame
	// Need to add interpolation beetween keys
	return keys[bone][frame];
}

/// not used
void BoneAnimation::Load(File * file, int32 _boneCount, int32 _frameCount)
{
	if (!file)return;
	boneCount = _boneCount;
	frameCount = _frameCount;
	
	keys = new BoneAnimationKey*[boneCount];
	for (int bi = 0; bi < boneCount; ++bi)
	{
		keys[bi] = new BoneAnimationKey[frameCount];
		
		for (int f = 0; f < frameCount; ++f)
		{
			keys[bi][f].frame = f;
			
			keys[bi][f].matrix.Identity();
			file->Read(&keys[bi][f].matrix, 4 * 4 * 3);
			keys[bi][f].matrix.Transpose();
			
			file->Read(&keys[bi][f].orientation.w, 4);
			file->Read(&keys[bi][f].orientation.x, 4);
			file->Read(&keys[bi][f].orientation.y, 4);
			file->Read(&keys[bi][f].orientation.z, 4);
			
			
			file->Read(&keys[bi][f].translation.x, 4);
			file->Read(&keys[bi][f].translation.y, 4);
			file->Read(&keys[bi][f].translation.z, 4);		
		}
	}
}

const String &		BoneAnimation::GetName()
{
	return name;
}

float32				BoneAnimation::GetFPS()
{
	return fps;
}

AnimatedMesh::AnimatedMesh(Scene * _scene)
	: StaticMesh(_scene)
{		
}

AnimatedMesh::~AnimatedMesh()
{

}
	
// structure for fast joint access
struct JointWeight
{
	int32 index[4];
	float32 weight[4];
};

	
inline void BlendedMul(Vector3 * res, const Vector3 & _v, const Matrix4 & _m, float32 w)
{
	res->x += (_v.x * _m._00 + _v.y * _m._10 + _v.z * _m._20 + _m._30) * w;
	res->y += (_v.x * _m._01 + _v.y * _m._11 + _v.z * _m._21 + _m._31) * w;
	res->z += (_v.x * _m._02 + _v.y * _m._12 + _v.z * _m._22 + _m._32) * w;
}
	

void AnimatedMesh::Update(float32 timeElapsed)
{
	
	//if (mesh == 0)return;
	//if (joints[0].node == 0)return;
	

	/* 
	for (int k = 0; k < (int)joints.size(); ++k)
	{
		Joint & currentJoint = joints[k];
		if (joints[k].node)
		{
			joints[k].localTransform = joints[k].node->localTransform;
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
	*/
	
	int32 boneCount = (int32)bones.size();
	for (int b = 0; b < boneCount; ++b)
	{
		bones[b]->finalMatrix = bindShapeMatrix * bones[b]->inverse0Matrix * bones[b]->GetWorldTransform();
	}
	
	int32 polygroupCount = (int32)polygroups.size();
	for (int32 poly = 0; poly < polygroupCount; ++poly)
	{
		PolygonGroup * polyGroup = polygroups[poly];	
		
		int32 vertexCount = polyGroup->GetVertexCount();
		
		uint8 * jointsWeightsPlain = (uint8*)polyGroup->jointIdxArray;
		uint8 * vertexPlain = (uint8*)polyGroup->vertexArray;
		Vector3 * v = polyGroup->baseVertexArray;
		int32 * jointCounts = polyGroup->jointCountArray;
		
		for (int32 vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
		{

//			JointWeight * jw = (JointWeight *)(jointsWeightsPlain);
			Vector3 * resultPos = (Vector3 *)(vertexPlain);
			resultPos->x = 0.0f;
			resultPos->y = 0.0f;
			resultPos->z = 0.0f;
			for (int jointIndex = *jointCounts - 1; jointIndex >= 0 ; --jointIndex)
			{
				
				Vector3 pos = *v * bones[((JointWeight *)(jointsWeightsPlain))->index[jointIndex]]->finalMatrix;
				float weight = ((JointWeight *)(jointsWeightsPlain))->weight[jointIndex]; 
				resultPos->x += pos.x * weight;
				resultPos->y += pos.y * weight;
				resultPos->z += pos.z * weight;
				 
//				Matrix4 & matrix = ;
//				float weight = ; 

//				BlendedMul(resultPos, *v, bones[jw->index[jointIndex]]->finalMatrix, jw->weight[jointIndex]);
			}
			
			v++;
			//resultPos->x = v.x;
			//resultPos->y = v.y;
			//resultPos->z = v.z;
			
			jointCounts++;

			jointsWeightsPlain += polyGroup->vertexStride;
			vertexPlain += polyGroup->vertexStride;
		}
	}
}

void AnimatedMesh::RestoreBonesFromNames()
{
	for (int32 b = 0; b < (int32)boneNames.size(); ++b)
	{
		BoneNode * bone = dynamic_cast<BoneNode*>(scene->FindByName(boneNames[b]));
		if (!bone)
		{
			printf("-- error: can't find bone node: %s", boneNames[b].c_str());
		}
		bones.push_back(bone);
	}	
}
	
void AnimatedMesh::Draw()
{
		
}


/*
void AnimatedMesh::Load(const String & animationFilename, int32 additionalFlags)
{
	// \TODO: error messages
	if (!fs)return;
	
	File * file = File::Create(animationFilename.c_str(), File::OPEN | IO::READ);
	if (!file)return;
	
	baseMesh.Load(file, additionalFlags);
	
	
	
	// Load skeletal data information
	
	// read weights
	vertexArray.reserve(baseMesh.GetVertexCount());
	weightArray.reserve(baseMesh.GetVertexCount());
	
	for (int v = 0; v < baseMesh.GetVertexCount(); ++v)
	{
		// base vertex pos
		baseMesh.GetCoord(v, vertexArray[v]);
		// weights
		for (int w = 0; w < 4; ++w)
		{
			int8 b;
			float32 wr;
			file->Read(&wr, 4);
			weightArray[v].weight[w] = wr;
			file->Read(&b, 1);
			weightArray[v].boneIndex[w] = b;
		}
	}
	
	// read bone info
	file->Read(&boneCount, 4);
	
	boneArray.reserve(boneCount);
	
	for (int bone = 0; bone < boneCount; ++bone)
	{
		int16 parentIndex;
		file->Read(&parentIndex, 2);
		boneArray[bone].parentIndex = parentIndex;
		
		// \TODO: check for incorrect bone assigments
		if (boneArray[bone].parentIndex >= bone)
		{
			Global::Log("[AnimatedMesh] bone assigment incorrect\n");
		}
	}
	
	file->Read(&frameCount, 4);
	
	boneAnimationArray.push_back(BoneAnimation());
	boneAnimationArray[0].Load(file, boneCount, frameCount);	
	
	currentAnimation = &boneAnimationArray[0];
	
	
	PrepareMesh0();
	currentFrame = 0;
	
	SafeRelease(file);
}
*/
//
//void AnimatedMesh::PrepareMesh0()
//{
//	if (!currentAnimation)return;
//	// find joints positions
//	for (int32 bi = 0; bi < boneCount; ++bi)
//	{
//		Bone & bone = boneArray[bi];
//		
//		BoneAnimationKey & key0 = currentAnimation->GetKey(bi, 0);
//		
//		Matrix4 base0matrix;
//		Matrix4 base0trans;
//		Matrix4 transform;
//		
//		// \TODO : Matrix functions to set angles & translations
//		// we use fact that R * T = R set T
//		
//		key0.orientation.GetMatrix(&base0matrix);
//		base0trans.CreateTranslation(key0.translation);
//        transform = base0matrix * base0trans;
//		
//		// transform relative matrix transform 
//		// using absolute parent transformation.
//		
//		if (bone.parentIndex != -1)
//			bone.inverse0Transform = transform * boneArray[bone.parentIndex].inverse0Transform;
//		else
//			bone.inverse0Transform = transform;
//		
//	}
//	// inverse only after full computation of 
//	// all matrixes (because matrixes is used as parent transformations)
//	for (int32 bi = 0; bi < boneCount; ++bi)
//	{
//		Bone & bone = boneArray[bi];
//		bone.absoluteTransform = bone.inverse0Transform;
//		bone.inverse0Transform.Inverse();
//	}
//}
//
//void AnimatedMesh::PrepareMesh()
//{
//	if (!currentAnimation)return;
//	// find joints positions
//	for (int32 bi = 0; bi < boneCount; ++bi)
//	{
//		Bone & bone = boneArray[bi];
//		
//		BoneAnimationKey & keyCurrent = currentAnimation->GetKey(bi, currentFrame);
//		
//		Matrix4 currentRotation;
//		Matrix4 currentTranslation;
//		
//		// \TODO : Matrix functions to set angles & translations
//		// we use fact that R * T = R set T
//		
//		keyCurrent.orientation.GetMatrix(&currentRotation);
//		currentTranslation.CreateTranslation(keyCurrent.translation);
//		bone.relativeTransform = currentRotation * currentTranslation;
//		
//		if (bone.parentIndex != -1)
//		{
//			// transform relative matrix transform 
//			// using absolute parent transformation.
//			
//			bone.absoluteTransform = bone.relativeTransform * boneArray[bone.parentIndex].absoluteTransform;
//		}else bone.absoluteTransform = bone.relativeTransform;
//	}
//}
//
//void AnimatedMesh::RenderMesh()
//{
//	currentFrame++;
//	if (currentFrame >= frameCount)
//		currentFrame = 0;
//	
//	PrepareMesh();
///*	for (int v = 0; v < baseMesh.GetVertexCount(); ++v)
//	{
//		Vector3 resultVert(0.0f, 0.0f, 0.0f);
//		for (int b = 0; b < 4; ++b)
//		{
//			int32 boneIndex = weightArray[v].boneIndex[b];
//			float32 weight = weightArray[v].weight[b];
//			if (boneIndex != -1)
//			{
//				Bone & bone = boneArray[boneIndex];
//				
//				Vector3 r1 = vertexArray[v] * bone.inverse0Transform;
//				Vector3 r2 = r1 * bone.absoluteTransform;
//
//				resultVert += weight * r2;
//			}
//		}
//
//		baseMesh.Coord(v, resultVert);
//	}
//*/
//	//! 
//	// IRenderSystem * rs = Global::GetRenderSystem();
//	// rs->RenderBaseMesh(&baseMesh);
//}

};

