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
#ifndef __DAVAENGINE_D3D9HELPERS_H__
#define __DAVAENGINE_D3D9HELPERS_H__

#include "Render/RenderBase.h"

#if defined(__DAVAENGINE_DIRECTX9__)
namespace DAVA
{

#define RENDER_VERIFY(command) \
{ \
	if(!Thread::IsMainThread() && RenderManager::Instance()->GetNonMainLockCount() == 0)\
	{\
		DVASSERT(0 && "Application tried to call GL or DX in separate thread without lock");\
	}\
	/*Logger::Debug("%s started",  #command);*/\
	HRESULT xhr = command;\
	/*Logger::Debug("%s ended",  #command);*/\
	if (FAILED(xhr))Logger::Debug("%s file:%s line:%d failed with errorcode: 0x%08x %s", #command, __FILE__, __LINE__, xhr, DXHelperGetErrorDescription(xhr));\
}

	const char8 * DXHelperGetErrorDescription(HRESULT hr);
	

/**
	DirectX objects crash the app when you pass them to dynamic_cast
	This function is cloned from SafeRelease to perform that without additional checks.
	Original SafeRelease function allow you to verify what happens with BaseObjects and whom release them wrongly
	
	\todo find smart solution to avoid D3DSafeRelease and make it in one SafeRelease call
 */
template<class C>
void D3DSafeRelease(C * &c) 
{ 
	if (c) 
	{
		c->Release();
		c = 0;
	}
}


};
#endif // __DAVAENGINE_DIRECTX9__
#endif // __DAVAENGINE_D3D9HELPERS_H__