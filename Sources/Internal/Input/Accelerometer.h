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
#ifndef __DAVAENGINE_ACCELEROMETER_H__
#define __DAVAENGINE_ACCELEROMETER_H__

#include "Base/BaseTypes.h"
#include "Base/BaseMath.h"
#include "Base/Singleton.h"
#include "Base/EventDispatcher.h"

namespace DAVA
{
	
#define DEFAULT_UPDATE_RATE 1.0f / 10.0f

/**
	\defgroup input Input System
 */
/**
	\ingroup input
	\brief Accelerometer access. Available only on devices that have accelerometer. 
 */
	
class Accelerometer : public Singleton<Accelerometer>
{
	IMPLEMENT_EVENT_DISPATCHER(eventDispatcher);
public:
	enum
	{
		EVENT_ACCELLEROMETER_DATA = 1,
	};
	
	
	Accelerometer();
	virtual ~Accelerometer();
	
	/*
	 Always return true for iPhone, but not supported by PSP & Nintendo DS
	 By default if nothing provided by platform layer this function return false
	 */
	virtual bool	IsSupportedByHW() {return false; };
	/*
	 
	*/
	virtual const Vector3 & GetAccelerationData() {return accelerationData; }

	virtual void Enable(float32 updateRate = DEFAULT_UPDATE_RATE){}
	virtual void Disable(){}

protected:	
	Vector3 accelerationData;
};
		
};

#endif // __DAVAENGINE_ACCELEROMETER_H__
