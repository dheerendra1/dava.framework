#ifndef __DAVAENGINE_COLLADAANIMATION_H__
#define __DAVAENGINE_COLLADAANIMATION_H__

#include "ColladaIncludes.h"
#include "ColladaSceneNode.h"

namespace DAVA
{
	
class ColladaAnimation
{
public:
	ColladaAnimation();
	~ColladaAnimation();
	
	std::string name;
	float32 duration;
	Map<ColladaSceneNode*, SceneNodeAnimation*> animations;
	
	void Assign();
};
};
					 
#endif // __DAVAENGINE_COLLADAANIMATION_H__
