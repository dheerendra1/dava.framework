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
#ifndef __DAVAENGINE_DYNAMICINDEXBUFFER_H__
#define __DAVAENGINE_DYNAMICINDEXBUFFER_H__

#include "Base/BaseTypes.h"
#include "Render/RenderBase.h"
#include "Render/IndexBuffer.h"

#if defined(__DAVAENGINE_DIRECTX9__)

namespace DAVA
{

class DynamicIndexBuffer : public IndexBuffer
{
	LPDIRECT3DINDEXBUFFER9  indexBuffer;

	int32					indexSize;
	int32					indexCount;
	int32					position;
	int32					indexFormat;

	bool					locked;
	bool					needFlush;

	enum LOCK_FLAGS
	{
		LOCKFLAGS_FLUSH  = D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD,
		LOCKFLAGS_APPEND = D3DLOCK_NOSYSLOCK | D3DLOCK_NOOVERWRITE
	};

public :

	DynamicIndexBuffer(int32 indexFormat, int32 indexCount);
	~DynamicIndexBuffer();

	void			* Lock(const int32 indexCount, int32 & startIndex);
	void			Unlock();
	eBufferType		GetType();
	void			Flush();
	int32			GetFormat();
	LPDIRECT3DINDEXBUFFER9  GetNativeBuffer() { return indexBuffer; }

	// Resource overloaded functions
	virtual void Lost();
	virtual void Invalidate();
};


inline int32 BuildD3D9IndexFormat(int32 indexFormat)
{
	switch(indexFormat)
	{
	case EIF_16:
		return D3DFMT_INDEX16;
		break;
	case EIF_32:
		return D3DFMT_INDEX32;
		break;
	default:
		return 0;
	}
}

}

#endif // #if defined(__DAVAENGINE_DIRECTX9__)
#endif // __LOGENGINE_DYNAMICIB_H__


