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
#ifndef __DAVAENGINE_MUTEX_H__
#define __DAVAENGINE_MUTEX_H__

#include "Base/BaseTypes.h"
#include "Base/BaseObject.h"

namespace DAVA
{
/**
	\ingroup threads
	\brief wrapper mutex class compatible with Thread class. Now is supports Win32, MacOS, iPhone platforms.
	
	This class works primarily as POSIX pthread mutex. 
	Main difference that on Win32 it allows recursive locks from one thread. On Unix it doesn't because of it's mutex implementation.
	So be careful and try to avoid recursive locks because in this case code will not be portable. 
*/
class Mutex : public BaseObject
{
public:
	Mutex();
	virtual ~Mutex();

	/**
		\brief lock the execution thread of this mutex object

		If the mutex is already locked, the calling thread shall block until the mutex becomes available. 
		This operation returns when all references of this mutex will be unlocked. 
	*/
	virtual void Lock();

	/**
		\brief release the mutex object.
	*/
	virtual void Unlock();

#if defined(__DAVAENGINE_WIN32__)
	CRITICAL_SECTION criticalSection;
#elif defined(__DAVAENGINE_IPHONE__) || defined(__DAVAENGINE_MACOS__)
	pthread_mutex_t mutex;
#endif 
};

};

#endif // __DAVAENGINE_MUTEX_H__