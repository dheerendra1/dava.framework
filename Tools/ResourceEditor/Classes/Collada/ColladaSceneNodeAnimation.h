/*
 *  ColladaSceneNodeAnimation.h
 *  ColladaConverter
 *
 *  Created by Vitaliy  Borodovsky on 5/10/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __COLLADALOADER_COLLADA_SCENE_NODE_ANIMATION_H__
#define __COLLADALOADER_COLLADA_SCENE_NODE_ANIMATION_H__

#include "ColladaIncludes.h"
#include "ColladaMeshInstance.h"
#include "ColladaLight.h"

namespace DAVA
{

class ColladaSceneNodeAnimation
{
public:
	struct Key
	{
		float32		time;
		Quaternion	rotation;	
		Vector3		translation;
	};
	
	
};
};

#endif // __COLLADALOADER_COLLADA_SCENE_NODE_ANIMATION_H__
