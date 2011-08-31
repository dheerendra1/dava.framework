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
#include "Render/RenderManagerGL20.h"
#include "Render/Shader.h"

#if defined(__DAVAENGINE_OPENGL__)

namespace DAVA
{
	
RenderVertexAttributesState::RenderVertexAttributesState()
{
    activeVertexAttributes = 0;
}

void RenderVertexAttributesState::EnableVertexAttributes(uint32 attributesToEnable)
{
    uint32 diff = attributesToEnable ^ activeVertexAttributes;
    for (uint32 attribIndex = 0; attribIndex < 4; ++attribIndex)
        if ((diff >> attribIndex) & 1)
        {
            if ((attributesToEnable >> attribIndex) & 1)
                glEnableVertexAttribArray(attribIndex);
            else 
                glDisableVertexAttribArray(attribIndex);
        }
    activeVertexAttributes = attributesToEnable;
}

    
RenderManagerGL20::RenderManagerGL20(Core::eRenderer renderer)
    : RenderManager(renderer)
{
    enabledAttribCount = 0;
}
    
void RenderManagerGL20::AttachRenderData(Shader * shader)
{
    DVASSERT(shader);
    if (shader)
    {
        int32 currentEnabledAttribCount = 0;
        //glDisableVertexAttribArray(0);
        //glDisableVertexAttribArray(1);
                                   
        pointerArraysCurrentState = 0;
        
        
        //if (currentRenderData->vboBuffer != 0)
        //{
#if defined(__DAVAENGINE_MACOS__)
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, currentRenderData->vboBuffer);
#else
        glBindBuffer(GL_ARRAY_BUFFER, currentRenderData->vboBuffer);
#endif
        //}
        
        
        int32 size = (int32)currentRenderData->streamArray.size();
        for (int32 k = 0; k < size; ++k)
        {
            RenderDataStream * stream = currentRenderData->streamArray[k];
            GLboolean normalized = GL_FALSE;
            
            int32 attribIndex = shader->GetAttributeIndex(stream->formatMark);
            if (attribIndex != -1)
            {
                glVertexAttribPointer(attribIndex, stream->size, VERTEX_DATA_TYPE_TO_GL[stream->type], normalized, stream->stride, stream->pointer);
                
                if (attribIndex >= enabledAttribCount)  // enable only if it was not enabled on previous step
                {
                    glEnableVertexAttribArray(attribIndex);
                }
                if (attribIndex + 1 > currentEnabledAttribCount)
                    currentEnabledAttribCount = attribIndex + 1;    // count of enabled attributes

                pointerArraysCurrentState |= stream->formatMark;
            }
        };
        
        for (int32 p = currentEnabledAttribCount; p < enabledAttribCount; ++p)
        {
            glDisableVertexAttribArray(p);
        }
        
//        uint32 difference = pointerArraysCurrentState ^ pointerArraysRendererState;
//        
//        if (!(difference & EVF_VERTEX))
//        {
//            int32 attribIndex = shader->GetAttributeIndex(EVF_VERTEX);
//            RENDER_VERIFY(glDisableVertexAttribArray(attribIndex));
//        }
//        if (!(difference & EVF_TEXCOORD0))
//        {
//            int32 attribIndex = shader->GetAttributeIndex(EVF_TEXCOORD0);
//            RENDER_VERIFY(glDisableVertexAttribArray(attribIndex));
//        }
        pointerArraysRendererState = pointerArraysCurrentState;
    }
    
    // RENDER_VERIFY("RenderManagerGL20::AttachRenderData");
}  

    
};

#endif // __DAVAENGINE_OPENGL__
