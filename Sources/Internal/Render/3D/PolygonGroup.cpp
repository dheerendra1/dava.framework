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
#include "Render/3D/PolygonGroup.h"


namespace DAVA 
{
	
PolygonGroup::PolygonGroup()
:	vertexCount(0),
	indexCount(0),
	textureCoordCount(0),
	vertexStride(0),
	vertexFormat(0),
	renderPassCount(0),
	triangleCount(0),
	
	vertexArray(0), 
	textureCoordArray(0),
	normalArray(0), 
	tangentArray(0),
	binormalArray(0),
	jointIdxArray(0), 
	weightArray(0),
	jointCountArray(0),
	
	colorArray(0), 
	indexArray(0), 
	meshData(0),
	baseVertexArray(0)
{
}

PolygonGroup::~PolygonGroup()
{
	ReleaseData();
}

void PolygonGroup::AllocateData(int32 _meshFormat, int32 _vertexCount, int32 _indexCount, int32 _textureCoordCount)
{
	vertexCount = _vertexCount;
	indexCount = _indexCount;
	textureCoordCount = _textureCoordCount;
	vertexStride = GetVertexSize(_meshFormat);
	vertexFormat = _meshFormat;
	
	meshData = new uint8[vertexStride * vertexCount];
	indexArray = new int16[indexCount];
	textureCoordArray = new Vector2*[textureCoordCount];
	
    renderDataObject = new RenderDataObject();

	int32 baseShift = 0;
	if (vertexFormat & EVF_VERTEX)
	{
		vertexArray = reinterpret_cast<Vector3*>(meshData + baseShift);
		baseShift += GetVertexSize(EVF_VERTEX);

        renderDataObject->SetStream(EVF_VERTEX, TYPE_FLOAT, 3, vertexStride, vertexArray);
    }
	if (vertexFormat & EVF_NORMAL)
	{
		normalArray = reinterpret_cast<Vector3*>(meshData + baseShift);
		baseShift += GetVertexSize(EVF_NORMAL);
        
        renderDataObject->SetStream(EVF_NORMAL, TYPE_FLOAT, 3, vertexStride, normalArray);
	}
	if (vertexFormat & EVF_COLOR)
	{
		colorArray = reinterpret_cast<RGBColor*>(meshData + baseShift);
		baseShift += GetVertexSize(EVF_COLOR);
        
        // need DWORD color support  
        // renderDataObject->SetStream(EVF_COLOR, TYPE_FLOAT, 3, vertexStride, vertexArray);
        //
    }
	if (vertexFormat & EVF_TEXCOORD0)
	{
		textureCoordArray[0] = reinterpret_cast<Vector2*>(meshData + baseShift);
		baseShift += GetVertexSize(EVF_TEXCOORD0);
        
        renderDataObject->SetStream(EVF_TEXCOORD0, TYPE_FLOAT, 2, vertexStride, textureCoordArray[0]);
	}
	if (vertexFormat & EVF_TEXCOORD1)
	{
		textureCoordArray[1] = reinterpret_cast<Vector2*>(meshData + baseShift);
		baseShift += GetVertexSize(EVF_TEXCOORD1);

        renderDataObject->SetStream(EVF_TEXCOORD1, TYPE_FLOAT, 2, vertexStride, textureCoordArray[1]);
    }
	if (vertexFormat & EVF_TEXCOORD2)
	{
		textureCoordArray[2] = reinterpret_cast<Vector2*>(meshData + baseShift);
		baseShift += GetVertexSize(EVF_TEXCOORD2);
        
        renderDataObject->SetStream(EVF_TEXCOORD2, TYPE_FLOAT, 2, vertexStride, textureCoordArray[2]);
	}
	if (vertexFormat & EVF_TEXCOORD3)
	{
		textureCoordArray[3] = reinterpret_cast<Vector2*>(meshData + baseShift);
		baseShift += GetVertexSize(EVF_TEXCOORD3);
        
        renderDataObject->SetStream(EVF_TEXCOORD3, TYPE_FLOAT, 2, vertexStride, textureCoordArray[3]);
	}	
	if (vertexFormat & EVF_TANGENT)
	{
		tangentArray = reinterpret_cast<Vector3*>(meshData + baseShift);
		baseShift += GetVertexSize(EVF_TANGENT);

        renderDataObject->SetStream(EVF_TANGENT, TYPE_FLOAT, 3, vertexStride, tangentArray);
	}
	if (vertexFormat & EVF_BINORMAL)
	{
		binormalArray = reinterpret_cast<Vector3*>(meshData + baseShift);
		baseShift += GetVertexSize(EVF_BINORMAL);

        renderDataObject->SetStream(EVF_BINORMAL, TYPE_FLOAT, 3, vertexStride, binormalArray);
	}
    
	if (vertexFormat & EVF_JOINTWEIGHT)
	{
		jointIdxArray = reinterpret_cast<int32*>(meshData + baseShift);
		weightArray = reinterpret_cast<float32*>(meshData + baseShift + 4 * 4);
		baseShift += GetVertexSize(EVF_JOINTWEIGHT);
		
		jointCountArray = new int32[vertexCount];
	}
}
    
void PolygonGroup::BuildTangents()
{
    if (vertexFormat & EVF_TANGENT)
    {
        Logger::Debug("PolygonGroup::BuildTangents - this polygroup already have tangents.");
        return;
    }
}
    
void PolygonGroup::CreateBaseVertexArray()
{
	baseVertexArray = new Vector3[vertexCount];
	for (int v = 0; v < vertexCount; ++v)
	{
		GetCoord(v, baseVertexArray[v]);
	}
}
	
void PolygonGroup::ReleaseData()
{
    SafeRelease(renderDataObject);
    
	if (jointCountArray)
	{
		delete [] jointCountArray;
		jointCountArray = 0;
	}
	if (meshData)
	{
		delete [] meshData;
	}
	if (indexArray)
	{
		delete [] indexArray;
	}
	if (textureCoordArray)
	{
		delete [] textureCoordArray;
	}
}
	
void PolygonGroup::BuildVertexBuffer()
{
    renderDataObject->BuildVertexBuffer(vertexCount);
};

};






