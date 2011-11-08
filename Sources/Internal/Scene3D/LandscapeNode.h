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
#include "Scene3D/Frustum.h"


namespace DAVA
{

class Scene;
class Image;
class Texture;
class RenderDataObject;
class Shader;

    
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
    
template <class T>
class LinearQuadTree
{
public:
    
    
    

};
    
/**    
    \brief Implementation of cdlod algorithm to render landscapes
    This class is base of the landscape code on all platforms
    Landscape node is always axial aligned for simplicity of frustum culling calculations
    Keep in mind that landscape orientation cannot be changed using localTransform and worldTransform matrices. 
 */ 

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
    
	LandscapeNode(Scene * scene);
	virtual ~LandscapeNode();
    
    /**
        \brief Set lod coefficients for dynamic roam landscape
        Default values: (60, 120, 240, 480)
        Every next value should be almost twice higher than previous to avoid gaps between levels
     */
    void SetLods(const Vector4 & lods);
    

    enum eRenderingMode
    {
        RENDERING_MODE_TEXTURE = 0,
        RENDERING_MODE_DETAIL_SHADER, 
        RENDERING_MODE_BLENDED_SHADER, 
    };
    
    /**
        \brief Builds landscape from heightmap image and bounding box of this landscape block
        \param[in] renderingMode rendering mode of landscape/
        \param[in] landscapeBox axial-aligned bounding box of the landscape block
     */
    void BuildLandscapeFromHeightmapImage(eRenderingMode renderingMode, const String & heightmapPathname, const AABBox3 & landscapeBox);
    
    enum eTextureLevel
    {
        TEXTURE_TEXTURE0 = 0,
        TEXTURE_TEXTURE1,
        TEXTURE_DETAIL = TEXTURE_TEXTURE1,
        TEXTURE_BUMP,
        TEXTURE_TEXTUREMASK, 
        TEXTURE_COUNT,
    };
    
    /**
        \brief Set texture for the specific texture level
        To render landscape you need to set textures. 
            
        For RENDERING_MODE_TEXTURE you need to set only TEXTURE_TEXTURE0.
        For RENDERING_MODE_DETAIL_SHADER you have to set TEXTURE_TEXTURE0 and TEXTURE_DETAIL
        For RENDERING_MODE_BLENDED_SHADER you have to set TEXTURE_TEXTURE0, TEXTURE_TEXTURE1, TEXTURE_TEXTUREMASK
     
        \param[in] level level of texture you want to set
        \param[in] texture pointer to Texture object you want to set
     */
    void SetTexture(eTextureLevel level, Texture * texture);
    
    /**
        \brief Set texture for the specific texture level
        To render landscape you need to set textures.  
          
        \param[in] level level of texture you want to set
        \param[in] textureName name of texture you want to open and set to specific level
     */
    void SetTexture(eTextureLevel level, const String & textureName);
    
    /**
        \brief Overloaded draw function to draw landscape
     */
	virtual void Draw();
    
protected:	
    
    class LandscapeQuad
    {
    public:
        int16   x, y;
        //int16   xbuf, ybuf;
        int16   size;
        int8    lod;
        int8    rdoQuad;
        AABBox3 bbox;
        uint32  frame;
    };
    
    class LandscapeVertex
    {
    public:
        Vector3 position;
        Vector2 texCoord;
    };
    
    
    static const int32 RENDER_QUAD_WIDTH = 129;
    static const int32 RENDER_QUAD_AND = RENDER_QUAD_WIDTH - 2;
    

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
    
    int8 AllocateRDOQuad(LandscapeQuad * quad);
    void ReleaseAllRDOQuads();

    Vector<LandscapeVertex *> landscapeVerticesArray;
    Vector<RenderDataObject *> landscapeRDOArray;
    
    uint16 * indices;
    Texture * textures[TEXTURE_COUNT];
    
    int32 lodLevelsCount;
    float32 lodDistance[8]; //
    float32 lodSqDistance[8];
    
    QuadTreeNode<LandscapeQuad> quadTreeHead;

    List<QuadTreeNode<LandscapeQuad>*> fans;
    
    int32 allocatedMemoryForQuads;
    
    Vector3 cameraPos;
    Frustum *frustum;
    
    eRenderingMode renderingMode;
    ePrimitiveType primitypeType;

    void InitShaders();
    void ReleaseShaders();
    
    int32 uniformTexture;
    int32 uniformDetailTexture;
    int32 uniformCameraPosition;
    
    Shader * activeShader;
    Shader * singleTextureShader;
    Shader * detailShader;
    Shader * blendedShader;
    
    int32 uniformTexture0;
    int32 uniformTexture1;
    int32 uniformTextureMask;
    
    
};

	
};

#endif // __DAVAENGINE_LANDSCAPE_NODE_H__





