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
#include "Render/StaticVertexBuffer.h"
#include "Render/DynamicVertexBuffer.h"
#include "Utils/StringFormat.h"
#include "Render/RenderManager.h"
#include "Render/D3D9Helpers.h"


#if defined(__DAVAENGINE_DIRECTX9__)

namespace DAVA
{


StaticVertexBuffer::StaticVertexBuffer(int32 _vertexFormat, const int32 _vertexSize, const int32 _vertexCount)
{
	LPDIRECT3DDEVICE9 deviceDX9 = RenderManager::Instance()->GetD3DDevice();
	vertexBuffer = 0;
	position = 0;
	locked = false;
	vertexSize = _vertexSize;
	vertexCount = _vertexCount;
	vertexFormat = _vertexFormat;
	fvf = BuildD3D9VertexFormat(vertexFormat);

	HRESULT hr = deviceDX9->CreateVertexBuffer( vertexCount * vertexSize,
		D3DUSAGE_WRITEONLY,
		fvf,	// Non standart FVF
		D3DPOOL_DEFAULT,
		&vertexBuffer,
		0);

	if (!FAILED(hr))
	{
		Logger::Debug(Format("[StaticVertexBuffer] created >> elementsize(%d) buffersize(%d)\n", _vertexSize, _vertexCount));
	}
}

StaticVertexBuffer::~StaticVertexBuffer()
{	
	//RenderSystemDX9 * renderSystem = (RenderSystemDX9 *)Global::GetRenderSystem();
	
	//renderSystem->pID3DDevice->SetStreamSource(0, 0, 0, 0);
	//renderSystem->pID3DDevice->SetVertexDeclaration(0);
	//renderSystem->ReleaseVertexDeclaration(vertexDecl);
	
	Unlock();
	D3DSafeRelease(vertexBuffer);
	Logger::Debug("[StaticVertexBuffer] released\n");
}

void * StaticVertexBuffer::Lock(const int32 lockVertexCount, int32 & startVertex)
{
	startVertex = 0;
	void * lockedData = 0;

	// Ensure there is enough space in the VB for this data
	if ( lockVertexCount > vertexCount ) { return 0; }

	if ( vertexBuffer )
	{
		DWORD flags = D3DLOCK_NOSYSLOCK;

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

void StaticVertexBuffer::Unlock()
{
	if (locked)
	{
		vertexBuffer->Unlock();
		locked = false;
	}
}

eBufferType	StaticVertexBuffer::GetType()
{
	return EBT_STATIC;
}

void StaticVertexBuffer::Flush()
{
	position = 0;
}
int32 StaticVertexBuffer::GetFormat()
{
	return vertexFormat;
}


};

#endif // #if defined(__DAVAENGINE_DIRECTX9__)

