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
#ifndef __DAVAENGINE_LANDSCAPE_NODE_H__
#define __DAVAENGINE_LANDSCAPE_NODE_H__

#include "Base/BaseObject.h"
#include "Base/BaseTypes.h"
#include "Base/BaseMath.h"
#include "Render/RenderBase.h"
#include "Scene3D/SceneNode.h"

namespace DAVA
{

class Scene;
class Image;
class Texture;
class RenderDataObject;


/**    
    \brief Implementation of cdlod algorithm to render landscapes
    This class is base of the landscape code on all platforms
    Landscape node is always axial aligned for simplicity of frustum culling calculations
 */ 
    
template<class T>
class QuadTreeNode
{
public:
    QuadTreeNode()
    {
        childs = 0;
        parent = 0;
        for (int32 k = 0; k < 4; ++k)
            neighbours[k] = 0;
    }
    ~QuadTreeNode()
    {
        SafeDeleteArray(childs);
    }
    
    void AllocChilds()
    {
        childs = new QuadTreeNode[4];
    }
    
    QuadTreeNode * childs;  // It's array of 4 child nodes
    QuadTreeNode * parent;
    QuadTreeNode * neighbours[4]; 
    T data;
};
    
class QuadTree
{
public:
    
    

};
    
class LandscapeNode : public SceneNode
{
public:	
    enum 
    {
        LEFT = 0,
        RIGHT = 1,
        TOP = 2,
        BOTTOM = 3,
    };
    
    enum eTextureType
    {
        TEXTURE_BASE = 0,
        TEXTURE_DETAIL,
        TEXTURE_BUMP,
        TEXTURE_COUNT,
    };
    
    
	LandscapeNode(Scene * scene);
	virtual ~LandscapeNode();
    
    void BuildLandscapeFromHeightmapImage(const String & heightmapPathname, const AABBox3 & landscapeBox);
    void SetTexture(eTextureType type, Texture * texture);
    
	virtual void Draw();
    
protected:	
    class LandscapeQuad
    {
    public:
        int16   x, y;
        int16   size;
        int8    lod;
        AABBox3 bbox;
        uint32  frame;
    };
    
    class LandscapeVertex
    {
    public:
        Vector3 position;
        Vector2 texCoord;
    };

    void RecursiveBuild(QuadTreeNode<LandscapeQuad> * currentNode, int32 level, int32 maxLevels);
    QuadTreeNode<LandscapeQuad> * FindNodeWithXY(QuadTreeNode<LandscapeQuad> * currentNode, int16 quadX, int16 quadY, int16 quadSize);
    void FindNeighbours(QuadTreeNode<LandscapeQuad> * currentNode);
    void MarkFrames(QuadTreeNode<LandscapeQuad> * currentNode, int32 & depth);

    Vector3 GetPoint(int16 x, int16 y, uint8 height);
    void DrawQuad(QuadTreeNode<LandscapeQuad> * currentNode, int8 lod);
    void Draw(QuadTreeNode<LandscapeQuad> * currentNode);
    void DrawFans();

    Image *     heightmap;
    AABBox3     box;
    
    LandscapeVertex * landscapeVertices;
    RenderDataObject * landscapeRDO;
    
    uint16 * indices;
    Texture * textures[TEXTURE_COUNT];
    
    int32 lodLevelsCount;
    float32 lodDistance[8]; //
    float32 lodSqDistance[8];
    
    QuadTreeNode<LandscapeQuad> quadTreeHead;

    List<QuadTreeNode<LandscapeQuad>*> fans;
};

	
};

#endif // __DAVAENGINE_LANDSCAPE_NODE_H__





