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
#include "Scene3D/LandscapeNode.h"
#include "Render/Image.h"
#include "Render/RenderManager.h"
#include "Render/RenderHelper.h"
#include "Render/RenderDataObject.h"
#include "Render/Texture.h"
#include "Scene3D/Scene.h"

namespace DAVA
{
	
LandscapeNode::LandscapeNode(Scene * _scene)
	: SceneNode(_scene)
    , heightmap(0)
    , landscapeVertices(0)
    , landscapeRDO(0)
    , indices(0)
{
    for (int32 t = 0; t < TEXTURE_COUNT; ++t)
        textures[t] = 0;
}

LandscapeNode::~LandscapeNode()
{
    for (int32 t = 0; t < TEXTURE_COUNT; ++t)
    {
        SafeRelease(textures[t]);
    }
    SafeRelease(landscapeRDO);
    SafeDeleteArray(indices);
    SafeDeleteArray(landscapeVertices);
    SafeRelease(heightmap);
}

void LandscapeNode::BuildLandscapeFromHeightmapImage(const String & heightmapPathname, const AABBox3 & _box)
{
    heightmap = Image::CreateFromFile(heightmapPathname);
    
    if (heightmap->GetPixelFormat() != Image::FORMAT_A8)
    {
        Logger::Error("Image for landscape should be grayscale");
        SafeRelease(heightmap);
        return;
    }
    box = _box;    
    
    DVASSERT(heightmap->GetWidth() == heightmap->GetHeight());

    quadTreeHead.data.x = quadTreeHead.data.y = quadTreeHead.data.lod = 0;
    quadTreeHead.data.size = heightmap->GetWidth() - 1; 
    
    lodLevelsCount = 4;
    lodDistance[0] = 400;
    lodDistance[1] = 800;
    lodDistance[2] = 1500;
    lodDistance[3] = 3000;
    for (int32 ll = 0; ll < lodLevelsCount; ++ll)
        lodSqDistance[ll] = lodDistance[ll] * lodDistance[ll];
    
    RecursiveBuild(&quadTreeHead, 0, lodLevelsCount);
    FindNeighbours(&quadTreeHead);
    
    landscapeVertices = new LandscapeVertex[heightmap->GetWidth() * heightmap->GetHeight()];
    indices = new uint16[heightmap->GetWidth() * heightmap->GetHeight() * 6];
    
    int32 index = 0;
    for (int32 y = 0; y < heightmap->GetHeight(); ++y)
        for (int32 x = 0; x < heightmap->GetWidth(); ++x)
        {
            landscapeVertices[index].position = GetPoint(x, y, heightmap->GetData()[index]);
            landscapeVertices[index].texCoord = Vector2((float32)x / (float32)(heightmap->GetWidth() - 1), (float32)y / (float32)(heightmap->GetHeight() - 1));           
            landscapeVertices[index].texCoord *= 10.0f;
            index++;
        }
    
    // setup a base RDO
    landscapeRDO = new RenderDataObject();
    landscapeRDO->SetStream(EVF_VERTEX, TYPE_FLOAT, 3, sizeof(LandscapeVertex), &landscapeVertices[0].position); 
    landscapeRDO->SetStream(EVF_TEXCOORD0, TYPE_FLOAT, 2, sizeof(LandscapeVertex), &landscapeVertices[0].texCoord); 
}

/*
    level 0 = full landscape
    level 1 = first set of quads
    level 2 = 2
    level 3 = 3
    level 4 = 4
 */
    
//float32 LandscapeNode::BitmapHeightToReal(uint8 height)
Vector3 LandscapeNode::GetPoint(int16 x, int16 y, uint8 height)
{
    Vector3 res;
    res.x = (box.min.x + (float32)x / (float32)(heightmap->GetWidth() - 1) * (box.max.x - box.min.x));
    res.y = (box.min.y + (float32)y / (float32)(heightmap->GetWidth() - 1) * (box.max.y - box.min.y));
    res.z = (box.min.z + ((float32)height / 255.0f) * (box.max.z - box.min.z));
    return res;
};

void LandscapeNode::RecursiveBuild(QuadTreeNode<LandscapeQuad> * currentNode, int32 level, int32 maxLevels)
{
    currentNode->data.lod = level;

    
    // 
    // Check if we can build tree with less number of nodes
    // I think we should stop much earlier to perform everything faster
    //
    
    if (currentNode->data.size == 2)
    {
        // compute node bounding box
        uint8 * data = heightmap->GetData();
        for (int16 x = currentNode->data.x; x <= currentNode->data.x + currentNode->data.size; ++x)
            for (int16 y = currentNode->data.y; y <= currentNode->data.y + currentNode->data.size; ++y)
            {
                uint8 value = data[heightmap->GetWidth() * y + x];
                Vector3 pos = GetPoint(x, y, value);
                
                currentNode->data.bbox.AddPoint(pos);
            }
        return;
    }
    
    
    // alloc and process childs
    currentNode->AllocChilds();
    
    int16 minIndexX = currentNode->data.x;
    int16 minIndexY = currentNode->data.y;
    int16 size = currentNode->data.size;
    
    // We should be able to divide landscape by 2 here
    DVASSERT((size & 1) == 0);

    QuadTreeNode<LandscapeQuad> * child0 = &currentNode->childs[0];
    child0->data.x = minIndexX;
    child0->data.y = minIndexY;
    child0->data.size = size / 2;
    
    QuadTreeNode<LandscapeQuad> * child1 = &currentNode->childs[1];
    child1->data.x = minIndexX + size / 2;
    child1->data.y = minIndexY;
    child1->data.size = size / 2;

    QuadTreeNode<LandscapeQuad> * child2 = &currentNode->childs[2];
    child2->data.x = minIndexX;
    child2->data.y = minIndexY + size / 2;
    child2->data.size = size / 2;

    QuadTreeNode<LandscapeQuad> * child3 = &currentNode->childs[3];
    child3->data.x = minIndexX + size / 2;
    child3->data.y = minIndexY + size / 2;
    child3->data.size = size / 2;
    
    for (int32 index = 0; index < 4; ++index)
    {
        QuadTreeNode<LandscapeQuad> * child = &currentNode->childs[index];
        child->parent = currentNode;
        RecursiveBuild(child, level + 1, maxLevels);
        
        currentNode->data.bbox.AddPoint(child->data.bbox.min);
        currentNode->data.bbox.AddPoint(child->data.bbox.max);
    }


}
/*
    Neighbours looks up
    *********
    *0*1*0*1*
    **0***1**
    *2*3*2*3*
    ****0****
    *0*1*0*1*
    **2***3**
    *2*3*2*3*
    *********
    *0*1*0*1*
    **0***1**
    *2*3*2*3*
    ****2****
    *0*1*0*1*
    **2***3**
    *2*3*2*3*
    *********
 */

QuadTreeNode<LandscapeNode::LandscapeQuad> * LandscapeNode::FindNodeWithXY(QuadTreeNode<LandscapeQuad> * currentNode, int16 quadX, int16 quadY, int16 quadSize)
{
    if ((currentNode->data.x <= quadX) && (quadX < currentNode->data.x + currentNode->data.size))
        if ((currentNode->data.y <= quadY) && (quadY < currentNode->data.y + currentNode->data.size))
    {
        if (currentNode->data.size == quadSize)return currentNode;
        if (currentNode->childs)
        {
            for (int32 index = 0; index < 4; ++index)
            {
                QuadTreeNode<LandscapeQuad> * child = &currentNode->childs[index];
                QuadTreeNode<LandscapeQuad> * result = FindNodeWithXY(child, quadX, quadY, quadSize);
                if (result)
                    return result;
            } 
        }
    } else
    {
        return 0;
    }
    
    return 0;
}
    
void LandscapeNode::FindNeighbours(QuadTreeNode<LandscapeQuad> * currentNode)
{
    currentNode->neighbours[LEFT] = FindNodeWithXY(&quadTreeHead, currentNode->data.x - 1, currentNode->data.y, currentNode->data.size);
    currentNode->neighbours[RIGHT] = FindNodeWithXY(&quadTreeHead, currentNode->data.x + currentNode->data.size, currentNode->data.y, currentNode->data.size);
    currentNode->neighbours[TOP] = FindNodeWithXY(&quadTreeHead, currentNode->data.x, currentNode->data.y - 1, currentNode->data.size);
    currentNode->neighbours[BOTTOM] = FindNodeWithXY(&quadTreeHead, currentNode->data.x, currentNode->data.y + currentNode->data.size, currentNode->data.size);
    
    if (currentNode->childs)
    {
        for (int32 index = 0; index < 4; ++index)
        {
            QuadTreeNode<LandscapeQuad> * child = &currentNode->childs[index];
            FindNeighbours(child);
        }
    }
}

void LandscapeNode::MarkFrames(QuadTreeNode<LandscapeQuad> * currentNode, int32 & depth)
{
    if (--depth <= 0)
    {
        currentNode->data.frame = Core::Instance()->GetGlobalFrameIndex();
        depth++;
        return;
    }
    if (currentNode->childs)
    {
        for (int32 index = 0; index < 4; ++index)
        {
            QuadTreeNode<LandscapeQuad> * child = &currentNode->childs[index];
            MarkFrames(child, depth);
        }
    }
    depth++;
}
    
void LandscapeNode::SetTexture(eTextureType type, Texture * texture)
{
    texture->GenerateMipmaps();
    texture->SetWrapMode(Texture::WRAP_REPEAT, Texture::WRAP_REPEAT);
    SafeRelease(textures[type]);
    textures[type] = SafeRetain(texture);
}

void LandscapeNode::DrawQuad(QuadTreeNode<LandscapeQuad> * currentNode, int8 lod)
{
    int32 depth = currentNode->data.size / (1 << lod);
    if (depth == 1)
    {
        currentNode->parent->data.frame = Core::Instance()->GetGlobalFrameIndex();
    }else
    {
        int32 newdepth = (int)(logf((float)depth) / logf(2.0f) + 0.5f);
        int32 newdepth2 = CountLeadingZeros(depth);
        //Logger::Debug("dp: %d %d %d", depth, newdepth, newdepth2);
        DVASSERT(newdepth == newdepth2); // Check of math, we should use optimized version with depth2
        
        MarkFrames(currentNode, newdepth);
    }
    
    uint16 * drawIndices = indices;
    int32 step = (1 << lod);
    
    int16 width = heightmap->GetWidth();
    int32 cnt = 0;
    for (uint16 y = currentNode->data.y; y < currentNode->data.y + currentNode->data.size; y += step)
        for (uint16 x = currentNode->data.x; x < currentNode->data.x + currentNode->data.size; x += step)
        {
            *drawIndices++ = x + y * width;
            *drawIndices++ = (x + step) + y * width;
            *drawIndices++ = x + (y + step) * width;
            
            *drawIndices++ = (x + step) + y * width;
            *drawIndices++ = (x + step) + (y + step) * width;
            *drawIndices++ = x + (y + step) * width;     
 
            cnt += 6;
        }
    RenderManager::Instance()->SetRenderData(landscapeRDO);
    RenderManager::Instance()->DrawElements(PRIMITIVETYPE_TRIANGLELIST, cnt, EIF_16, indices); 
}
    
void LandscapeNode::DrawFans()
{
    uint32 currentFrame = Core::Instance()->GetGlobalFrameIndex();;
    int16 width = heightmap->GetWidth();
    
    List<QuadTreeNode<LandscapeQuad>*>::const_iterator end = fans.end();
    for (List<QuadTreeNode<LandscapeQuad>*>::iterator t = fans.begin(); t != end; ++t)
    {
        uint16 * drawIndices = indices;
        QuadTreeNode<LandscapeQuad>* node = *t;
        
        int32 count = 0;
        int16 halfSize = (node->data.size >> 1);
        drawIndices[count++] = (node->data.x + halfSize) + (node->data.y + halfSize) * width;
        drawIndices[count++] = (node->data.x) + (node->data.y) * width;
        
        if ((node->neighbours[TOP]) && (node->neighbours[TOP]->data.frame == currentFrame))
            drawIndices[count++] = (node->data.x + halfSize) + (node->data.y) * width;
        
        drawIndices[count++] = (node->data.x + node->data.size) + (node->data.y) * width;
        
        if ((node->neighbours[RIGHT]) && (node->neighbours[RIGHT]->data.frame == currentFrame))
            drawIndices[count++] = (node->data.x + node->data.size) + (node->data.y + halfSize) * width;
            
        drawIndices[count++] = (node->data.x + node->data.size) + (node->data.y + node->data.size) * width;
        
        if ((node->neighbours[BOTTOM]) && (node->neighbours[BOTTOM]->data.frame == currentFrame))
            drawIndices[count++] = (node->data.x + halfSize) + (node->data.y + node->data.size) * width;

        drawIndices[count++] = (node->data.x) + (node->data.y + node->data.size) * width;
        
        if ((node->neighbours[LEFT]) && (node->neighbours[LEFT]->data.frame == currentFrame))
            drawIndices[count++] = (node->data.x) + (node->data.y + halfSize) * width;

        drawIndices[count++] = (node->data.x) + (node->data.y) * width;
        
        //RenderManager::Instance()->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
        RenderManager::Instance()->DrawElements(PRIMITIVETYPE_TRIANGLEFAN, count, EIF_16, indices); 
    }
}
    
void LandscapeNode::Draw(QuadTreeNode<LandscapeQuad> * currentNode)
{
    Frustum * frustum = scene->GetClipCamera()->GetFrustum();
    if (!frustum->IsInside(currentNode->data.bbox))return;
    
    Camera * cam = scene->GetClipCamera();
    
    Vector3 corners[8];
    currentNode->data.bbox.GetCorners(corners);
    
    float32 minDist =  100000000.0f;
    float32 maxDist = -100000000.0f;
    for (int32 k = 0; k < 8; ++k)
    {
        Vector3 v = cam->GetPosition() - corners[k];
        float32 dist = v.Length();
        if (dist < minDist)
            minDist = dist;
        if (dist > maxDist)
            maxDist = dist;
    };
    
    int32 minLod = 0;
    int32 maxLod = 0;
    
    for (int32 k = 0; k < lodLevelsCount; ++k)
    {
        if (minDist > lodDistance[k])
            minLod = k + 1;
        if (maxDist > lodDistance[k])
            maxLod = k + 1;
    }
    
    // debug block
#if 1
    if (currentNode == &quadTreeHead)
    {
        //Logger::Debug("== draw start ==");
    }
    //Logger::Debug("%f %f %d %d", minDist, maxDist, minLod, maxLod);
#endif
                      
//    if (frustum->IsFullyInside(currentNode->data.bbox))
//    {
//        RenderManager::Instance()->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
//        RenderHelper::Instance()->DrawBox(currentNode->data.bbox);
//    }
    
    
    if ((minLod == maxLod) && (frustum->IsFullyInside(currentNode->data.bbox) || currentNode->data.size <= (1 << maxLod) + 1) )
    {
        //Logger::Debug("lod: %d depth: %d pos(%d, %d)", minLod, currentNode->data.lod, currentNode->data.x, currentNode->data.y);
        
//        if (currentNode->data.size <= (1 << maxLod))
//            RenderManager::Instance()->SetColor(0.0f, 1.0f, 0.0f, 1.0f);
//        if (frustum->IsFullyInside(currentNode->data.bbox))
//            RenderManager::Instance()->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
        //if (frustum->IsFullyInside(currentNode->data.bbox) && (currentNode->data.size <= (1 << maxLod)))
        //    RenderManager::Instance()->SetColor(1.0f, 1.0f, 0.0f, 1.0f);

            
        //RenderManager::Instance()->SetColor(0.0f, 1.0f, 0.0f, 1.0f);
        DrawQuad(currentNode, maxLod);
        //RenderManager::Instance()->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
        //RenderHelper::Instance()->DrawBox(currentNode->data.bbox);

        return;
    }

    
    if ((minLod != maxLod) && (currentNode->data.size <= (1 << maxLod) + 1))
    {
//        RenderManager::Instance()->SetColor(0.0f, 0.0f, 1.0f, 1.0f);
        //DrawQuad(currentNode, minLod);
        //RenderManager::Instance()->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
        //RenderHelper::Instance()->DrawBox(currentNode->data.bbox);
        fans.push_back(currentNode);
        return;
    }
    
    //
    // Check performance and identify do we need a sorting here. 
    // Probably sorting algorithm can be helpfull to render on Mac / Windows, but will be useless for iOS and Android
    //
    
    {
        if (currentNode->childs)
        {
            for (int32 index = 0; index < 4; ++index)
            {
                QuadTreeNode<LandscapeQuad> * child = &currentNode->childs[index];
                Draw(child); 
            }
        }
    }
}

void LandscapeNode::Draw()
{
    RenderManager::Instance()->SetRenderEffect(RenderManager::TEXTURE_MUL_FLAT_COLOR);
    if (textures[TEXTURE_BASE])
        RenderManager::Instance()->SetTexture(textures[TEXTURE_BASE]);
    
    RenderManager::Instance()->ResetColor();
    
#if defined(__DAVAENGINE_MACOS__)
    if (debugFlags & DEBUG_DRAW_ALL)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }   
#endif

    fans.clear();
    Draw(&quadTreeHead);
    DrawFans();
    
    RenderManager::Instance()->RestoreRenderEffect();
    
#if defined(__DAVAENGINE_MACOS__)
    if (debugFlags & DEBUG_DRAW_ALL)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
#endif
}
    
    
};




