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
    This class creates standard matrix-based view-directional camera. Right now engine do not have quaternion camera, and
    you can always add it support if it will be required.
 */
class Camera : public SceneNode
{
public:
	Camera(Scene * scene);
	virtual ~Camera();
	
    /**
        \brief Setup camera with basic camera params
        This function set all parameters for the camera. All these parameters will be applied in case if camera Set function will be called. 
     
        \param[in] fovyInDegrees horizontal FOV of camera in degrees
        \param[in] aspectYdivX proportion between y and x. So if you want to setup camera manually pass y / x
        \param[in] zNear near clipping distance of camera
        \param[in] zFar far clipping distance of camera
        \param[in] isOrtho is camera will be with orthographic projection or perspective. By default it's false so camera will be with perspective projection.
     */
	void Setup(float32 fovyInDegrees, float32 aspectYdivX, float32 zNear, float32 zFar, bool isOrtho = false);
    /**
        \brief Function change fov in camera.
        You can use this function in many cases. For example you can use it when you want to change zoom of camera in your game. 
        
        \param[in] fovInDegrees new for in degrees for the camera
     */ 
    
	void SetFOV(float32 fovyInDegrees);
    /**
        \brief Set camera aspect ratio 
        \param[in] aspectYdivX Aspect ratio is viewport height / viewport width
     */
	void SetAspect(float32 aspectYdivX);
    
	/** 
        \brief Function applies camera transformations (projection, model-view matrices) to RenderManager
        This function normally is called internally from Scene class. In most cases you'll not need it. 
     */
	void Set();
	
	/**     
        \brief Restore camera transform to original camera transform that was set using 
     */
	void RestoreOriginalSceneTransform();
	
    /**
        \brief return current Field Of View of this camera
        \returns FOV for this camera
     */ 
    float32 GetFOV() const;
        
    /** 
        \brief return current aspect for this camera
        \returns current aspect ratio for the camera
     */
    float32 GetAspect() const;

    /**
        \brief return znear value for this camera
        \returns current znear value
     */
    float32 GetZNear() const;
    
    /**
        \brief return zfar value for this camera
        \returns current zfar value
     */
    float32 GetZFar() const;
    
    /**
        \brief Function change camera position.
        \param[in] position new camera position
     */
    void SetPosition(const Vector3 & position);
    /**
        \brief Function change camera direction.
        Be carefull with this function. It changing target of the camera because in calculations we use pair: position, target. 
        \param[in] direction new camera direction
     */
	void SetDirection(const Vector3 & direction);
    
    /**
        \brief Function to change camera target
        \param[in] target new camera target
     */
	void SetTarget(const Vector3 & target);
    
    /**
        \brief Function to change camera up vector
        \param[in] up new camera up vector
     */
	void SetUp(const Vector3 & up);
    
    /**
        \brief Function to change left camera vector
        \param[in] left new camera left vector
     */
	void SetLeft(const Vector3 & left);
    
	/**
        \brief Function returns current camera target vector
        Be carefull if you using SetDirection, target will not give you a vector that will make any sense. 
        It'll be just a point that located on (position, direction) ray distant on 1 from position.
        \returns target vector
     */
	const Vector3 & GetTarget() const;
    /**
        \brief Function returns position of camera
        \returns current position
     */
	const Vector3 & GetPosition() const;
    /**
        \brief Function returns normalized direction of camera
        \returns current normalized direction
     */
	const Vector3 & GetDirection();   // camera forward direction
    /**
        \brief Function returns current normalized up vector of camera
        \returns current normalized up vector
     */
    const Vector3 & GetUp() const;
    /**
        \brief Function returns current normalized left vector of camera
        \returns current normalized left vector
     */
    const Vector3 & GetLeft() const;    // camera left direction
    /**
        \brief Function returns current camera matrix
        This function returns camera matrix without multiplication of this matrix to viewport rotation matrix according to RenderManager and Core Interface Orientation.
        This matrix is right matrix that should be used in all 3D computations, that depends from camera, but final multiplication should be done to model view matrix.
        \returns current camera matrix
     */
    const Matrix4 & GetMatrix() const;  
	
    /**
        \brief Rebuild camera from all values that set inside it.
     */
	void RebuildCameraFromValues();

    /**
        \brief Extract values from matrices to all vectors
        This function is called when you load camera from file, using camera matrix
     */
	void ExtractCameraToValues();
	
    /**
        \brief Clone current camera
        
     */
    virtual SceneNode* Clone(SceneNode *dstNode = NULL);
    
    /**
        \brief Get project * camera matrix
        \returns matrix 
     */
    const Matrix4 &GetUniformProjModelMatrix();
    
    /**
        \brief Function to return 2D position of 3D point that is transformed to screen. 
        \returns 2D point on screen.
     */
    Vector2 GetOnScreenPosition(const Vector3 & forPoint);

    /**
        \brief Get frustum object for this camera.
        This function is widely used everywhere where you need object clipping.
     
        \returns pointer to frustum object
     */
    Frustum * GetFrustum() const;
    
    /**
        \brief Get camera zoom factor. 
        You can use zoom factor to have dependencies between camera zoom and visualisation of object
        \returns tanf(fov / 2). 
     */
    float32 GetZoomFactor() const;

    
    /**
        \brief Draw debug camera information if debug flags enabled
     */
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
    
    
    void CalculateZoomFactor();
    
    float32 zoomFactor;
    
};

} // ns

#endif
