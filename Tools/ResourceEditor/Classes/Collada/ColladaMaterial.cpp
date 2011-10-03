#include "stdafx.h"
#include "ColladaMaterial.h"
#include "ColladaScene.h"

#include "FCDocument/FCDTexture.h"
#include "FCDocument/FCDEffect.h"
#include "FCDocument/FCDEffectStandard.h"

namespace DAVA
{

ColladaMaterial::ColladaMaterial( ColladaScene * scene, FCDMaterial * _material )
{
	material = _material;
	
	if (material == 0) 
 	{
 		SetDefaultMaterial();
 		return;
 	}
	
	// if common profile does not exist, the set as a default materiqal
	FCDEffect* fx = material->GetEffect();
	FCDEffectProfile* profile = fx->FindProfile(FUDaeProfileType::COMMON);
	if (profile == 0) 
 	{
 		SetDefaultMaterial();
 		return;
 	}
	

	// copy properties of common profile
	FCDEffectStandard* standardProfile=dynamic_cast<FCDEffectStandard*>(profile);

	// configure alpha value in diffuse material
	transparency =standardProfile->GetTranslucencyFactor();

	// ambient
	ambient = Vector4(
		standardProfile->GetAmbientColor().x,
		standardProfile->GetAmbientColor().y,
		standardProfile->GetAmbientColor().z,
		standardProfile->GetAmbientColor().w			  
		);

	// diffuse component
	diffuse = Vector4(
		standardProfile->GetDiffuseColor().x,
		standardProfile->GetDiffuseColor().y,
		standardProfile->GetDiffuseColor().z,
		1.0 // opaque for opengl, use m_transparency on polygon::render 		  
		);

	// specular
	float specularFactor = standardProfile->GetSpecularFactor();
	specular = Vector4(
		specularFactor * standardProfile->GetSpecularColor().x,
		specularFactor * standardProfile->GetSpecularColor().y,
		specularFactor * standardProfile->GetSpecularColor().z,
		specularFactor * standardProfile->GetSpecularColor().w			  
		);

	// shininess
	shininess = standardProfile->GetShininess();

	// emission
	if (standardProfile->IsEmissionFactor()) 
	{
		emission = Vector4(
			standardProfile->GetEmissionFactor(),
			standardProfile->GetEmissionFactor(),
			standardProfile->GetEmissionFactor(),
			standardProfile->GetEmissionFactor()
			);
	} else {
		emission = Vector4(
			standardProfile->GetEmissionColor().x * standardProfile->GetEmissionFactor(),
			standardProfile->GetEmissionColor().y * standardProfile->GetEmissionFactor(),
			standardProfile->GetEmissionColor().z * standardProfile->GetEmissionFactor(),
			standardProfile->GetEmissionColor().w * standardProfile->GetEmissionFactor()
			);
	}

//	if (standardProfile->GetTransparencyMode()==FCDEffectStandard::RGB_ZERO) 
//		transparency = 1.0f - transparency;

	// textures

	// diffusse textures
	hasDiffuseTexture = false;
	diffuseTexture = 0;

	// diffuse texture
	if (standardProfile->GetTextureCount(FUDaeTextureChannel::DIFFUSE) > 0) 
	{
		FCDTexture * texture = standardProfile->GetTexture(FUDaeTextureChannel::DIFFUSE, 0);
		if (texture != NULL) 
		{
			FCDImage * image = texture->GetImage();
			
			if (image!=NULL) 
			{
				diffuseTexture = scene->FindTextureWithName(image->GetDaeId());
				diffuseTextureName = image->GetDaeId();

				if (diffuseTexture != NULL)
					if (diffuseTexture->GetTextureId() != -1)
						hasDiffuseTexture = true;
			}
		}
	}	

	// reflective texture
	hasReflectiveTexture = false;
	reflectiveTexture = 0;
	
	// reflective texture
	if (standardProfile->GetTextureCount(FUDaeTextureChannel::REFLECTION)>0) 
	{
		//float r=standardProfile->GetReflectivityFactor();
		//float r=standardProfile->GetReflectivity();
		//float r2=standardProfile->GetReflectivityFactor();
		FCDTexture * texture = standardProfile->GetTexture(FUDaeTextureChannel::REFLECTION,0);
		if (texture != NULL) 
		{
			FCDImage * image = texture->GetImage();
			if (image!=NULL) 
			{
				reflectiveTexture = scene->FindTextureWithName(image->GetDaeId());
				reflectiveTextureName = image->GetDaeId();

				if (reflectiveTexture != NULL)
					if (reflectiveTexture->GetTextureId() != -1)
						hasReflectiveTexture = true;
			}
		}
	}
	
	printf("* added material: %s alpha: %f", material->GetDaeId().c_str(), transparency);
	if (hasDiffuseTexture)
		printf(" diffuse: %s", diffuseTextureName.c_str());
	if (hasReflectiveTexture)
		printf(" reflective: %s", reflectiveTextureName.c_str());
	printf("\n");
}

ColladaMaterial::~ColladaMaterial()
{
	
}


void ColladaMaterial::SetDefaultMaterial() 
{

	ambient = Vector4(1.0f,1.0f,1.0f,1.0f );
	diffuse = Vector4(1.0f,0.0f,1.0f,1.0f ); // a nice pink color
	specular = Vector4(0.2f,0.2f,0.2f,1.0f );
	shininess = 10.0f;
	emission = Vector4(0.0f,0.0f,0.0f,1.0f );
	transparency = 1.0f; // opaque like colladamax 3.03

	hasReflectiveTexture = false;
	hasDiffuseTexture = false;
	hasTransparentTexture = false;
}

bool ColladaMaterial::IsTransparent()
{
	if ((hasDiffuseTexture) && (diffuseTexture->hasAlphaChannel)) 
		return true;

//	if transparent, return true
//	if ((m_main->GetTransparencyFix()) && (m_transparency<1.0f))
//		return true;

	if (transparency < 1.0f)
		return true;

	// in other case, false
	return false;
}

ColladaMaterial * ColladaMaterial::defaultMaterial = 0;

ColladaMaterial * ColladaMaterial::GetDefaultMaterial()
{
	if (0 == defaultMaterial)
	{
		defaultMaterial = new ColladaMaterial(0, 0);
	}
	return defaultMaterial;
}


};