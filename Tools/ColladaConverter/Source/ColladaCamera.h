#ifndef __COLLADALOADER_COLLADACAMERA_H__
#define __COLLADALOADER_COLLADACAMERA_H__

#include "ColladaIncludes.h"
#include "Base/BaseMath.h"


namespace DAVA
{

class ColladaCamera
{
public:
	ColladaCamera(FCDCamera * cam);
	~ColladaCamera();
	
	void Set();


	//CameraState camera;
	FCDCamera * camera;
};

};

#endif // __COLLADALOADER_COLLADACAMERA_H__
