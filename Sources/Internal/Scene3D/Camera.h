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
#ifndef __DAVA_CAMERA_H__
#define __DAVA_CAMERA_H__

#include "Base/BaseTypes.h"
#include "Base/BaseMath.h"
#include "Base/BaseObject.h"
#include "Scene3D/SceneNode3d.h"
#include "Scene3D/Frustum.h"

namespace DAVA
{
/**
    \brief this class is main class to perform camera transformations in our 3D engine.
 */
class Camera : public SceneNode
{
public:
	Camera(Scene * scene);
	virtual ~Camera();
	
	void Setup(float32 fovy, float32 aspect, float32 znear, float32 zfar, bool ortho = false);
	void SetFOV(float32 _fovy);
	
	// forms matrix
	//void LookAt(Vector3 position, Vector3 view, Vector3 up);
	
	/** apply camera transformations (projection, model-view matrices) */
	void Set();
	
	/** Restore camera transform that was give from file */
	void RestoreOriginalSceneTransform();
	
    /**
        \brief return current Field Of View of this camera
        \returns FOV for this camera
     */ 
    float32 GetFOV();
        
    /** 
        \brief return current aspect for this camera
     */
    float32 GetAspect();

    /**
        \brief return znear value for this camera
     */
    float32 GetZNear();
    
    /**
        \brief return zfar value for this camera
     */
    float32 GetZFar();
    
    void SetPosition(const Vector3 & position);
	void SetDirection(const Vector3 & direction);
	void SetTarget(const Vector3 & target);
	void SetUp(const Vector3 & up);
	void SetLeft(const Vector3 & left);
	
	Vector3 & GetTarget();
	Vector3 & GetPosition();
	Vector3 & GetDirection();   // camera forward direction
    Vector3 & GetUp();
    Vector3 & GetLeft();    // camera left direction
	
	void RebuildCameraFromValues();
	void ExtractCameraToValues();
	
    virtual SceneNode* Clone(SceneNode *dstNode = NULL);
    const Matrix4 &GetUniformProjModelMatrix();
    Vector2 GetOnScreenPosition(const Vector3 &forPoint);
//    virtual SceneNode* Clone();
    
    Frustum * GetFrustum() const;

    
    // mostly for debug purposes
    void Draw();

protected:
    enum
    {
        REQUIRE_REBUILD = 1,
        REQUIRE_REBUILD_MODEL = 1<<1,
        REQUIRE_REBUILD_PROJECTION = 1<<2,
        REQUIRE_REBUILD_UNIFORM_PROJ_MODEL = 1<<3
    };
    
    
//    virtual SceneNode* CopyDataTo(SceneNode *dstNode);

	float32 xmin, xmax, ymin, ymax, znear, zfar, aspect, fovy;
	bool ortho;
		
	
	Vector3 position;		//
	Vector3 target;		//	
	Vector3 up;
	Vector3 left;
    
    Vector3 direction;  // right now this variable updated only when you call GetDirection.
    
	Quaternion rotation;	// 
	Matrix4 cameraTransform;

    Matrix4 modelMatrix;
	Matrix4 projMatrix;
    Matrix4 uniformProjModelMatrix;

    uint32 flags;
    Frustum * currentFrustum;
	
	void ExtractValuesFromMatrix();
	void ConstructMatrixFromValues();
	void Recalc();

	void RecalcFrustum();
	void RecalcTransform();
    
	
	/** calls glFrustum for projection matrix */
	void ApplyFrustum();
	
	/** operates on model-view matrix */
	void ApplyTransform();
    
    
};

} // ns

#endif
