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
#include "Render/StaticIndexBuffer.h"
#include "Render/DynamicIndexBuffer.h"
#include "Utils/StringFormat.h"
#include "Render/RenderManager.h"
#include "Render/D3D9Helpers.h"

#if defined(__DAVAENGINE_DIRECTX9__)

namespace DAVA
{

StaticIndexBuffer::StaticIndexBuffer(int32 _indexFormat, const int32 _indexSize, const int32 _indexCount)
{
	LPDIRECT3DDEVICE9 deviceDX9 = RenderManager::Instance()->GetD3DDevice();
	indexBuffer = 0;
	position = 0;
	locked = false;
	indexSize = _indexSize;
	indexCount = _indexCount;
	indexFormat = _indexFormat;

	HRESULT hr = deviceDX9->CreateIndexBuffer( indexCount * indexSize,
		D3DUSAGE_WRITEONLY,
		(D3DFORMAT)BuildD3D9IndexFormat(indexFormat),
		D3DPOOL_DEFAULT,
		&indexBuffer, 
		0);
	if (!FAILED(hr))
	{
		Logger::Debug(Format("[StaticIndexBuffer] created >> elementsize(%d) buffersize(%d)\n", _indexSize, _indexCount));
	}
}

StaticIndexBuffer::~StaticIndexBuffer()
{
	Unlock();

//	RenderSystemDX9 * renderSystem = (RenderSystemDX9 *)Global::GetRenderSystem();
//	renderSystem->pID3DDevice->SetIndices(0);

	D3DSafeRelease(indexBuffer);
	Logger::Debug("[StaticIndexBuffer] released\n");
}

void * StaticIndexBuffer::Lock(const int32 lockIndexCount, int32 & startIndex)
{
	startIndex = 0;
	void * lockedData = 0;

	// Ensure there is enough space in the VB for this data
	if ( lockIndexCount > indexCount ) { return 0; }

	if ( indexBuffer )
	{
		DWORD flags = D3DLOCK_NOSYSLOCK;


		HRESULT hr = indexBuffer->Lock(position * indexSize, 
			lockIndexCount * indexSize, 
			&lockedData, 
			flags );

		if ( hr == D3D_OK )
		{
			locked = true;
			startIndex = position;
			position += lockIndexCount;
		}
	}

	return lockedData;
}

void StaticIndexBuffer::Unlock()
{
	if (locked)
	{
		indexBuffer->Unlock();
		locked = false;
	}
}

eBufferType	StaticIndexBuffer::GetType()
{
	return EBT_STATIC;
}

int32 StaticIndexBuffer::GetFormat()
{
	return indexFormat;
}

void StaticIndexBuffer::Flush()
{
	position = 0;
}

};

#endif // #if defined(__DAVAENGINE_DIRECTX9__)
