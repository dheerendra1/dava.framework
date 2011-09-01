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
#include "Render/RenderBase.h"
#include "Render/RenderDataObject.h"
#include "Render/RenderManager.h"

namespace DAVA 
{
    
RenderDataStream::RenderDataStream()
{
    formatMark = EVF_VERTEX;
    type = TYPE_FLOAT;
    size = 0;
    stride = 0;
    pointer = 0;
}

RenderDataStream::~RenderDataStream()
{
    
}

void RenderDataStream::Set(eVertexDataType _type, int32 _size, int32 _stride, void * _pointer)
{
    type = _type;
    size = _size;
    stride = _stride;
    pointer = _pointer;
}
    
RenderDataObject::RenderDataObject()
{
    resultVertexFormat = 0;
    vboBuffer = 0;
}

RenderDataObject::~RenderDataObject()
{
    uint32 size = streamArray.size();
    for (uint32 k = 0; k < size; ++k)
    {
        SafeRelease(streamArray[k]);
    }
    //streamArray.clear();
    //streamMap.clear();
    
#if defined(__DAVAENGINE_OPENGL__)
    if (vboBuffer)
        RENDER_VERIFY(glDeleteBuffers(1, &vboBuffer));
#endif
}

RenderDataStream * RenderDataObject::SetStream(eVertexFormat formatMark, eVertexDataType vertexType, int32 size, int32 stride, void * pointer)
{
    Map<eVertexFormat, RenderDataStream *>::iterator iter = streamMap.find(formatMark);
    RenderDataStream * stream = 0;
    if (iter == streamMap.end())
    {
        // New item - add it
        resultVertexFormat |= formatMark;
        stream = new RenderDataStream(); // todo optimize dynamic object cache
        stream->formatMark = formatMark;
        stream->type = vertexType;
        stream->size = size;
        stream->stride = stride;
        stream->pointer = pointer;
        
        streamMap[formatMark] = stream;
        streamArray.push_back(stream);
    }else
    {
        stream = iter->second;
    }
    
    stream->formatMark = formatMark;
    stream->type = vertexType;
    stream->size = size;
    stream->stride = stride;
    stream->pointer = pointer;
    return stream;
}

uint32 RenderDataObject::GetResultFormat()
{
    return resultVertexFormat;
}
    
void RenderDataObject::BuildVertexBuffer(int32 vertexCount)
{
#if !defined(__DAVAENGINE_MACOS__)
    
#if defined (__DAVAENGINE_OPENGL__)
    uint32 size = streamArray.size();
    if (size == 0)return;
    
    //;
    
    for (uint32 k = 1; k < size; ++k)
    {
        DVASSERT(streamArray[k]->stride == streamArray[k - 1]->stride);
        //DVASSERT((uint8*)streamArray[k]->pointer == (uint8*)streamArray[k - 1]->pointer + GetVertexSize(streamArray[k - 1]->formatMark));
    }
    
    uint32 format = 0;
    for (uint32 k = 0; k < size; ++k)
    {
        format |= streamArray[k]->formatMark;
    }
    
    int32 stride = streamArray[0]->stride;
    
#if defined(__DAVAENGINE_MACOS__)
    RENDER_VERIFY(glGenBuffersARB(1, &vboBuffer));
    RENDER_VERIFY(glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboBuffer));
    RENDER_VERIFY(glBufferDataARB(GL_ARRAY_BUFFER_ARB, vertexCount * stride, streamArray[0]->pointer, GL_STATIC_DRAW_ARB));
#else
    RENDER_VERIFY(glGenBuffers(1, &vboBuffer));
    RENDER_VERIFY(glBindBuffer(GL_ARRAY_BUFFER, vboBuffer));
    RENDER_VERIFY(glBufferData(GL_ARRAY_BUFFER, vertexCount * stride, streamArray[0]->pointer, GL_STATIC_DRAW));
#endif
    streamArray[0]->pointer = 0;
    for (uint32 k = 1; k < size; ++k)
    {
        streamArray[k]->pointer = (uint8*)streamArray[k - 1]->pointer + GetVertexSize(streamArray[k - 1]->formatMark);
        //Logger::Debug("vbo offset: %d", (uint32)streamArray[k]->pointer);
    }
    
#if defined(__DAVAENGINE_MACOS__)
    RENDER_VERIFY(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
#else
    RENDER_VERIFY(glBindBuffer(GL_ARRAY_BUFFER, 0));
#endif

#endif // #if defined (__DAVAENGINE_OPENGL__)
    
#endif // #if !defined(__DAVAENGINE_MACOS__)
    
} 

}