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
#ifndef __DAVAENGINE_CAMERA_H__
#define __DAVAENGINE_CAMERA_H__

#include "Base/BaseTypes.h"
#include "Base/BaseMath.h"

namespace DAVA
{

class CameraState
{
public:
	void	BuildFromAngles(const Vector3 & _position, const Vector3 & _angles);
	void	BuildFromLookAt(const Vector3 & _position, const Vector3 & _lookAt, const Vector3 & _up);
	void	BuildFromQuat(const Vector3 & _position, const Quaternion & orientation);
	void	BuildFromStateLerp(const CameraState & _camera1, const CameraState & _camera2, float32 _lerpCoeff);
	void	BuildFromLerp(const Vector3 & _pos0, const Quaternion & _quat0, const Vector3 & _pos1, const Quaternion & _quat1, float32 _lerpCoeff);
	
	Matrix4 			GetMatrix();


	Vector3		position;
	Quaternion	orientation;
};


//! Class to manage basic camera operations
//! \todo comment class Camera
class Camera
{
public:
	Camera();
	~Camera();

	void Update(float32 timeElapsed);
	void SetState(const CameraState & state);
	void MoveToState(const CameraState & nextState, float32 timeToMove);
	void Stop();

	inline bool IsMoving();


	inline Matrix4 & GetMatrix();
	inline Vector3 & GetPosition();
	inline Quaternion & GetOrientation();
	
	inline const Vector3 	& GetForward();
	inline const Vector3 	& GetUp();
	inline const Vector3 	& GetRight();


	//! Get next camera position 
	Vector3 GetMoveLeftPosition(float32 dist);
	Vector3 GetMoveRightPosition(float32 dist);
	Vector3 GetMoveForwardPosition(float32 dist);
	Vector3 GetMoveBackwardPosition(float32 dist);

	void SetPosition(const Vector3 & newPosition);

	//! Move camera	
	void MoveLeft(float32 dist);
	void MoveRight(float32 dist);
	void MoveForward(float32 dist);
	void MoveBackward(float32 dist);

	//! Rotate camera around custom axis (increments in grads)
	void Rotate(const Vector3 & rotationAxis, float32 angle);
	
private:
	void				UpdateCameraInternal();

	Vector3		forward;
	Vector3		up;
	Vector3		right;
	
	CameraState			currentState;
	CameraState			prevState;
	CameraState			nextState;
	float32				timeElapsed;
	float32				timeToMove;
	bool				isMoving;

	Matrix4		viewMatrix;
};


// Camera implementation

inline Matrix4 & Camera::GetMatrix()
{
	return viewMatrix;	
}

inline Vector3 & Camera::GetPosition()
{
	return currentState.position;
}

inline Quaternion & Camera::GetOrientation()
{
	return currentState.orientation;
}

inline const  Vector3 & Camera::GetForward()
{
	return forward;
}

inline const Vector3 & Camera::GetUp()
{
	return up;
}

inline const Vector3 & Camera::GetRight()
{
	return right;
}

inline bool Camera::IsMoving()
{
	return isMoving;
}

};

#endif
