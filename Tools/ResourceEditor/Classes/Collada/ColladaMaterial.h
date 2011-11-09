#ifndef __COLLADALOADER_COLLADAMATERIAL_H__
#define __COLLADALOADER_COLLADAMATERIAL_H__

#include "ColladaIncludes.h"
#include "ColladaTexture.h"


namespace DAVA
{

class ColladaScene;

class ColladaMaterial
{
public:
	ColladaMaterial(ColladaScene * scene, FCDMaterial * _material);
	~ColladaMaterial();
		
	void SetDefaultMaterial();

	static ColladaMaterial * defaultMaterial;
	static ColladaMaterial * GetDefaultMaterial();

	FCDMaterial * material;

	Vector4 ambient;
	Vector4 diffuse;
	Vector4 specular;
	Vector4 emission;
	
	float	shininess;
	Vector4 reflective;
	float	reflectivity;
	
	Vector4 transparent;
	float	transparency; // use m_is_transparency_one_opaque to figure out
	float	indexOfRefraction;

	ColladaTexture* diffuseTexture;
	fm::string		diffuseTextureName;
	bool			hasDiffuseTexture;

	// pointer to texture reflective
	ColladaTexture* reflectiveTexture;
	fm::string		reflectiveTextureName;
	bool			hasReflectiveTexture;

    ColladaTexture* lightmapTexture;
	fm::string		lightmapTextureName;
	bool			hasLightmapTexture;

	// pointer to texture transparent
	ColladaTexture* transparentTexture;
	bool			hasTransparentTexture;

	
	bool			IsTransparent();

};

};

#endif // __COLLADALOADER_COLLADAMATERIAL_H__


