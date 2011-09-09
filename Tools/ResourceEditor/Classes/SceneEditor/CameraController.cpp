/*
    Copyright (c) 2008, DAVA Consulting, LLC
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 
    * Created by Vitaliy Borodovsky
*/

#include "CameraController.h"

namespace DAVA 
{

CameraController::CameraController()
    : camera(0)
{

}
CameraController::~CameraController()
{
    SafeRelease(camera);
}

void CameraController::SetCamera(Camera * _camera)
{
    SafeRelease(camera);
    camera = SafeRetain(_camera);
}

void CameraController::Input(UIEvent * event)
{
    
}
    
WASDCameraController::WASDCameraController(float32 _speed)
    : speed(_speed)
    , viewXAngle(0)
    , viewYAngle(0)
{

}
void WASDCameraController::SetSpeed(float32 _speed)
{
    speed = _speed;
}

WASDCameraController::~WASDCameraController()
{
    
}
    
void WASDCameraController::Input(UIEvent * event)
{
    if (!camera)return;
    if (event->phase == UIEvent::PHASE_KEYCHAR)
    {   
        switch(event->keyChar)
        {
            case 'w':
            case 'W':
            {
                Vector3 pos = camera->GetPosition();
                Vector3 direction = camera->GetDirection();
                //Logger::Debug("oldpos: %f %f %f olddir: %f %f %f", pos.x, pos.y, pos.z, direction.x, direction.y, direction.z);

                direction.Normalize();
                pos += direction * speed * SystemTimer::Instance()->FrameDelta();
                camera->SetPosition(pos);
                camera->SetDirection(direction);    // right now required because camera rebuild direction to target, and if position & target is equal after set position it produce wrong results
                
                //Logger::Debug("newpos: %f %f %f", pos.x, pos.y, pos.z);
            }
            break;
            
            case 'a':
            case 'A':
            { 
                Vector3 pos = camera->GetPosition();
                Vector3 dir = camera->GetDirection();
                Vector3 left = camera->GetLeft();
                
                pos -= left * speed * SystemTimer::Instance()->FrameDelta();
                camera->SetPosition(pos);
                camera->SetDirection(dir);
            }
            break;
                
            case 's':
            case 'S':
            {
                Vector3 pos = camera->GetPosition();
                Vector3 direction = camera->GetDirection();
                //Logger::Debug("oldpos: %f %f %f olddir: %f %f %f", pos.x, pos.y, pos.z, direction.x, direction.y, direction.z);

                pos -= direction * speed * SystemTimer::Instance()->FrameDelta();
                camera->SetPosition(pos);
                camera->SetDirection(direction);    // right now required because camera rebuild direction to target, and if position & target is equal after set position it produce wrong results
                //Logger::Debug("newpos: %f %f %f olddir: %f %f %f", pos.x, pos.y, pos.z, direction.x, direction.y, direction.z);
            }
            break;
            
            case 'd':
            case 'D':
            {
                Vector3 pos = camera->GetPosition();
                Vector3 dir = camera->GetDirection();
                Vector3 left = camera->GetLeft();
                
                pos += left * speed * SystemTimer::Instance()->FrameDelta();
                camera->SetPosition(pos);
                camera->SetDirection(dir);
            }
            break;
        };
    } 
    
    if (event->phase == UIEvent::PHASE_BEGAN) 
    {
        oldTouchPoint = event->point;
    }
    else if(event->phase == UIEvent::PHASE_DRAG || event->phase == UIEvent::PHASE_ENDED)
    {
        Vector2 dp = oldTouchPoint - event->point;
        viewXAngle += dp.x * SystemTimer::Instance()->FrameDelta();
        viewYAngle += dp.y * SystemTimer::Instance()->FrameDelta();
        oldTouchPoint = event->point;
        //ClampAngles();
        //LOG_AS_FLOAT(viewXAngle);
        //LOG_AS_FLOAT(viewYAngle);
        Matrix4 aimUser;
        aimUser.Identity();
        Matrix4 mt, mt2;
        mt.CreateTranslation(Vector3(0,10,0));
        aimUser *= mt;
        mt.CreateRotation(Vector3(0,0,1), DegToRad(viewXAngle));
        mt2.CreateRotation(Vector3(1,0,0), DegToRad(viewYAngle));
        mt2 *= mt;
        aimUser *= mt2;
        
        Vector3 dir = Vector3() * aimUser;
        camera->SetDirection(dir);
    }
}
    
    
};
