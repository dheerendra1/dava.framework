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
#ifndef __DAVAENGINE_STATICMESHGLES_H__
#define __DAVAENGINE_STATICMESHGLES_H__

#include "Base/BaseTypes.h"
#include "Base/BaseMath.h"
#include "Render/3D/PolygonGroup.h"
#include "Render/Material.h"

namespace DAVA
{
	
class StaticMesh : public SceneNode
{
public:
	StaticMesh(Scene * _scene);
	~StaticMesh();
	
	void	Create(uint32 _polygroupCount);
	//int32	Release();
	
	virtual void Draw();
	// TODO pass material to it
	virtual void DrawPolygonGroup(int32 index, Material * material);
	
	inline uint32			GetPolygonGroupCount();
	inline PolygonGroup *	GetPolygonGroup(uint32 index);
	
protected:	
	uint32					polygroupCount;
	Vector<PolygonGroup *>	polygroups;
};

// Static Mesh Implementation
	
inline uint32 StaticMesh::GetPolygonGroupCount()
{
	return polygroupCount;
}
	
inline PolygonGroup * StaticMesh::GetPolygonGroup(uint32 index)
{
	return polygroups[index];
}

};

#endif // __DAVAENGINE_STATICMESHGLES_H__






