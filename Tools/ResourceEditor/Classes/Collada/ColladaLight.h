#ifndef __COLLADALOADER_COLLADALIGHT_H__
#define __COLLADALOADER_COLLADALIGHT_H__

#include "ColladaIncludes.h"

namespace DAVA
{

struct ColladaLightState
{
	ColladaLightState()
	{
		lightIndex = 0;
		globalAmbientalLight[0] = 0.0f;
		globalAmbientalLight[1] = 0.0f;
		globalAmbientalLight[2] = 0.0f;
		globalAmbientalLight[3] = 1.0f;
	}

	int lightIndex;
	GLfloat globalAmbientalLight[4];
};


class ColladaLight
{
public:
	ColladaLight(FCDLight * _light);
	~ColladaLight();
	
	void ApplyLight(ColladaLightState & state);

	enum eType
	{
		AMBIENT = 0,
		SPOT,
		DIRECTIONAL,
		POINT,
	};

	FCDLight * light;

	eType		type;
	Vector4		color;				// for all lights
	Vector4		direction;			// for directional
	Vector4		attenuation;		// for point 
};

};

#endif // __COLLADALOADER_COLLADALIGHT_H__
