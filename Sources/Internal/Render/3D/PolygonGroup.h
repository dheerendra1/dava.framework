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
#ifndef __DAVAENGINE_POLYGONGROUP_H__
#define __DAVAENGINE_POLYGONGROUP_H__

#include "Base/BaseTypes.h"
#include "Base/BaseMath.h"
#include "Render/VertexBuffer.h"

namespace DAVA
{	
/**
	\ingroup render_3d
	\brief Group of polygons with same data type & structure
 */
class PolygonGroup
{
public:
	enum VertexDataType
	{
		VERTEX_FLOAT = 1,
		//		VERTEX_FIXED16_16,
		//		VERTEX_SHORT,
	};
	
	enum IndexFormat
	{
		INDEX_16 = 1,
		INDEX_32,
	};
	
	PolygonGroup();
	~PolygonGroup();
	
	//! Getters
	inline void	GetCoord(int32 i, Vector3 & v);
	inline void	GetNormal(int32 i, Vector3 & v);
	inline void	GetTangent(int32 i, Vector3 & v);
	inline void	GetBinormal(int32 i, Vector3 & v);
	
	inline void	GetColor(int32 i, RGBColor & v);
	inline void	GetTexcoord(int32 ti, int32 i, Vector2 & v);
	inline void	GetIndex(int32 i, int32 & index);
	
	//! Setters
	inline void	SetCoord(int32 i, const Vector3 & v);
	inline void	SetNormal(int32 i, const Vector3 & v);
	inline void	SetTangent(int32 i, const Vector3 & v);
	inline void	SetBinormal(int32 i, const Vector3 & v);
	
	inline void	SetColor(int32 i, const RGBColor & v);
	inline void	SetTexcoord(int32 ti, int32 i, const Vector2 & v);
	inline void	SetJointIndex(int32 vIndex, int32 jointIndex, int32 boneIndexValue);
	inline void SetJointCount(int32 vIndex, int32 jointCount);
	inline void	SetWeight(int32 vIndex, int32 jointIndex, float32 boneWeightValue);
	
	inline void	SetIndex(int32 i, int16 index);
	
	inline int32 GetVertexCount();
	inline int32 GetIndexCount();
	
	inline AABBox3 & GetBoundingBox();
	
	int32	vertexCount;
	int32	indexCount;
	int32	textureCoordCount;
	int32	vertexStride;
	int32	vertexFormat;
	int32	renderPassCount;
	int32	triangleCount;
	
	Vector3		*vertexArray;
	Vector2		**textureCoordArray;
	Vector3		*normalArray;
	Vector3		*tangentArray;
	Vector3		*binormalArray;
	int32		*jointIdxArray;
	float32		*weightArray;

	int32		*jointCountArray;
	
	RGBColor	*colorArray;
	int16		*indexArray;
	uint8		*meshData;
	
	AABBox3		aabbox;
	
	/*
		Used for animated meshes to hold original vertexes in array that suitable for fast access
	 */
	void		CreateBaseVertexArray();
	Vector3		* baseVertexArray;
	
	void	AllocateData(
						 int32 meshFormat, 
						 int32 vertexCount, 
						 int32 indexCount, 
						 int32 textureCoordCount);
	void	ReleaseData();
private:
	
};

// Static Mesh Implementation	

inline void	PolygonGroup::SetCoord(int32 i, const Vector3 & _v)
{
	Vector3 * v = (Vector3 *)((uint8 *)vertexArray + i * vertexStride);  
	*v = _v;
	aabbox.AddPoint(_v);
}

inline void	PolygonGroup::SetNormal(int32 i, const Vector3 & _v)
{
	Vector3 * v = (Vector3 *)((uint8 *)normalArray + i * vertexStride);  
	*v = _v;
}

inline void	PolygonGroup::SetTangent(int32 i, const Vector3 & _v)
{
	Vector3 * v = (Vector3 *)((uint8 *)tangentArray + i * vertexStride);  
	*v = _v;
}

inline void	PolygonGroup::SetBinormal(int32 i, const Vector3 & _v)
{
	Vector3 * v = (Vector3 *)((uint8 *)binormalArray + i * vertexStride);  
	*v = _v;
}

inline void	PolygonGroup::SetColor(int32 i, const RGBColor & _c)
{
	RGBColor * c = (RGBColor *)((uint8 *)colorArray + i * vertexStride);  
	*c = _c;
}

inline void	PolygonGroup::SetTexcoord(int32 ti, int32 i, const Vector2 & _t)
{
	Vector2 * t = (Vector2 *)((uint8 *)textureCoordArray[ti] + i * vertexStride);  
	*t = _t;
}
	
inline void	PolygonGroup::SetJointIndex(int32 vIndex, int32 jointIndex, int32 boneIndexValue)
{
	int32 * t = (int32*)((uint8*)jointIdxArray + vIndex * vertexStride);  
	t[jointIndex] = boneIndexValue;
}
	
inline void	PolygonGroup::SetWeight(int32 vIndex, int32 jointIndex, float32 boneWeightValue)
{
	float32 * t = (float32*)((uint8*)weightArray + vIndex * vertexStride);  
	t[jointIndex] = boneWeightValue;
}
	
inline void PolygonGroup::SetJointCount(int32 vIndex, int32 jointCount)
{
	jointCountArray[vIndex] = jointCount;
}


inline void	PolygonGroup::SetIndex(int32 i, int16 index)
{
	indexArray[i] = index;
}

inline void	PolygonGroup::GetCoord(int32 i, Vector3 & _v)
{
	Vector3 * v = (Vector3 *)((uint8 *)vertexArray + i * vertexStride);  
	_v = *v;
}

inline void	PolygonGroup::GetNormal(int32 i, Vector3 & _v)
{
	Vector3 * v = (Vector3 *)((uint8 *)normalArray + i * vertexStride);  
	_v = *v;
}

inline void	PolygonGroup::GetTangent(int32 i, Vector3 & _v)
{
	Vector3 * v = (Vector3 *)((uint8 *)tangentArray + i * vertexStride);  
	_v = *v;
}

inline void	PolygonGroup::GetBinormal(int32 i, Vector3 & _v)
{
	Vector3 * v = (Vector3 *)((uint8 *)binormalArray + i * vertexStride);  
	_v = *v;
}

inline void	PolygonGroup::GetColor(int32 i, RGBColor & _c)
{
	RGBColor * c = (RGBColor *)((uint8 *)colorArray + i * vertexStride);  
	_c = *c;
}

inline void	PolygonGroup::GetTexcoord(int32 ti, int32 i, Vector2 & _t)
{
	Vector2 * t = (Vector2 *)((uint8 *)textureCoordArray[ti] + i * vertexStride);  
	_t = *t;
}

inline void	PolygonGroup::GetIndex(int32 i, int32 &index)
{
	index = indexArray[i];
}
	
inline int32 PolygonGroup::GetVertexCount()
{
	return vertexCount;
}
inline int32 PolygonGroup::GetIndexCount()
{
	return indexCount;
}

inline AABBox3 & PolygonGroup::GetBoundingBox()
{
	return aabbox;
}
	




};

#endif // __DAVAENGINE_POLYGONGROUPGLES_H__





