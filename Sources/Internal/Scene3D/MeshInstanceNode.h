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
#ifndef __DAVAENGINE_MESH_INSTANCE_H__
#define __DAVAENGINE_MESH_INSTANCE_H__

#include "Scene3D/SceneNode3d.h"

namespace DAVA 
{
class Scene;
class StaticMesh;
class Material;
class MeshInstanceNode : public SceneNode3d
{
public:	
	MeshInstanceNode(Scene * _scene);
	~MeshInstanceNode();
	
	void AddPolygonGroup(StaticMesh * mesh, int32 polygonGroupIndex, Material* material);
	
	virtual void Draw();
	
	inline void SetVisible(bool isVisible);
	inline bool GetVisible();
	
	inline AABBox3 & GetBoundingBox();
	
	std::vector<StaticMesh*> & GetMeshes()
	{
		return meshes;
	}
	
    virtual SceneNode* Clone(SceneNode *dstNode = NULL);
//    virtual SceneNode* Clone();
    
	
protected:
//    virtual SceneNode* CopyDataTo(SceneNode *dstNode);

	std::vector<StaticMesh*> meshes;
	std::vector<int32> polygonGroupIndexes;
	std::vector<Material*> materials;
	AABBox3 bbox;
};
	
inline AABBox3 & MeshInstanceNode::GetBoundingBox()
{
	return bbox;
}

};

#endif // __DAVAENGINE_MESH_INSTANCE_H__