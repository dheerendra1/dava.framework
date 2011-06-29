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
#ifndef __DAVAENGINE_STATICVERTEXBUFFER_H__
#define __DAVAENGINE_STATICVERTEXBUFFER_H__

#include "Render/RenderBase.h"


#if defined(__DAVAENGINE_DIRECTX9__)

#include "Render/VertexBuffer.h"

namespace DAVA
{

class StaticVertexBuffer : public VertexBuffer
{
	LPDIRECT3DVERTEXBUFFER9 vertexBuffer;

	int32					vertexFormat;

	int32					vertexSize;
	int32					vertexCount;
	int32					position;
	bool					locked;
	DWORD					fvf;
public :

	StaticVertexBuffer(int32 vertexFormat, const int32 vertexSize, const int32 vertexCount);
	~StaticVertexBuffer();
	
	void			* Lock(const int32 vertexCount, int32 & startVertex);
	void			Unlock();
	eBufferType		GetType();
	void			Flush();
	int32			GetFormat();
	inline int32	GetVertexSize() { return vertexSize; };

	inline LPDIRECT3DVERTEXBUFFER9 GetNativeBuffer() { return vertexBuffer; };
};

}

#endif // #if defined(__DAVAENGINE_DIRECTX9__)

#endif // __DAVAENGINE_STATICVERTEXBUFFER_H__




