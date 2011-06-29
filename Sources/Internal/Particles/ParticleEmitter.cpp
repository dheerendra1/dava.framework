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
#include "Particles/ParticleEmitter.h"
#include "Particles/Particle.h"
#include "Particles/ParticleLayer.h"
#include "Render/RenderManager.h"
#include "Utils/Random.h"

namespace DAVA 
{
ParticleEmitter::ParticleEmitter()
{
	type = EMITTER_POINT;
	emissionAngle = RefPtr<PropertyLineValue<float32> >(new PropertyLineValue<float32>(0.0f));
	emissionRange = RefPtr<PropertyLineValue<float32> >(new PropertyLineValue<float32>(360.0f));
	colorOverLife = 0;
	radius = 0;
	// number = new PropertyLineValue<float>(1.0f);

	time = 0.0f;
	repeatCount = 0;
	lifeTime = 1000000000.0f;
	emitPointsCount = -1;
	isPaused = false;
	angle = 0.f;
	isAutorestart = true;
	particlesFollow = false;
}

ParticleEmitter::~ParticleEmitter()
{
	Vector<ParticleLayer*>::iterator it;
	for(it = layers.begin(); it != layers.end(); ++it)
	{
		SafeRelease(*it);
	}	
	layers.clear();
}
	
ParticleEmitter * ParticleEmitter::Clone()
{
	ParticleEmitter * emitter = new ParticleEmitter();
	for (int32 k = 0; k < (int32)layers.size(); ++k)
	{
		ParticleLayer * newLayer = layers[k]->Clone();
		newLayer->SetEmitter(emitter);
		emitter->layers.push_back(newLayer);
	}
	if (emissionAngle)
		emitter->emissionAngle = emissionAngle->Clone();
	if (emissionRange)
		emitter->emissionRange = emissionRange->Clone();
	if(colorOverLife)
		emitter->colorOverLife = colorOverLife->Clone();
	if (radius)
		emitter->radius = radius->Clone();
	
	emitter->type = type;
	emitter->lifeTime = lifeTime;
	emitter->emitPointsCount = emitPointsCount;
	emitter->isPaused = isPaused;
	emitter->width = width;
	emitter->height = height;
	emitter->isAutorestart = isAutorestart;
	emitter->particlesFollow = particlesFollow;
	return emitter;
}

void ParticleEmitter::AddLayer(ParticleLayer * layer)
{
	if (layer)
	{
		layers.push_back(layer);
		layer->Retain();
		layer->SetEmitter(this);
	}	
}

/* float32 ParticleEmitter::GetCurrentNumberScale()
{
	return number->GetValue(time);
} */


void ParticleEmitter::Restart(bool isDeleteAllParticles)
{
	Vector<ParticleLayer*>::iterator it;
	for(it = layers.begin(); it != layers.end(); ++it)
	{
		(*it)->Restart(isDeleteAllParticles);
	}

	time = 0.0f;
	repeatCount = 0;
}

void ParticleEmitter::Update(float32 timeElapsed)
{
	time += timeElapsed;
	float32 t = time / lifeTime;

	if (colorOverLife)
	{
		currentColor = colorOverLife->GetValue(t);
	}

	if(isAutorestart && (time > lifeTime))
	{
		time -= lifeTime;
		Restart(true);
		repeatCount ++;
	}

	Vector<ParticleLayer*>::iterator it;
	for(it = layers.begin(); it != layers.end(); ++it)
	{
		(*it)->Update(timeElapsed);
	}
}

void ParticleEmitter::Draw()
{
	eBlendMode srcMode = RenderManager::Instance()->GetSrcBlend();
	eBlendMode destMode = RenderManager::Instance()->GetDestBlend();

	if(particlesFollow)
	{
		RenderManager::Instance()->PushDrawMatrix();
		RenderManager::Instance()->SetDrawTranslate(position);
	}
	Vector<ParticleLayer*>::iterator it;
	for(it = layers.begin(); it != layers.end(); ++it)
	{
		(*it)->Draw();
	}
	if(particlesFollow)
	{
		RenderManager::Instance()->PopDrawMatrix();
	}

	RenderManager::Instance()->SetBlendMode(srcMode, destMode);
}

void ParticleEmitter::PrepareEmitterParameters(Particle * particle, float32 velocity, int32 emitIndex)
{
	Vector2 tempPosition = particlesFollow ? Vector2() : position;
	if (type == EMITTER_POINT)
	{
		particle->position = tempPosition;
	}else if (type == EMITTER_LINE)
	{
		// TODO: add emitter angle support
		float32 rand05 = ((float32)(Rand() & 255) / 255.0f) - 0.5f; // [-0.5f, 0.5f]
		Vector2 lineDirection(width * rand05, 0);
		particle->position = tempPosition + lineDirection;
	}else if (type == EMITTER_RECT)
	{
		// TODO: add emitter angle support
		float32 rand05_x = ((float32)(Rand() & 255) / 255.0f) - 0.5f; // [-0.5f, 0.5f]
		float32 rand05_y = ((float32)(Rand() & 255) / 255.0f) - 0.5f; // [-0.5f, 0.5f]
		Vector2 lineDirection(width * rand05_x, height * rand05_y);
		particle->position = tempPosition + lineDirection;
	}else if (type == EMITTER_ONCIRCLE)
	{
		// here just set particle position
		particle->position = tempPosition;
		//if (
	}
	
	Vector2 vel;
	//vel.x = (float32)((rand() & 255) - 128);
	//vel.y = (float32)((rand() & 255) - 128);
	//vel.Normalize();

	float32 rand05 = ((float32)(Rand() & 255) / 255.0f) - 0.5f; // [-0.5f, 0.5f]
	
	float32 particleAngle = DegToRad(emissionAngle->GetValue(time) + angle);
	float32 range = DegToRad(emissionRange->GetValue(time));
	
	if (emitPointsCount == -1)
	{
		// if emitAtPoints property is not set just emit randomly in range
		particleAngle += range * rand05;
	}else {
		particleAngle += range * (float32)emitIndex / (float32)emitPointsCount;
	}


	vel.x = cosf(particleAngle);
	vel.y = sinf(particleAngle);
	
	// reuse particle velocity we've calculated 
	if (type == EMITTER_ONCIRCLE)
	{
		particle->position += vel * radius->GetValue(time);
	}

	vel *= velocity;
	particle->velocity.x = vel.x;
	particle->velocity.y = vel.y;
	particle->angle = particleAngle;
}

void ParticleEmitter::LoadFromYaml(const String & filename)
{
	time = 0.0f;
	repeatCount = 0;
	lifeTime = 1000000000.0f;
	
	YamlParser * parser = YamlParser::Create(filename);
	YamlNode * rootNode = parser->GetRootNode();

	int cnt = rootNode->GetCount();
	for (int k = 0; k < cnt; ++k)
	{
		YamlNode * node = rootNode->Get(k);
		YamlNode * typeNode = node->Get("type");
		if (typeNode && typeNode->AsString() == "layer")
		{	
			ParticleLayer * layer = new ParticleLayer();
			layer->LoadFromYaml(node);
			AddLayer(layer);
			SafeRelease(layer);
		}
	}

	YamlNode * emitterNode = rootNode->Get("emitter");
	if (emitterNode)
	{		
		if (emitterNode->Get("emissionAngle"))
			emissionAngle = PropertyLineYamlReader::CreateFloatPropertyLineFromYamlNode(emitterNode, "emissionAngle");
		if (emitterNode->Get("emissionRange"))
			emissionRange = PropertyLineYamlReader::CreateFloatPropertyLineFromYamlNode(emitterNode, "emissionRange");
		if (emitterNode->Get("colorOverLife"))
			colorOverLife = PropertyLineYamlReader::CreateColorPropertyLineFromYamlNode(emitterNode, "colorOverLife");
		if (emitterNode->Get("radius"))
			radius = PropertyLineYamlReader::CreateFloatPropertyLineFromYamlNode(emitterNode, "radius");
		
		emitPointsCount = -1; 
		YamlNode * emitAtPointsNode = emitterNode->Get("emitAtPoints");
		if (emitAtPointsNode)
			emitPointsCount = emitAtPointsNode->AsInt();
		
		YamlNode * lifeTimeNode = emitterNode->Get("life");
		if (lifeTimeNode)
		{
			lifeTime = lifeTimeNode->AsFloat();
		}else
		{
			lifeTime = 1000000000.0f;
		}
		
		YamlNode * typeNode = emitterNode->Get("type");
		if (typeNode)
		{	
			if (typeNode->AsString() == "point")
				type = EMITTER_POINT;
			else if (typeNode->AsString() == "line")
				type = EMITTER_LINE;
			else if (typeNode->AsString() == "rect")
				type = EMITTER_RECT;
			else if (typeNode->AsString() == "oncircle")
				type = EMITTER_ONCIRCLE;
			else 
				type = EMITTER_POINT;
		}else
			type = EMITTER_POINT;
		
		width = 1.0f;
		YamlNode * widthNode = emitterNode->Get("width");
		if (widthNode)
			width = widthNode->AsFloat();

		height = 1.0f;
		YamlNode * heightNode = emitterNode->Get("height");
		if (heightNode)
			height = heightNode->AsFloat();

		YamlNode * autorestartNode = emitterNode->Get("autorestart");
		if(autorestartNode)
			isAutorestart = autorestartNode->AsBool();

		YamlNode * particlesFollowNode = emitterNode->Get("particlesFollow");
		if(particlesFollowNode)
			particlesFollow = particlesFollowNode->AsBool();
	}

	
	SafeRelease(parser);
}

int32 ParticleEmitter::GetParticleCount()
{
	int32 cnt = 0;
	Vector<ParticleLayer*>::iterator it;
	for(it = layers.begin(); it != layers.end(); ++it)
	{
		cnt += (*it)->GetParticleCount();
	}
	return cnt;
}

int32 ParticleEmitter::GetRepeatCount()
{
	return repeatCount;
}

int32 ParticleEmitter::GetEmitPointsCount()
{
	return emitPointsCount;
}
	
const Vector<ParticleLayer*> & ParticleEmitter::GetLayers()
{
	return layers;
}
	
float32 ParticleEmitter::GetLifeTime()
{
	return lifeTime;
}

void ParticleEmitter::Pause(bool _isPaused)
{
	isPaused = _isPaused;
}
bool ParticleEmitter::IsPaused()
{
	return isPaused;
}

void ParticleEmitter::SetAutorestart(bool _isAutorestart)
{
	isAutorestart = _isAutorestart;
}

bool ParticleEmitter::GetAutorestart()
{
	return isAutorestart;
}

Vector2 ParticleEmitter::GetSize()
{
	return Vector2(width, height);
}

void ParticleEmitter::SetSize(const Vector2& size)
{
	width = size.x;
	height = size.y;
}

}