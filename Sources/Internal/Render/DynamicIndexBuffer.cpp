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
#include "Render/DynamicIndexBuffer.h"
#include "Utils/StringFormat.h"
#include "Render/RenderManager.h"
#include "Render/D3D9Helpers.h"

#if defined(__DAVAENGINE_DIRECTX9__)

namespace DAVA
{

DynamicIndexBuffer::DynamicIndexBuffer(int32 _indexFormat, const int32 _indexCount)
{
	indexBuffer = 0;
	position = 0;
	needFlush = true;
	locked = false;
	indexCount = _indexCount;
	indexFormat = _indexFormat;

	if (indexFormat == EIF_16)indexSize = 2;
	else if (indexFormat == EIF_32) indexSize = 4;

	Invalidate();
}

// Resource overloaded functions
void DynamicIndexBuffer::Lost()
{
	D3DSafeRelease(indexBuffer);
}

void DynamicIndexBuffer::Invalidate()
{
	LPDIRECT3DDEVICE9 deviceDX9 = RenderManager::Instance()->GetD3DDevice();


	HRESULT hr = deviceDX9->CreateIndexBuffer( indexCount * indexSize,
		D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
		(D3DFORMAT)BuildD3D9IndexFormat(indexFormat),
		D3DPOOL_DEFAULT,
		&indexBuffer, 
		0);

	if (!FAILED(hr))
	{
		Logger::Debug(Format("[DynamicIndexBuffer] created >> elementsize(%d) buffersize(%d)", indexSize, indexCount));
	}
}

DynamicIndexBuffer::~DynamicIndexBuffer()
{
	Unlock();
	D3DSafeRelease(indexBuffer);
	Logger::Debug("[DynamicIndexBuffer] released");
}

void * DynamicIndexBuffer::Lock(const int32 lockIndexCount, int32 & startIndex)
{
	startIndex = 0;
	void * lockedData = 0;

	// Ensure there is enough space in the VB for this data
	if (lockIndexCount > indexCount) { return 0; }

	if (indexBuffer)
	{
		DWORD flags = LOCKFLAGS_APPEND;

		// If either user forced us to flush,
		//  or there is not enough space for the vertex data,
		//  then flush the buffer contents
		//
		if ( needFlush || ( ( lockIndexCount + position ) > indexCount ) )
		{
			needFlush = false;
			position = 0;
			flags = LOCKFLAGS_FLUSH;
		}

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

void DynamicIndexBuffer::Unlock()
{
	if (locked)
	{
		indexBuffer->Unlock();
		locked = false;
	}
}

eBufferType DynamicIndexBuffer::GetType()
{
	return EBT_DYNAMIC;
}


int32 DynamicIndexBuffer::GetFormat()
{
	return indexFormat;
}

void DynamicIndexBuffer::Flush()
{
	needFlush = true;
}

};

#endif // #if defined(__DAVAENGINE_DIRECTX9__)
