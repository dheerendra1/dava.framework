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
#include "Scene3D/Camera.h"
#include "Render/RenderBase.h"
#include "Core/Core.h"
#include "Render/RenderManager.h"
#include "Scene3D/Scene.h"

namespace DAVA 
{

Camera::Camera(Scene * scene) : SceneNode(scene)
{
	localTransform.Identity();
	Setup(35.0f, 1.0f, 1.0f, 2500.f, false);
	up = Vector3(0.0f, 1.0f, 0.0f);
	left = Vector3(1.0f, 0.0f, 0.0f);
	flags = REQUIRE_REBUILD | REQUIRE_REBUILD_MODEL | REQUIRE_REBUILD_PROJECTION;
    
	cameraTransform.Identity();
    currentFrustum = new Frustum();
}
	
Camera::~Camera()
{
	SafeRelease(currentFrustum);
}
	
void Camera::RestoreOriginalSceneTransform()
{
	cameraTransform = localTransform;
	SceneNode * node = GetParent();
	while(node)
	{
		cameraTransform = node->localTransform * cameraTransform;
		node = node->GetParent();
	}
	worldTransform = cameraTransform;
	cameraTransform.Inverse();
	ExtractCameraToValues();
}

void Camera::SetFOV(float32 _fovy)
{
    Setup(_fovy, aspect, znear, zfar, ortho);
}
    
float32 Camera::GetFOV()
{
    return fovy;
}

float32 Camera::GetAspect()
{
    return aspect;
}

float32 Camera::GetZNear()
{
    return znear;
}

float32 Camera::GetZFar()
{
    return zfar;
}

void Camera::Setup(float32 fovy, float32 aspect, float32 znear, float32 zfar, bool ortho)
{
    flags |= REQUIRE_REBUILD_PROJECTION;
	this->fovy = fovy;
	this->aspect = aspect;
	this->znear = znear;
	this->zfar = zfar;
	this->ortho = ortho;
	
	this->znear = 1;
	this->zfar = 5000;
	Recalc();
}

void Camera::Recalc()
{
	ymax = znear * tanf(fovy* PI / 360.0f);
	ymin = -ymax;
	
	xmin = ymin * aspect;
	xmax = ymax * aspect;
}

Vector2 Camera::GetOnScreenPosition(const Vector3 &forPoint)
{
    Vector4 pv(forPoint);
    pv = pv * GetUniformProjModelMatrix();
    return Vector2((GetScreenWidth() * 0.5f) * (1.f + pv.y/pv.w)
                   , (GetScreenHeight() * 0.5f) * (1.f + pv.x/pv.w));
}

const Matrix4 &Camera::GetUniformProjModelMatrix()
{
    if (flags & REQUIRE_REBUILD)
    {
        RebuildCameraFromValues();
    }
    if (flags & REQUIRE_REBUILD_PROJECTION)
    {
        RecalcFrustum();
    }
    if (flags & REQUIRE_REBUILD_MODEL)
    {
        RecalcTransform();
    }
    if (flags & REQUIRE_REBUILD_UNIFORM_PROJ_MODEL)
    {
        uniformProjModelMatrix = modelMatrix * projMatrix;
        flags &= ~REQUIRE_REBUILD_UNIFORM_PROJ_MODEL;
    }
    
    return uniformProjModelMatrix;
}

void Camera::RecalcFrustum()
{
    flags &= ~REQUIRE_REBUILD_PROJECTION;
    flags |= REQUIRE_REBUILD_UNIFORM_PROJ_MODEL;
    if (!ortho) 
    {
        projMatrix.glFrustum(xmin, xmax, ymin, ymax, znear, zfar);
    }
    else
    {
        projMatrix.glOrtho(xmin, xmax, ymin, ymax, znear, zfar);
    }
}

void Camera::RecalcTransform()
{
    flags &= ~REQUIRE_REBUILD_MODEL;
    flags |= REQUIRE_REBUILD_UNIFORM_PROJ_MODEL;

//	Core::eScreenOrientation orientation = Core::Instance()->GetScreenOrientation();
	modelMatrix = Matrix4::IDENTITY;
    
	switch(Core::Instance()->GetScreenOrientation())
	{
		case Core::SCREEN_ORIENTATION_LANDSCAPE_LEFT:
                //glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
			modelMatrix.CreateRotation(Vector3(0.0f, 0.0f, 1.0f), DegToRad(-90.0f));
            break;
		case Core::SCREEN_ORIENTATION_LANDSCAPE_RIGHT:
                //glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
            modelMatrix.CreateRotation(Vector3(0.0f, 0.0f, 1.0f), DegToRad(90.0f));
			break;
        default:
            break;
	}
    modelMatrix = cameraTransform * modelMatrix;
}

    
void Camera::ApplyFrustum()
{
    if (flags & REQUIRE_REBUILD_PROJECTION)
    {
        RecalcFrustum();
    }

    RenderManager::Instance()->SetMatrix(RenderManager::MATRIX_PROJECTION, projMatrix);
    
    /*  Boroda: Matrix Extract Snippet
     
     float32 proj[16];
     glGetFloatv(GL_PROJECTION_MATRIX, proj);
     
     Matrix4 frustumMatrix;
     frustumMatrix.glFrustum(xmin, xmax, ymin, ymax, znear, zfar);
     glLoadMatrixf(frustumMatrix.data);
     
     for (int32 k = 0; k < 16; ++k)
     {
        printf("k:%d - %0.3f = %0.3f\n", k, proj[k], frustumMatrix.data[k]);
     }

     */
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//#ifdef __DAVAENGINE_IPHONE__
//	if (!ortho)
//	{
//		glFrustumf(xmin, xmax, ymin, ymax, znear, zfar);
//	}
//	else 
//	{
//		glOrthof(xmin, xmax, ymin, ymax, znear, zfar);
//	}
//#else
//	if (!ortho)
//	{
//		glFrustum(xmin, xmax, ymin, ymax, znear, zfar);        
//    }
//	else 
//	{
//		glOrtho(xmin, xmax, ymin, ymax, znear, zfar);
//	}
//#endif
}

void Camera::ApplyTransform()
{
    if (flags & REQUIRE_REBUILD_MODEL)
    {
        RecalcTransform();
    }
        //glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
    
    //RenderManager::Instance()->SetMatrix(RenderManager::MATRIX_MODELVIEW, Matrix4::IDENTITY);

	
	// Xpen poymesh chto eto napisano
	//glLoadMatrixf(localTransform.data);
	// Matrix4 m = worldTransform;
	// m.Inverse();
	// cameraTransform =
	
    // Correct code from boroda // commented during refactoring
    //glMultMatrixf(cameraTransform.data);
	RenderManager::Instance()->SetMatrix(RenderManager::MATRIX_MODELVIEW, modelMatrix);
}

void Camera::SetPosition(const Vector3 & _position)
{
	position = _position;
    flags |= REQUIRE_REBUILD;
}

void Camera::SetDirection(const Vector3 & _direction)
{
	target = position + _direction;
    flags |= REQUIRE_REBUILD;
}

void Camera::SetTarget(const Vector3 & _target)
{
	target = _target;
    flags |= REQUIRE_REBUILD;
}
	
Vector3 & Camera::GetTarget()
{
	return target;
}

Vector3 & Camera::GetPosition()
{
	return position;
}

Vector3 & Camera::GetDirection()
{
    direction = target - position;
    direction.Normalize();
    return direction;
}

void Camera::SetUp(const Vector3 & _up)
{
	up = _up;
    flags |= REQUIRE_REBUILD;
}
    
Vector3 & Camera::GetUp()
{
    return up;
}

void Camera::SetLeft(const Vector3 & _left)
{
	left = _left;
    flags |= REQUIRE_REBUILD;
}

Vector3 & Camera::GetLeft()
{
    return left;
}

void Camera::RebuildCameraFromValues()
{
//    Logger::Debug("camera rebuild: pos(%0.2f %0.2f %0.2f) target(%0.2f %0.2f %0.2f) up(%0.2f %0.2f %0.2f)",
//                  position.x, position.y, position.z, target.x, target.y, target.z, up.x, up.y, up.z);
    
    flags &= ~REQUIRE_REBUILD; 
    flags |= REQUIRE_REBUILD_MODEL;
	cameraTransform.BuildLookAtMatrixRH(position, target, up);
    
    // update left vector after rebuild
	left.x = cameraTransform._00;
	left.y = cameraTransform._10;
	left.z = cameraTransform._20;
}
	
void Camera::ExtractCameraToValues()
{
	position.x = worldTransform._30;
	position.y = worldTransform._31;
	position.z = worldTransform._32;
	left.x = cameraTransform._00;
	left.y = cameraTransform._10;
	left.z = cameraTransform._20;
	up.x = cameraTransform._01;
	up.y = cameraTransform._11;
	up.z = cameraTransform._21;
	target.x = position.x - cameraTransform._02;
	target.y = position.y - cameraTransform._12;
	target.z = position.z - cameraTransform._22;
}


/*
void Camera::LookAt(Vector3	position, Vector3 view, Vector3 up)
{
	// compute matrix
	localTransform.BuildLookAtMatrixLH(position, view, up);
}
 */

void Camera::Set()
{
    if (flags & REQUIRE_REBUILD)
    {
        RebuildCameraFromValues();
    }
	ApplyFrustum();
	ApplyTransform();
    
    if (currentFrustum)
    {
        currentFrustum->Set();
    }
}

SceneNode* Camera::Clone(SceneNode *dstNode)
{
    if (!dstNode) 
    {
        dstNode = new Camera(scene);
    }
    SceneNode::Clone(dstNode);
    Camera *cnd = (Camera*)dstNode;
    cnd->xmin = xmin;
    cnd->xmax = xmax;
    cnd->ymin = ymin;
    cnd->ymax = ymax;
    cnd->znear = znear;
    cnd->zfar = zfar;
    cnd->aspect = aspect;
    cnd->fovy = fovy;
    cnd->ortho = ortho;
    
    cnd->position = position;
    cnd->target = target;
    cnd->up = up;
    cnd->left = left;
    cnd->rotation = rotation;
    cnd->cameraTransform = cameraTransform;
    cnd->flags = flags;
    return dstNode;
}
    
Frustum * Camera::GetFrustum() const
{
    return currentFrustum;
}
    
void Camera::Draw()
{
    if (debugFlags & DEBUG_DRAW_ALL)
    {
        Camera * prevCamera = scene->GetCurrentCamera();

        // Build this camera matrixes & it's frustum
        this->Set();
        
        // Restore original camera
        prevCamera->Set();
        
        RenderManager::Instance()->SetColor(0.0f, 1.0f, 0.0f, 1.0f);
        
        // If this is clip camera - show it as red camera
        if (this == scene->GetClipCamera())
            RenderManager::Instance()->SetColor(1.0f, 0.0f, 0.0f, 1.0f);

        // Draw frustum of this camera
        if (currentFrustum)
        {
            currentFrustum->DebugDraw();
        }
        // reset color
        RenderManager::Instance()->ResetColor();
    }
}

//SceneNode* Camera::Clone()
//{
//    return CopyDataTo(new Camera(scene));
//}
//
//SceneNode* Camera::CopyDataTo(SceneNode *dstNode)
//{
//    SceneNode::CopyDataTo(dstNode);
//    dstNode->xmin = xmin;
//    dstNode->xmax = xmax;
//    dstNode->ymin = ymin;
//    dstNode->ymax = ymax;
//    dstNode->znear = znear;
//    dstNode->zfar = zfar;
//    dstNode->aspect = aspect;
//    dstNode->fovy = fovy;
//	dstNode->ortho = ortho;
//    
//	dstNode->position = position;
//	dstNode->target = target;
//	dstNode->up = up;
//	dstNode->left = left;
//	dstNode->rotation = rotation;
//	dstNode->cameraTransform = cameraTransform;
//    return dstNode;
//}


	
} // ns



