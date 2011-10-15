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
#include "Render/Shader.h"

namespace DAVA
{
	
LandscapeNode::LandscapeNode(Scene * _scene)
	: SceneNode(_scene)
    , heightmap(0)
    , indices(0)
{
    for (int32 t = 0; t < TEXTURE_COUNT; ++t)
        textures[t] = 0;
    
    frustum = 0; //new Frustum();
    renderingMode = RENDERING_MODE_TEXTURE;
    
    activeShader = 0;
    singleTextureShader = 0;
    detailShader = 0;
    blendedShader = 0;
}

LandscapeNode::~LandscapeNode()
{
    ReleaseShaders();
    ReleaseAllRDOQuads();
    
    for (int32 t = 0; t < TEXTURE_COUNT; ++t)
    {
        SafeRelease(textures[t]);
    }
    SafeDeleteArray(indices);
    SafeRelease(heightmap);
}
    
void LandscapeNode::InitShaders()
{
    switch (renderingMode)
    {
        case RENDERING_MODE_TEXTURE:
            break;
        case RENDERING_MODE_DETAIL_SHADER:
            detailShader = new Shader();
            detailShader->LoadFromYaml("~res:/Shaders/Landscape/detail-texture.shader");
            
            uniformTexture = detailShader->FindUniformLocationByName("texture");
            uniformDetailTexture = detailShader->FindUniformLocationByName("detailTexture");
            uniformCameraPosition = detailShader->FindUniformLocationByName("cameraPosition");

            activeShader = detailShader;
            break;
        case RENDERING_MODE_BLENDED_SHADER:
            blendedShader = new Shader();
            blendedShader->LoadFromYaml("~res:/Shaders/Landscape/blended-texture.shader");
            
            uniformTexture0 = blendedShader->FindUniformLocationByName("texture0");
            uniformTexture1 = blendedShader->FindUniformLocationByName("texture1");
            uniformTextureMask = blendedShader->FindUniformLocationByName("textureMask");
            
            activeShader = blendedShader;
            break;
    }
}
    
void LandscapeNode::ReleaseShaders()
{
    SafeRelease(blendedShader);
    SafeRelease(singleTextureShader);
    SafeRelease(detailShader);
    activeShader = 0;
}


int8 LandscapeNode::AllocateRDOQuad(LandscapeQuad * quad)
{
    Logger::Debug("AllocateRDOQuad: %d %d size: %d", quad->x, quad->y, quad->size);
    DVASSERT(quad->size == RENDER_QUAD_WIDTH - 1);
    LandscapeVertex * landscapeVertices = new LandscapeVertex[(quad->size + 1) * (quad->size + 1)];
    
    int32 index = 0;
    for (int32 y = quad->y; y < quad->y + quad->size + 1; ++y)
        for (int32 x = quad->x; x < quad->x + quad->size + 1; ++x)
        {
            landscapeVertices[index].position = GetPoint(x, y, heightmap->GetData()[y * heightmap->GetWidth() + x]);
            landscapeVertices[index].texCoord = Vector2((float32)x / (float32)(heightmap->GetWidth() - 1), (float32)y / (float32)(heightmap->GetHeight() - 1));           
//            landscapeVertices[index].texCoord *= 10.0f;
            //Logger::Debug("AllocateRDOQuad: %d pos(%f, %f)", index, landscapeVertices[index].position.x, landscapeVertices[index].position.y);
            index++;
        }
    
    // setup a base RDO
    RenderDataObject * landscapeRDO = new RenderDataObject();
    landscapeRDO->SetStream(EVF_VERTEX, TYPE_FLOAT, 3, sizeof(LandscapeVertex), &landscapeVertices[0].position); 
    landscapeRDO->SetStream(EVF_TEXCOORD0, TYPE_FLOAT, 2, sizeof(LandscapeVertex), &landscapeVertices[0].texCoord); 
    landscapeRDO->BuildVertexBuffer((quad->size + 1) * (quad->size + 1));
    SafeDeleteArray(landscapeVertices);
    
    landscapeVerticesArray.push_back(landscapeVertices);
    
    landscapeRDOArray.push_back(landscapeRDO);
    
    Logger::Debug("Allocated vertices: %d KB", sizeof(LandscapeVertex) * (quad->size + 1) * (quad->size + 1) / 1024);
    
    return (int8)landscapeRDOArray.size() - 1;
}

void LandscapeNode::ReleaseAllRDOQuads()
{
    for (size_t k = 0; k < landscapeRDOArray.size(); ++k)
    {
        SafeRelease(landscapeRDOArray[k]);
        SafeDeleteArray(landscapeVerticesArray[k]);
    }
    landscapeRDOArray.clear();
    landscapeVerticesArray.clear();
}

void LandscapeNode::SetLods(const Vector4 & lods)
{
    
}

void LandscapeNode::BuildLandscapeFromHeightmapImage(eRenderingMode _renderingMode, const String & heightmapPathname, const AABBox3 & _box)
{
    renderingMode = _renderingMode;
    ReleaseShaders(); // release previous shaders
    InitShaders(); // init new shaders according to the selected rendering mode
    
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
    //quadTreeHead.data.xbuf = quadTreeHead.data.ybuf = 0;
    quadTreeHead.data.size = heightmap->GetWidth() - 1; 
    quadTreeHead.data.rdoQuad = -1;
    
    lodLevelsCount = 4;
//    lodDistance[0] = 400;
//    lodDistance[1] = 800;
//    lodDistance[2] = 1500;
//    lodDistance[3] = 3000;
    
    lodDistance[0] = 60;
    lodDistance[1] = 120;
    lodDistance[2] = 240;
    lodDistance[3] = 480;
    
    
    for (int32 ll = 0; ll < lodLevelsCount; ++ll)
        lodSqDistance[ll] = lodDistance[ll] * lodDistance[ll];
    
    allocatedMemoryForQuads = 0;
    RecursiveBuild(&quadTreeHead, 0, lodLevelsCount);
    FindNeighbours(&quadTreeHead);
    
    indices = new uint16[RENDER_QUAD_WIDTH * RENDER_QUAD_WIDTH * 6];
    
    Logger::Debug("Allocated indices: %d KB", RENDER_QUAD_WIDTH * RENDER_QUAD_WIDTH * 6 * 2 / 1024);
    Logger::Debug("Allocated memory for quads: %d KB", allocatedMemoryForQuads / 1024);
    Logger::Debug("sizeof(LandscapeQuad): %d bytes", sizeof(LandscapeQuad));
    Logger::Debug("sizeof(QuadTreeNode): %d bytes", sizeof(QuadTreeNode<LandscapeQuad>));
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
    res.y = (box.min.y + (float32)y / (float32)(heightmap->GetHeight() - 1) * (box.max.y - box.min.y));
    res.z = (box.min.z + ((float32)height / 255.0f) * (box.max.z - box.min.z));
    return res;
};

void LandscapeNode::RecursiveBuild(QuadTreeNode<LandscapeQuad> * currentNode, int32 level, int32 maxLevels)
{
    allocatedMemoryForQuads += sizeof(QuadTreeNode<LandscapeQuad>);
    currentNode->data.lod = level;
    
    // if we have parrent get rdo quad 
    if (currentNode->parent)
    {
        currentNode->data.rdoQuad = currentNode->parent->data.rdoQuad;
    }
    
    if ((currentNode->data.rdoQuad == -1) && (currentNode->data.size == RENDER_QUAD_WIDTH - 1))
    {
        currentNode->data.rdoQuad = AllocateRDOQuad(&currentNode->data);
        //currentNode->data.xbuf = 0;
        //currentNode->data.ybuf = 0;
    }
    
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
    
    //int16 bufMinIndexX = currentNode->data.xbuf;
    //int16 bufMinIndexY = currentNode->data.ybuf;
    
    int16 size = currentNode->data.size;
    
    // We should be able to divide landscape by 2 here
    DVASSERT((size & 1) == 0);

    QuadTreeNode<LandscapeQuad> * child0 = &currentNode->childs[0];
    child0->data.x = minIndexX;
    child0->data.y = minIndexY;
    //child0->data.xbuf = bufMinIndexX;
    //child0->data.ybuf = bufMinIndexY;
    child0->data.size = size / 2;
    
    QuadTreeNode<LandscapeQuad> * child1 = &currentNode->childs[1];
    child1->data.x = minIndexX + size / 2;
    child1->data.y = minIndexY;
    //child1->data.xbuf = bufMinIndexX + size / 2;
    //child1->data.ybuf = bufMinIndexY;
    child1->data.size = size / 2;

    QuadTreeNode<LandscapeQuad> * child2 = &currentNode->childs[2];
    child2->data.x = minIndexX;
    child2->data.y = minIndexY + size / 2;
    //child2->data.xbuf = bufMinIndexX;
    //child2->data.ybuf = bufMinIndexY + size / 2;
    child2->data.size = size / 2;

    QuadTreeNode<LandscapeQuad> * child3 = &currentNode->childs[3];
    child3->data.x = minIndexX + size / 2;
    child3->data.y = minIndexY + size / 2;
    //child3->data.xbuf = bufMinIndexX + size / 2;
    //child3->data.ybuf = bufMinIndexY + size / 2;
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
    
void LandscapeNode::SetTexture(eTextureLevel level, const String & textureName)
{
    Texture * texture = Texture::CreateFromFile(textureName);
    texture->GenerateMipmaps();
    texture->SetWrapMode(Texture::WRAP_REPEAT, Texture::WRAP_REPEAT);

    SafeRelease(textures[level]);
    textures[level] = texture;
}
    
void LandscapeNode::SetTexture(eTextureLevel level, Texture * texture)
{
    texture->GenerateMipmaps();
    texture->SetWrapMode(Texture::WRAP_REPEAT, Texture::WRAP_REPEAT);
    SafeRelease(textures[level]);
    textures[level] = SafeRetain(texture);
}

void LandscapeNode::DrawQuad(QuadTreeNode<LandscapeQuad> * currentNode, int8 lod)
{
    int32 depth = currentNode->data.size / (1 << lod);
    if (depth == 1)
    {
        currentNode->parent->data.frame = Core::Instance()->GetGlobalFrameIndex();
    }else
    {
        //int32 newdepth = (int)(logf((float)depth) / logf(2.0f) + 0.5f);
        int32 newdepth2 = CountLeadingZeros(depth);
        //Logger::Debug("dp: %d %d %d", depth, newdepth, newdepth2);
        //DVASSERT(newdepth == newdepth2); // Check of math, we should use optimized version with depth2
        
        MarkFrames(currentNode, newdepth2);
    }
    
    uint16 * drawIndices = indices;
    int32 step = (1 << lod);
    
    //int16 width = heightmap->GetWidth();
    int32 cnt = 0;
    for (uint16 y = (currentNode->data.y & RENDER_QUAD_AND); y < (currentNode->data.y & RENDER_QUAD_AND) + currentNode->data.size; y += step)
        for (uint16 x = (currentNode->data.x & RENDER_QUAD_AND); x < (currentNode->data.x & RENDER_QUAD_AND) + currentNode->data.size; x += step)
        {
            *drawIndices++ = x + y * RENDER_QUAD_WIDTH;
            *drawIndices++ = (x + step) + y * RENDER_QUAD_WIDTH;
            *drawIndices++ = x + (y + step) * RENDER_QUAD_WIDTH;
            
            *drawIndices++ = (x + step) + y * RENDER_QUAD_WIDTH;
            *drawIndices++ = (x + step) + (y + step) * RENDER_QUAD_WIDTH;
            *drawIndices++ = x + (y + step) * RENDER_QUAD_WIDTH;     
 
            cnt += 6;
        }
    RenderManager::Instance()->SetRenderData(landscapeRDOArray[currentNode->data.rdoQuad]);
    RenderManager::Instance()->FlushState();
    RenderManager::Instance()->HWDrawElements(PRIMITIVETYPE_TRIANGLELIST, cnt, EIF_16, indices); 
}
    
void LandscapeNode::DrawFans()
{
    uint32 currentFrame = Core::Instance()->GetGlobalFrameIndex();;
    int16 width = RENDER_QUAD_WIDTH;//heightmap->GetWidth();
    
    List<QuadTreeNode<LandscapeQuad>*>::const_iterator end = fans.end();
    for (List<QuadTreeNode<LandscapeQuad>*>::iterator t = fans.begin(); t != end; ++t)
    {
        uint16 * drawIndices = indices;
        QuadTreeNode<LandscapeQuad>* node = *t;
        
        RenderManager::Instance()->SetRenderData(landscapeRDOArray[node->data.rdoQuad]);

        
        int32 count = 0;
        int16 halfSize = (node->data.size >> 1);
        int16 xbuf = node->data.x & RENDER_QUAD_AND;
        int16 ybuf = node->data.y & RENDER_QUAD_AND;
        
        drawIndices[count++] = (xbuf + halfSize) + (ybuf + halfSize) * width;
        drawIndices[count++] = (xbuf) + (ybuf) * width;
        
        if ((node->neighbours[TOP]) && (node->neighbours[TOP]->data.frame == currentFrame))
            drawIndices[count++] = (xbuf + halfSize) + (ybuf) * width;
        
        drawIndices[count++] = (xbuf + node->data.size) + (ybuf) * width;
        
        if ((node->neighbours[RIGHT]) && (node->neighbours[RIGHT]->data.frame == currentFrame))
            drawIndices[count++] = (xbuf + node->data.size) + (ybuf + halfSize) * width;
            
        drawIndices[count++] = (xbuf + node->data.size) + (ybuf + node->data.size) * width;
        
        if ((node->neighbours[BOTTOM]) && (node->neighbours[BOTTOM]->data.frame == currentFrame))
            drawIndices[count++] = (xbuf + halfSize) + (ybuf + node->data.size) * width;

        drawIndices[count++] = (xbuf) + (ybuf + node->data.size) * width;
        
        if ((node->neighbours[LEFT]) && (node->neighbours[LEFT]->data.frame == currentFrame))
            drawIndices[count++] = (xbuf) + (ybuf + halfSize) * width;

        drawIndices[count++] = (xbuf) + (ybuf) * width;
        
        //RenderManager::Instance()->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
        RenderManager::Instance()->FlushState();
        RenderManager::Instance()->HWDrawElements(PRIMITIVETYPE_TRIANGLEFAN, count, EIF_16, indices); 
    }
}
    
void LandscapeNode::Draw(QuadTreeNode<LandscapeQuad> * currentNode)
{
    //Frustum * frustum = scene->GetClipCamera()->GetFrustum();
    if (!frustum->IsInside(currentNode->data.bbox))return;

    /*
        If current quad do not have geometry just traverse it childs. 
        Magic starts when we have a geometry
     */
    if (currentNode->data.rdoQuad == -1)
    {
        if (currentNode->childs)
        {
            for (int32 index = 0; index < 4; ++index)
            {
                QuadTreeNode<LandscapeQuad> * child = &currentNode->childs[index];
                Draw(child); 
            }
        }
        return;
    }
    /*
        // UNCOMMENT THIS TO CHECK GEOMETRY WITH 0 LEVEL OF DETAIL
        else
        {
            DrawQuad(currentNode, 0);
            return;
        }
     */
    
    /*
        We can be here only if we have a geometry in the node. 
        Here we use Geomipmaps rendering algorithm. 
        These quads are 129x129.
     */
//    Camera * cam = clipCamera;
    
    Vector3 corners[8];
    currentNode->data.bbox.GetCorners(corners);
    
    float32 minDist =  100000000.0f;
    float32 maxDist = -100000000.0f;
    for (int32 k = 0; k < 8; ++k)
    {
        Vector3 v = cameraPos - corners[k];
        float32 dist = v.SquareLength();
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
        /* EXPERIMENTAL => reduce level of quadtree, results was not successfull 
         else
        {
            DrawQuad(currentNode, maxLod);  
        }*/
    }
}

void LandscapeNode::Draw()
{
    //glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
#if defined(__DAVAENGINE_MACOS__)
    if (debugFlags & DEBUG_DRAW_ALL)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
#endif
    
    RenderManager::Instance()->ResetColor();

    
    
/*
    Boroda: I do not understand why, but this code breaks frustrum culling on landscape.
    I've spent an hour, trying to understand what's going on, without luck. 
 */
    
//	Matrix4 prevMatrix = RenderManager::Instance()->GetMatrix(RenderManager::MATRIX_MODELVIEW);
//	Matrix4 meshFinalMatrix = worldTransform * prevMatrix;
//    //frustum->Set(meshFinalMatrix * RenderManager::Instance()->GetMatrix(RenderManager::MATRIX_PROJECTION));
//    cameraPos = scene->GetClipCamera()->GetPosition() * worldTransform;
//
//    RenderManager::Instance()->SetMatrix(RenderManager::MATRIX_MODELVIEW, meshFinalMatrix);
//    frustum->Set();

    frustum = scene->GetClipCamera()->GetFrustum();
    cameraPos = scene->GetClipCamera()->GetPosition();
    
    fans.clear();
    
    switch(renderingMode)
    {
        case RENDERING_MODE_TEXTURE:
            RenderManager::Instance()->SetRenderEffect(RenderManager::TEXTURE_MUL_FLAT_COLOR);
            if (textures[TEXTURE_TEXTURE0])
                RenderManager::Instance()->SetTexture(textures[TEXTURE_TEXTURE0], 0);
        break;
            
        case RENDERING_MODE_DETAIL_SHADER:
        {
            if (textures[TEXTURE_TEXTURE0])
                RenderManager::Instance()->SetTexture(textures[TEXTURE_TEXTURE0], 0);
            if (textures[TEXTURE_DETAIL])
                RenderManager::Instance()->SetTexture(textures[TEXTURE_DETAIL], 1);

            RenderManager::Instance()->SetShader(detailShader);
            RenderManager::Instance()->FlushState();

            detailShader->SetUniformValue(uniformTexture, 0);
            detailShader->SetUniformValue(uniformDetailTexture, 1);
            detailShader->SetUniformValue(uniformCameraPosition, cameraPos);
        }
        break;
        case RENDERING_MODE_BLENDED_SHADER:
        {
            if (textures[TEXTURE_TEXTURE0])
                RenderManager::Instance()->SetTexture(textures[TEXTURE_TEXTURE0], 0);
            if (textures[TEXTURE_TEXTURE1])
                RenderManager::Instance()->SetTexture(textures[TEXTURE_TEXTURE1], 1);
            if (textures[TEXTURE_TEXTUREMASK])
                RenderManager::Instance()->SetTexture(textures[TEXTURE_TEXTUREMASK], 2);

            RenderManager::Instance()->SetShader(blendedShader);
            RenderManager::Instance()->FlushState();
            blendedShader->SetUniformValue(uniformTexture0, 0);
            blendedShader->SetUniformValue(uniformTexture1, 1);
            blendedShader->SetUniformValue(uniformTextureMask, 2);
            blendedShader->SetUniformValue(uniformCameraPosition, cameraPos);    
        }            
        break;
    }
    
    Draw(&quadTreeHead);
    DrawFans();
    
    
    //glDisable(GL_CULL_FACE);
#if defined(__DAVAENGINE_MACOS__)
    if (debugFlags & DEBUG_DRAW_ALL)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }   
#endif 
    
    switch(renderingMode)
    {
        case RENDERING_MODE_TEXTURE:
            break;
            
        case RENDERING_MODE_DETAIL_SHADER:
        {
            RenderManager::Instance()->SetTexture(0, 1);
        }
            break;
        case RENDERING_MODE_BLENDED_SHADER:
        {
            RenderManager::Instance()->SetTexture(0, 1);
            RenderManager::Instance()->SetTexture(0, 2);
        }            
            break;
    }


//#if defined(__DAVAENGINE_MACOS__)
//    if (debugFlags & DEBUG_DRAW_ALL)
//    {
//        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//        glEnable(GL_POLYGON_OFFSET_LINE);
//        glPolygonOffset(0.0f, 1.0f);
//        RenderManager::Instance()->SetRenderEffect(RenderManager::FLAT_COLOR);
//        fans.clear();
//        Draw(&quadTreeHead);
//        DrawFans();
//        glDisable(GL_POLYGON_OFFSET_LINE);
//        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//    }   
//#endif
    
    //RenderManager::Instance()->SetMatrix(RenderManager::MATRIX_MODELVIEW, prevMatrix);

}
    
    
};




