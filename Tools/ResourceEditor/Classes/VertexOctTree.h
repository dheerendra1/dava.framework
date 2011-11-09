/*
 *  GameCore.h
 *  TemplateProjectMacOS
 *
 *  Created by Vitaliy  Borodovsky on 3/19/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef __DAVAENGINE_VERTEX_OCT_TREE_H__
#define __DAVAENGINE_VERTEX_OCT_TREE_H__

#include "Base/BaseTypes.h"
#include "Base/BaseMath.h"
#include "Base/DynamicObjectCache.h"

namespace DAVA 
{

/*
    \brief Helper class to eleminate unique vertices from given set.
    Main purpose is a fast search if vertex already exists or not. 
 */
template<class VERTEX_TYPE>
class VertexOctTree
{
public:
    class Node
    {
        Vector3     position;
        AABBox3     box;
        //VERTEX_TYPE vertex;
        Node *      childs[4];
        Vector<int> vertexArray;
    };
    
    VertexOctTree()
    {
        headNode = cache.New();
    }
    ~VertexOctTree()
    {
        cache.Delete(headNode);
    };
    
    void BuildTree(int32 vertexCount, Vector3 * baseVertexPtr, int32 stride, VERTEX_TYPE * array)
    {
        headNode->box.Clear();
        uint8 * vertexPtr = (uint8*)baseVertexPtr;
        for (int32 k = 0; k < vertexCount; ++k)
        {
            Vector3 * vertex = (Vector3*)vertexPtr;
            headNode->box.AddPoint(*vertex);
            vertexPtr += stride;
        }
        
        headNode->vertexArray.resize(vertexCount);
        for (int32 i = 0; i < vertexCount; ++i)
            headNode->vertexArray[i] = i;
        
        RecursiveBuild(headNode, array);
    }
    
private:
    DynamicObjectCache<Node> cache;
    Node * headNode;
    
    void RecursiveBuild(Node * headNode, VERTEX_TYPE * array)
    {
        if (headNode.vertexArray.size() > 10)
        {
            Vector3 min = headNode->box.min;
            Vector3 mid = headNode->box.GetCenter();
            Vector3 size = (headNode->box.max - headNode->box.min) / 2;
            
            
            Vector3 minPoses[8] = 
            {
                Vector3(min.x, min.y, min.z),
                Vector3(min.x, min.y, min.z + size.z),
                Vector3(min.x, min.y + size.y, min.z),
                Vector3(min.x + size.x, min.y, min.z),
                Vector3(min.x + size.x, min.y + size.y, min.z),
                Vector3(min.x + size.x, min.y, min.z + size.z),
                Vector3(min.x, min.y + size.y, min.z + size.z),
                Vector3(min.x + size.x, min.y + size.y, min.z + size.z),
            };
            for (int32 k = 0; k < 8; ++k)
            {
                AABBox3 childBox(minPoses[k], minPoses[k] + size);
                int32 size = headNode.vertexArray.size();
                //for (
                
            
            }
        }
    }
};
                      


};

#endif // __DAVAENGINE_VERTEX_OCT_TREE_H__