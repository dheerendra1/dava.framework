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
#ifndef __DAVAENGINE_SCENENODE_H__
#define __DAVAENGINE_SCENENODE_H__

#include "Base/BaseObject.h"
#include "Base/BaseTypes.h"
#include "Base/BaseMath.h"
#include "Render/RenderBase.h"
#include "Scene3D/SceneNodeAnimationKey.h"
#include <deque>

namespace DAVA
{

class Scene;
class SceneNodeAnimation;
class SceneNodeAnimationKey;
class SceneNode : public BaseObject
{
public:	
	SceneNode(Scene * scene);
	virtual ~SceneNode();
	
	// working with childs
	virtual void	AddNode(SceneNode * node);
	virtual void	RemoveNode(SceneNode * node);
	virtual SceneNode * GetChild(int32 index);
	virtual int32 GetChildrenCount();
	virtual void	RemoveAllChilds();
	virtual SceneNode *	FindByName(const String & name);
	
	// virtual updates
	virtual void	Update(float32 timeElapsed);
	virtual void	Draw();
	
	// properties
	inline void SetVisible(bool isVisible);
	inline SceneNode * GetParent();
	
	// extract data from current node to use it in animations
	void ExtractCurrentNodeKeyForAnimation(SceneNodeAnimationKey & resultKey);

	
	Matrix4 localTransform;
	Matrix4 worldTransform;
	String	name;
	int32	tag;
	
	
	
	// animations 
	void ExecuteAnimation(SceneNodeAnimation * animation);	
	void DetachAnimation(SceneNodeAnimation * animation);
	virtual void StopAllAnimations(bool recursive = true);
	void RestoreOriginalTransforms();

	
	std::deque<SceneNodeAnimation *> nodeAnimations;
	
	
	Matrix4 originalLocalTransform;
    
    virtual SceneNode* Clone(SceneNode *dstNode = NULL);
//    virtual SceneNode* Clone();

    
    
	enum
	{
		DEBUG_DRAW_NONE = 0,
		DEBUG_DRAW_AABBOX = 1,              
		DEBUG_DRAW_LOCAL_AXIS = 2,
        DEBUG_DRAW_ALL = 0xFFFFFFFF,
	};
	/**
        \brief function to enable or disable debug drawing for particular node.
        By default it's not recursive. Some objects may support flags only partially.
        For example if node do not have bounding box flag DEBUG_DRAW_AABBOX will not produce any output
        These flags are mostly for debug purposes and we do not guarantee that logic of the debug rendering will remain unchanged between 
        framework versions.
     
        \param[in] debugFlags flags to be set
        \param[in] isRecursive do you want to set flags recursively
     
     */
	void SetDebugFlags(uint32 debugFlags, bool isRecursive = false);        
	
protected:
    
//    virtual SceneNode* CopyDataTo(SceneNode *dstNode);
	void SetParent(SceneNode * node);
	
	Scene * scene;
	SceneNode * parent;
	std::deque<SceneNode*> childs;
	bool visible;
    uint32 debugFlags;
};

inline void SceneNode::SetVisible(bool isVisible)
{
	visible = isVisible;
}
	
inline SceneNode * SceneNode::GetParent()
{
	return parent;
}
	
};

#endif // __DAVAENGINE_SCENENODE_H__





