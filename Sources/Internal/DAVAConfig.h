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

/*
	__DAVAENGINE_IPHONE__ this define must be set in preprocessor macros for all projects that compiled using DAVAEngine for iPhone
 */

#ifndef __DAVAENGINE_CONFIG_H__
#define __DAVAENGINE_CONFIG_H__

//#define ENABLE_MEMORY_MANAGER
//#define CHECK_MEMORY_OVERRUNS 1 // number of 4 byte blocks used to check memory overrun

//#define ENABLE_BASE_OBJECT_CHECKS // separate thing to check if you release BaseObjects properly. Need to be disabled for release configurations 

//#define ENABLE_CONTROL_EDIT //allows to drug'n'drop controls for position editing

//#define SHOW_FRAME_TIME	// shows milliseconds per fame

#define __DAVAENGINE_RENDER_AUTOCONFIG__	// it will use DAVANENGINE_OPENGL for MacOS / iPhone, and 
//#define __DAVAENGINE_DIRECTX9__
//#define __DAVAENGINE_OPENGL__

#endif // __DAVAENGINE_CONFIG_H__

