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
#include "Scene3D/SceneNode.h"
#include "Scene3D/Scene.h"
#include "Scene3D/SceneNodeAnimation.h"
#include "Scene3D/SceneNodeAnimationList.h"

namespace DAVA
{
	
SceneNode::SceneNode(Scene * _scene)
	: scene(_scene)
	, parent(0)
    , visible(true)
    , debugFlags(DEBUG_DRAW_NONE)
{
	localTransform.Identity();
	worldTransform.Identity();
	//animation = 0;
    visible = true;
    
    debugFlags = DEBUG_DRAW_NONE;
}

SceneNode::~SceneNode()
{

}

void SceneNode::SetParent(SceneNode * node)
{
	parent = node;
}

void SceneNode::AddNode(SceneNode * node)
{
	if (node)node->Retain();
	
	childs.push_back(node);
	node->SetParent(this);
}

void SceneNode::RemoveNode(SceneNode * node)
{
	for (std::deque<SceneNode*>::iterator t = childs.begin(); t != childs.end(); ++t)
	{
		if (*t == node)
		{
			childs.erase(t);
			break;
		}
	}
	
	if (node)
	{
		node->SetParent(0);
		node->Release();
	}
}
	
SceneNode * SceneNode::GetChild(int32 index)
{
	return childs[index];
}

int32 SceneNode::GetChildrenCount()
{
    return childs.size();
}

void SceneNode::RemoveAllChilds()
{
	childs.clear();
}


SceneNode *	SceneNode::FindByName(const String & searchName)
{
	if (name == searchName)
		return this;
	
	uint32 size = (uint32)childs.size();
	for (uint32 c = 0; c < size; ++c)
	{
		SceneNode * res = childs[c]->FindByName(searchName);
		if (res != 0)return res;
	}
	return 0;
}

	
void SceneNode::ExecuteAnimation(SceneNodeAnimation * _animation)
{
	nodeAnimations.push_back(_animation);
//	printf("-- add animation: %d node: %s anim: %s\n", nodeAnimations.size(), name.c_str(), _animation->GetParent()->name.c_str()); 
//	if (_animation->GetParent()->name == "a1")
//	{
//		int k = 0;
//		k++;
//	}
}
	
void SceneNode::DetachAnimation(SceneNodeAnimation * animation)
{
//	int32 size = nodeAnimations.size();
	for (std::deque<SceneNodeAnimation*>::iterator t = nodeAnimations.begin(); t != nodeAnimations.end(); ++t)
	{
		if (*t == animation)
		{
			nodeAnimations.erase(t);
			break;
		}
	}
//	int32 sizeAfter = nodeAnimations.size();
//	if (sizeAfter != size - 1)
//	{
//		printf("******** Error with animation detach");
//	}	
}

void SceneNode::StopAllAnimations(bool recursive)
{
	nodeAnimations.clear();
	if (recursive)
	{
		uint32 size = (uint32)childs.size();
		for (uint32 c = 0; c < size; ++c)
			childs[c]->StopAllAnimations(recursive);
	}	
}

void SceneNode::RestoreOriginalTransforms()
{
	localTransform = originalLocalTransform;
	
	uint32 size = (uint32)childs.size();
	for (uint32 c = 0; c < size; ++c)
		childs[c]->RestoreOriginalTransforms();
}
	
void SceneNode::ExtractCurrentNodeKeyForAnimation(SceneNodeAnimationKey & key)
{
	key.time = 0.0f;
	key.translation.x = localTransform._30;
	key.translation.y = localTransform._31;
	key.translation.z = localTransform._32;
	key.rotation.Construct(localTransform);
	//key.matrix = localTransform;
}

    
void SceneNode::Update(float32 timeElapsed)
{
	// TODO - move node update to render because any of objects can change params of other objects
	if (nodeAnimations.size() != 0)
	{
		Quaternion blendedRotation;
		Vector3 blendedTranslation;
		float32 accumWeight = 0.0f;
		std::deque<SceneNodeAnimation*>::const_iterator end = nodeAnimations.end();
		for (std::deque<SceneNodeAnimation*>::iterator it = nodeAnimations.begin(); it != end; ++it)
		{
			SceneNodeAnimation * animation = *it;
			SceneNodeAnimationKey & key = animation->Intepolate(animation->GetCurrentTime());
			if (accumWeight == 0.0f)
			{
				blendedTranslation = key.translation;
				blendedRotation = key.rotation;
				accumWeight = animation->weight;
			}else
			{
				float32 factor = animation->weight / (accumWeight + animation->weight);
				accumWeight += accumWeight;
				blendedTranslation.Lerp(blendedTranslation, key.translation, factor);
				blendedRotation.Slerp(blendedRotation, key.rotation, factor);
			}
			//key.GetMatrix(localTransform);
		}
		Matrix4 localTransformTrans;
		Matrix4 localTransformRot;
		Matrix4 localTransformFinal;
		localTransformTrans.CreateTranslation(blendedTranslation);
		localTransformRot = blendedRotation.GetMatrix();
		
		localTransform = localTransformRot * localTransformTrans;
		
//		if (nodeAnimations.size() != 1)
//		{
//			printf("-- blended node: %s\n", name.c_str());
//			std::deque<SceneNodeAnimation*>::const_iterator end = nodeAnimations.end();
//			for (std::deque<SceneNodeAnimation*>::iterator it = nodeAnimations.begin(); it != end; ++it)
//			{
//				SceneNodeAnimation * animation = *it;
//				printf(">>> blend: %s wei: %f inDelay: %f\n", animation->GetParent()->name.c_str(), animation->weight, animation->delayTime);
//			}
//		}
	}
	
	{
		if (parent)
			worldTransform = localTransform * parent->worldTransform;
		else 
			worldTransform = localTransform;
	}
	//printf("- node: %s tr: %f %f %f\n", name.c_str(), localTransform.data[12], localTransform.data[13], localTransform.data[14]); 
	
	uint32 size = (uint32)childs.size();
	for (uint32 c = 0; c < size; ++c)
		childs[c]->Update(timeElapsed);
	
}

void SceneNode::Draw()
{
	uint32 size = (uint32)childs.size();
	for (uint32 c = 0; c < size; ++c)
		childs[c]->Draw();
}

    
SceneNode* SceneNode::Clone(SceneNode *dstNode)
{
    if (!dstNode) 
    {
        dstNode = new SceneNode(scene);
    }
    dstNode->visible = visible;
    dstNode->originalLocalTransform = originalLocalTransform;
    
    dstNode->localTransform = localTransform;
    dstNode->worldTransform = worldTransform;
    dstNode->name = name;
    dstNode->tag = tag;
    dstNode->debugFlags = debugFlags;

//    Logger::Debug("Node %s clonned", name.c_str());
    
    dstNode->nodeAnimations = nodeAnimations;
    
    
//    Logger::Debug("Children +++++++++++++++++++++++++++++++");
    std::deque<SceneNode*>::iterator it = childs.begin();
    for(; it != childs.end(); it++)
    {
        SceneNode *n = (*it)->Clone();
        dstNode->AddNode(n);
        n->Release();
    }
//    Logger::Debug("Children -------------------------------");
    
    return dstNode;
}

void SceneNode::SetDebugFlags(uint32 _debugFlags, bool isRecursive)
{
    debugFlags = _debugFlags;
    if (isRecursive)
    {
        std::deque<SceneNode*>::iterator it = childs.begin();
        for(; it != childs.end(); it++)
        {
            SceneNode *n = (*it);
            n->SetDebugFlags(_debugFlags, isRecursive);
        }
    }
}

    
};




