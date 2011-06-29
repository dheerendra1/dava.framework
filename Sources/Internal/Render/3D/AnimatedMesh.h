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
#ifndef __DAVAENGINE_ANIMATED_MESH_H__
#define __DAVAENGINE_ANIMATED_MESH_H__

#include "Base/BaseTypes.h"
#include "Base/BaseMath.h"
#include "Render/RenderBase.h"
#include "FileSystem/File.h"
#include "Render/3D/StaticMesh.h"

namespace DAVA
{

class Scene;
class SkeletonNode;
class BoneNode;
	
class VertexWeight
{
public:
	float32		weight[4];
	int32		boneIndex[4];
	int32		count;
};
	
class Joint
{
public:
	Vector3		position;
	Quaternion	orientation;
	float32				length;
};
	
class Bone
{
public:
	Joint					joint;
	int32					parentIndex;
	
	Matrix4			inverse0Transform;		// matrix 0^(-1) frame
	Matrix4			relativeTransform;		// matrix to set current frame
	Matrix4			absoluteTransform;
};
	
class BoneAnimationKey
{
public:
	int32 frame;
	Vector3	 translation;	
	Quaternion	orientation;
	Matrix4	matrix;
};
	
class BoneAnimation
{
public:
	BoneAnimationKey &	GetKey(int32 bone, int32 frame);
	const String &		GetName();
	float32				GetFPS();
	
	void				Load(File * file, int32 boneCount, int32 frameCount);
private:
	String						name;	//! name of bone animation
	float32						fps;	//! frames per second (speed) NOT USED YET
	int32						boneCount;
	int32						frameCount;
	BoneAnimationKey		**	keys;
};
	
/*
	TODO: Split animated mesh, to static & dynamic part
 */
	
//! Animated Mesh
//! Used to create skeletal & morph animations
class AnimatedMesh : public StaticMesh
{
public:	
	AnimatedMesh(Scene * _scene);
	~AnimatedMesh();
	
	void Load(const String & animationFilename, int32 additionalFlags = 0);
	void SetAnimation(const String & animationName);
	
	void Update(float32 timeElapsed);
	void Draw();
	
	Matrix4					bindShapeMatrix;		//
	float32					currentTime;		//	
	
	SkeletonNode			* skeleton;
	Vector<BoneNode*>		bones;
	Vector<String>			boneNames;
	
	void					RestoreBonesFromNames();
	
	
//	int32					boneCount;
//	int32					frameCount;
//
//	Vector<Bone>			boneArray;
//	Vector<BoneAnimation>	boneAnimationArray;
};

};

#endif // __DAVAENGINE_ANIMATEDMESH_H__

