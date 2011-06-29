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
#include "Render/DynamicVertexBuffer.h"
#include "Utils/StringFormat.h"
#include "Render/RenderManager.h"
#include "Render/D3D9Helpers.h"

#if defined(__DAVAENGINE_DIRECTX9__)

namespace DAVA
{


DynamicVertexBuffer::DynamicVertexBuffer(int32 _vertexFormat, const int32 _vertexCount)
{
	vertexSize = DAVA::GetVertexSize(_vertexFormat);
	vertexCount = _vertexCount;
	vertexFormat = _vertexFormat;
	fvf = BuildD3D9VertexFormat(vertexFormat);

	// all creation of the object placed there
	Invalidate();
}

// Resource overloaded functions
void DynamicVertexBuffer::Lost()
{
	Unlock();
	D3DSafeRelease(vertexBuffer);
}
void DynamicVertexBuffer::Invalidate()
{
	vertexBuffer = 0;
	position = 0;
	needFlush = true;
	locked = false;

	LPDIRECT3DDEVICE9 deviceDX9 = RenderManager::Instance()->GetD3DDevice();

	HRESULT hr = deviceDX9->CreateVertexBuffer( vertexCount * vertexSize,
		D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
		fvf,
		D3DPOOL_DEFAULT,
		&vertexBuffer, 
		0);

	if (!FAILED(hr))
	{
		Logger::Debug(Format("[DynamicVertexBuffer] created >> elementsize(%d) buffersize(%d)", vertexSize, vertexCount));
	}
}


DynamicVertexBuffer::~DynamicVertexBuffer()
{
	Unlock();

	D3DSafeRelease(vertexBuffer);
	Logger::Debug("[DynamicVertexBuffer] released");
}

void			* DynamicVertexBuffer::Lock(const int32 lockVertexCount, int32 & startVertex)
{
	startVertex = 0;
	void * lockedData = 0;

	// Ensure there is enough space in the VB for this data
	if ( lockVertexCount > vertexCount ) { return 0; }

	if ( vertexBuffer )
	{
		DWORD flags = LOCKFLAGS_APPEND;

		// If either user forced us to flush,
		//  or there is not enough space for the vertex data,
		//  then flush the buffer contents
		//
		if ( needFlush || ( ( lockVertexCount + position ) > vertexCount ) )
		{
			needFlush = false;
			position = 0;
			flags = LOCKFLAGS_FLUSH;
		}

		HRESULT hr = vertexBuffer->Lock( position * vertexSize, 
			lockVertexCount * vertexSize, 
			&lockedData, 
			flags );

		if ( hr == D3D_OK )
		{
			locked = true;
			startVertex = position;
			position += lockVertexCount;
		}
	}

	return lockedData;
}

void			DynamicVertexBuffer::Unlock()
{
	if (locked)
	{
		vertexBuffer->Unlock();
		locked = false;
	}
}

eBufferType		DynamicVertexBuffer::GetType()
{
	return EBT_DYNAMIC;
}

void DynamicVertexBuffer::Flush()
{
	needFlush = true;
}

int32 DynamicVertexBuffer::GetFormat()
{
	return vertexFormat;
}


};


#endif // #if defined(__DAVAENGINE_DIRECTX9__)
