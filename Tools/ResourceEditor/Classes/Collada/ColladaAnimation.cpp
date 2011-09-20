#include "StdAfx.h"
#include "ColladaAnimation.h"


namespace DAVA 
{

ColladaAnimation::ColladaAnimation()
{
	
}
	
ColladaAnimation::~ColladaAnimation()
{
	
}
	
void ColladaAnimation::Assign()
{
	for (Map<ColladaSceneNode*, SceneNodeAnimation*>::iterator it = animations.begin(); it != animations.end(); ++it)
	{
		ColladaSceneNode * node = it->first;
		SceneNodeAnimation * anim = it->second;
		node->SetAnimation(anim);
	}
}

};