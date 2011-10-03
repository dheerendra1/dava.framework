#include "stdafx.h"
#include "ColladaLight.h"

namespace DAVA
{

ColladaLight::ColladaLight( FCDLight * _light )
{
	light = _light;

	if (light->GetLightType() == FCDLight::AMBIENT)
		type = AMBIENT;

	if (light->GetLightType() == FCDLight::SPOT)
		type = SPOT;

	if (light->GetLightType() == FCDLight::POINT)
		type = POINT;

	if (light->GetLightType() == FCDLight::DIRECTIONAL)
		type = DIRECTIONAL;
	
	printf("* added light: type: %d\n", type);
		
	// extract base color of the light
	// intensity * color
	float intensity = light->GetIntensity();
	color = Vector4(intensity * light->GetColor()->x, 
					intensity * light->GetColor()->y, 
					intensity * light->GetColor()->z, 1.0f);

	// store specific values for point lights
	attenuation.x = light->GetConstantAttenuationFactor();
	attenuation.y = light->GetLinearAttenuationFactor();
	attenuation.z = light->GetQuadraticAttenuationFactor();
}

ColladaLight::~ColladaLight()
{
	
}

void ColladaLight::ApplyLight(ColladaLightState & state)
{
	if (type == AMBIENT)
	{
		state.globalAmbientalLight[0] = state.globalAmbientalLight[0] + color.x;
		state.globalAmbientalLight[1] = state.globalAmbientalLight[1] + color.y;
		state.globalAmbientalLight[2] = state.globalAmbientalLight[2] + color.z;
	}


	if (type == DIRECTIONAL)
	{
		GLfloat lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };  // delete
		GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		GLfloat lightDiffuse[4] = { color.x, color.y, color.z, 1.0f};

		// 0,0,1 default direcction, I need to test this
		GLfloat lightDir[] = { 0.0f, 0.0f, 1.0f, 0.0f };

		// activate this light
		glEnable(GL_LIGHT0 + state.lightIndex);

		//always position 0,0,0 because light  is moved with transformations
		glLightfv(GL_LIGHT0 + state.lightIndex, GL_POSITION, lightDir);

		// colors 
		glLightfv(GL_LIGHT0 + state.lightIndex, GL_AMBIENT,  lightDiffuse); // now like diffuse color
		glLightfv(GL_LIGHT0 + state.lightIndex, GL_DIFFUSE,  lightDiffuse);
		glLightfv(GL_LIGHT0 + state.lightIndex, GL_SPECULAR, lightSpecular);

		//if (show_lights) 
		{
			glDisable(GL_LIGHTING);
			//glColor3f(light_diffuse[0], light_diffuse[1], light_diffuse[2]);
			glColor3f(0.984375, 0.078125, 0.64453125);
            // glutWireTorus(0.5,1.0,12,12);
			glEnable(GL_LIGHTING);
			glColor3f(1.0f, 1.0f, 1.0f);
		}

		state.lightIndex++;
	}
	 
	if (type == POINT)
	{
		GLfloat lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };  // delete
		GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		GLfloat lightDiffuse[4] = { color.x, color.y, color.z, 1.0f};

		GLfloat lightPos[] = { 0.0f, 0.0f, 0.0f, 1.0f };

		// activate this light
		glEnable(GL_LIGHT0 + state.lightIndex);

		//always position 0,0,0 because light  is moved with transformations
		glLightfv(GL_LIGHT0 + state.lightIndex, GL_POSITION, lightPos);

		// colors 
		glLightfv(GL_LIGHT0 + state.lightIndex, GL_AMBIENT, lightDiffuse); // now like diffuse color
		glLightfv(GL_LIGHT0 + state.lightIndex, GL_DIFFUSE, lightDiffuse);
		glLightfv(GL_LIGHT0 + state.lightIndex, GL_SPECULAR,lightSpecular);

		//specific values for this light
		glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION,	attenuation.x);
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION,		attenuation.y);
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION,	attenuation.z);

		//other values
		glLightf(GL_LIGHT0 + state.lightIndex, GL_SPOT_CUTOFF, 180.0f);
		glLightf(GL_LIGHT0 + state.lightIndex, GL_SPOT_EXPONENT, 0.0f);
		
		GLfloat spotDirection[] = { 0.0f, 0.0f, -1.0f, 0.0f }; // irrelevant for this light (I guess)
		glLightfv(GL_LIGHT0 + state.lightIndex, GL_SPOT_DIRECTION, spotDirection);

		// draw a sphere that represents this point light
		//if (show_lights) 
		{
			glDisable(GL_LIGHTING);
			glColor3f(0.984375, 0.078125, 0.64453125);
			// glutWireSphere(1.0f, 12, 12);
			glEnable(GL_LIGHTING);
			glColor3f(1.0f, 1.0f, 1.0f);
		}
		state.lightIndex++;
	} 
	
	if (type == SPOT)
	{
		GLfloat lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };  // delete
		GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		GLfloat lightDiffuse[4] = { color.x, color.y, color.z, 1.0f};

		// activate this light
		glEnable(GL_LIGHT0 + state.lightIndex);

		// spot light configuration
		glLightfv (GL_LIGHT0 + state.lightIndex, GL_AMBIENT, lightAmbient);
		glLightfv (GL_LIGHT0 + state.lightIndex, GL_DIFFUSE, lightDiffuse);
		glLightfv (GL_LIGHT0 + state.lightIndex, GL_SPECULAR, lightSpecular);

		GLfloat lightPosition[] = {0.0f, 0.0f, 0.0f, 1.0f }; // w=0 = direcctional, w=1 spot or positional
		glLightfv (GL_LIGHT0 + state.lightIndex, GL_POSITION, lightPosition);

		glLightf (GL_LIGHT0 + state.lightIndex, GL_SPOT_CUTOFF, 45.0f);
		GLfloat lightDirection[] = {0.0f, 0.0f, -1.0f, 0.0f };
		glLightfv (GL_LIGHT0 + state.lightIndex, GL_SPOT_DIRECTION, lightDirection);
		glLightf (GL_LIGHT0 + state.lightIndex, GL_SPOT_EXPONENT, 20.0f);

		// atenuaton
		glLightf (GL_LIGHT0 + state.lightIndex, GL_LINEAR_ATTENUATION,    0.0f);
		glLightf (GL_LIGHT0 + state.lightIndex, GL_QUADRATIC_ATTENUATION, 0.0f);
		glLightf (GL_LIGHT0 + state.lightIndex, GL_CONSTANT_ATTENUATION,  0.0);

		// draw a sphere that represents this point light
		//if (show_lights) 
		{
			glDisable(GL_LIGHTING);
			//glColor3f(light_diffuse[0], light_diffuse[1], light_diffuse[2]);
			glColor3f(0.984375, 0.078125, 0.64453125);
			// glutWireCone(0.5f,1.0f,10,10);
			glEnable(GL_LIGHTING);
			glColor3f(1.0f, 1.0f, 1.0f);
		}
		state.lightIndex++;
	}
}





};