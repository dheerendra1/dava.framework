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
#include "Particles/ParticleLayer.h"
#include "Particles/ParticleEmitter.h"
#include "Particles/ParticleSystem.h"
#include "Utils/StringFormat.h"
#include "Render/RenderManager.h"
#include "Utils/Random.h"

namespace DAVA
{

ParticleLayer::ParticleLayer()
	: head(0)
	, count(0)
	, limit(500)
	, emitter(0)
	, sprite(0)
{
	life = 0;
	lifeVariation = 0;

	number = 0;	
	numberVariation = 0;		

	size = 0;
	sizeVariation = 0;
	sizeOverLife = 0;

	velocity = 0;
	velocityVariation = 0;	
	velocityOverLife = 0;

	spin = 0;				
	spinVariation = 0;
	spinOverLife = 0;

	motionRandom = 0;		
	motionRandomVariation = 0;
	motionRandomOverLife = 0;

	bounce = 0;				
	bounceVariation = 0;
	bounceOverLife = 0;	
	
	colorOverLife = 0;
	colorRandom = 0;
	alphaOverLife = 0;

	particlesToGenerate = 0.0f;
	alignToMotion = 0.0f;
	
	layerTime = 0.0f;
	additive = true;
	type = TYPE_PARTICLES;
}

ParticleLayer::~ParticleLayer()
{
	SafeRelease(sprite);
	head = 0;
	// dynamic cache automatically delete all particles
}

ParticleLayer * ParticleLayer::Clone()
{
	ParticleLayer * layer = new ParticleLayer();
	if (life)
		layer->life.Set(life->Clone());
	
	if (lifeVariation)
		layer->lifeVariation.Set(lifeVariation->Clone());

	if (number)
		layer->number.Set(number->Clone());
	
	if (numberVariation)
		layer->numberVariation.Set(numberVariation->Clone());

	if (size)
		layer->size.Set(size->Clone());
	
	if (sizeVariation)
		layer->sizeVariation.Set(sizeVariation->Clone());
	
	if (sizeOverLife)
		layer->sizeOverLife.Set(sizeOverLife->Clone());
	
	if (velocity)
		layer->velocity.Set(velocity->Clone());
	
	if (velocityVariation)
		layer->velocityVariation.Set(velocityVariation->Clone());
	
	if (velocityOverLife)
		layer->velocityOverLife.Set(velocityOverLife->Clone());
	
	for (int32 f = 0; f < (int32)forces.size(); ++f)
	{
		// TODO: normal fix for the calls
		RefPtr< PropertyLine<Vector2> > forceClone;
		if (forces[f])
			forceClone.Set(forces[f]->Clone());
		layer->forces.push_back(forceClone);

		RefPtr< PropertyLine<Vector2> > forceVariationClone;
		if (forcesVariation[f])
			forceVariationClone.Set(forcesVariation[f]->Clone());
		layer->forcesVariation.push_back(forceVariationClone);

		RefPtr< PropertyLine<float32> > forceOverLifeClone;
		if (forcesOverLife[f])
			forceOverLifeClone.Set(forcesOverLife[f]->Clone());
		layer->forcesOverLife.push_back(forceOverLifeClone);
	}
	
	if (spin)
		layer->spin.Set(spin->Clone());
	
	if (spinVariation)
		layer->spinVariation.Set(spinVariation->Clone());
	
	if (spinOverLife)
		layer->spinOverLife.Set(spinOverLife->Clone());
	
	if (motionRandom)
		layer->motionRandom.Set(motionRandom->Clone());
	
	if (motionRandomVariation)
		layer->motionRandomVariation.Set(motionRandomVariation->Clone());
	
	if (motionRandomOverLife)
		layer->motionRandomOverLife.Set(motionRandomOverLife->Clone());
	
	if (bounce)
		layer->bounce.Set(bounce->Clone());
	
	if (bounceVariation)
		layer->bounceVariation.Set(bounceVariation->Clone());
	
	if (bounceOverLife)
		layer->bounceOverLife.Set(bounceOverLife->Clone());
	
	if (colorOverLife)
		layer->colorOverLife.Set(colorOverLife->Clone());
	
	if (colorRandom)
		layer->colorRandom.Set(colorRandom->Clone());
	
	if (alphaOverLife)
		layer->alphaOverLife.Set(alphaOverLife->Clone());
	
	if (frameOverLife)
		layer->frameOverLife.Set(frameOverLife->Clone());
	
	layer->alignToMotion = alignToMotion;
	layer->additive = additive;
	layer->startTime = startTime;
	layer->endTime = endTime;
	layer->type = type;
	layer->sprite = SafeRetain(sprite);
	layer->pivotPoint = pivotPoint;
	
	layer->frameStart = frameStart;
	layer->frameEnd = frameEnd;
	
	return layer;
}
	

void ParticleLayer::SetEmitter(ParticleEmitter * _emitter)
{
	emitter = _emitter;
}

void ParticleLayer::SetSprite(Sprite * _sprite)
{
	SafeRelease(sprite);
	sprite = SafeRetain(_sprite);
	if(sprite)
	{
		pivotPoint = Vector2(_sprite->GetWidth()/2.0f, _sprite->GetHeight()/2.0f);
	}
}
	
Sprite * ParticleLayer::GetSprite()
{
	return sprite;
}

void ParticleLayer::DeleteAllParticles()
{
	if(TYPE_SINGLE_PARTICLE == type)
	{

	}
	Particle * current = head;
	while(current)
	{
		Particle * next = current->next;
		ParticleSystem::Instance()->DeleteParticle(current);
		count--;
		current = next;
	}
	head = 0;
	
	DVASSERT(count == 0);
}

void ParticleLayer::RunParticle(Particle * particle)
{
	particle->next = head;
	head = particle;
	count++;
}


/*void ParticleLayer::SetLimit(int32 _limit)
{
	limit = _limit;
}

void ParticleLayer::SetPosition(Vector3 _position)
{
	position = _position;
}*/

void ParticleLayer::Restart(bool isDeleteAllParticles)
{
	if (isDeleteAllParticles)
		DeleteAllParticles();

	layerTime = 0.0f;
	particlesToGenerate = 0.0f;
}


void ParticleLayer::Update(float32 timeElapsed)
{
	// increment timer	
	layerTime += timeElapsed;

	switch(type)
	{
	case TYPE_PARTICLES:
		{
			Particle * prev = 0;
			Particle * current = head;
			while(current)
			{
				Particle * next = current->next;
				if (!current->Update(timeElapsed))
				{
					if (prev == 0)head = next;
					else prev->next = next;
					ParticleSystem::Instance()->DeleteParticle(current);
					count--;
				}else
				{
					ProcessParticle(current);
					prev = current;
				}

				current = next;
			}
			
			if (count == 0)
			{
				DVASSERT(head == 0);
			}
			
			if ((layerTime >= startTime) && (layerTime < endTime) && !emitter->IsPaused())
			{
				float32 randCoeff = (float32)(Rand() & 255) / 255.0f;
				float32 newParticles = 0.0f;
				if (number)
					newParticles += timeElapsed * number->GetValue(layerTime);
				if (numberVariation)
					newParticles += randCoeff * timeElapsed * numberVariation->GetValue(layerTime);
				//newParticles *= emitter->GetCurrentNumberScale();
				particlesToGenerate += newParticles;

				while(particlesToGenerate >= 1.0f)
				{
					particlesToGenerate -= 1.0f;
					
					int32 emitPointsCount = emitter->GetEmitPointsCount();
					
					if (emitPointsCount == -1)
						GenerateNewParticle(-1);
					else {
						for (int k = 0; k < emitPointsCount; ++k)
							 GenerateNewParticle(k);
					}

				}
			}
			break;
		}
	case TYPE_SINGLE_PARTICLE:
		{
			bool needUpdate = true;
			if ((layerTime >= startTime) && (layerTime < endTime) && !emitter->IsPaused())
			{
				if(!head)
				{
					GenerateSingleParticle();
					needUpdate = false;
				}
			}
            if(head && needUpdate)
            {
				if (!head->Update(timeElapsed))
				{
					ParticleSystem::Instance()->DeleteParticle(head);
					count--;
					DVASSERT(0 == count);
					head = 0;
					if ((layerTime >= startTime) && (layerTime < endTime) && !emitter->IsPaused())
					{
						GenerateSingleParticle();
					}
				}
				else
				{
					ProcessParticle(head);
				}
            }
			
			break;		
		}
	}
}
	
void ParticleLayer::GenerateSingleParticle()
{
	GenerateNewParticle(-1);
	
	head->angle = 0.0f;
	//particle->velocity.x = 0.0f;
	//particle->velocity.y = 0.0f;
}

void ParticleLayer::GenerateNewParticle(int32 emitIndex)
{
	if (count >= limit)
	{
		return;
	}
	
	Particle * particle = ParticleSystem::Instance()->NewParticle();
	
	particle->next = 0;
	particle->sprite = sprite;
	particle->life = 0.0f;

	float32 randCoeff = (float32)(Rand() & 255) / 255.0f;
	
	
	particle->color = Color();
	if (colorRandom)
	{
		particle->color = colorRandom->GetValue(randCoeff);
	}
	

	particle->lifeTime = 0.0f;
	if (life)
		particle->lifeTime += life->GetValue(layerTime);
	if (lifeVariation)
		particle->lifeTime += (lifeVariation->GetValue(layerTime) * randCoeff);
	
	// size 
	particle->size = Vector2(0.0f, 0.0f); 
	if (size)
		particle->size = size->GetValue(layerTime);
	if (sizeVariation)
		particle->size +=(sizeVariation->GetValue(layerTime) * randCoeff);
	
	particle->size.x /= (float32)sprite->GetWidth();
	particle->size.y /= (float32)sprite->GetHeight();

	float32 vel = 0.0f;
	if (velocity)
		vel += velocity->GetValue(layerTime);
	if (velocityVariation)
		vel += (velocityVariation->GetValue(layerTime) * randCoeff);
	
	particle->angle = 0.0f;
	particle->spin = 0.0f;
	if (spin)
		particle->spin = DegToRad(spin->GetValue(layerTime));
	if (spinVariation)
		particle->spin += DegToRad(spinVariation->GetValue(layerTime) * randCoeff);

	//particle->position = emitter->GetPosition();	
	// parameters should be prepared inside prepare emitter parameters
	emitter->PrepareEmitterParameters(particle, vel, emitIndex);
	particle->angle += alignToMotion;

	particle->sizeOverLife = 1.0f;
	particle->velocityOverLife = 1.0f;
	particle->spinOverLife = 1.0f;
	particle->forceOverLife0 = 1.0f;
		
	particle->force0.x = 0.0f;
	particle->force0.y = 0.0f;

	if ((forces.size() == 1) && (forces[0]))
	{
		particle->force0 = forces[0]->GetValue(layerTime);
	}
	if ((forcesVariation.size() == 1) && (forcesVariation[0]))
	{
		particle->force0 += forcesVariation[0]->GetValue(layerTime) * randCoeff;
	}
	
	particle->frame = frameStart + (int)(randCoeff * (float32)(frameEnd - frameStart));
	
	// process it to fill first life values
	ProcessParticle(particle);

	// go to life
	RunParticle(particle);
}

void ParticleLayer::ProcessParticle(Particle * particle)
{
	float32 t = particle->life / particle->lifeTime;
	if (sizeOverLife)
		particle->sizeOverLife = sizeOverLife->GetValue(t);
	if (spinOverLife)
		particle->spinOverLife = spinOverLife->GetValue(t);
	if (velocityOverLife)
		particle->velocityOverLife = velocityOverLife->GetValue(t);
	if (colorOverLife)
		particle->color = colorOverLife->GetValue(t);
	if (alphaOverLife)
		particle->color.a = alphaOverLife->GetValue(t);

	Color emitterColor;
	if(emitter->GetCurrentColor(&emitterColor))
	{
		particle->drawColor = particle->color*emitterColor*emitter->ambientColor;
	}
	else
	{
		particle->drawColor = particle->color*emitter->ambientColor;
	}
	
	if (frameOverLife)
	{
		int32 frame = (int32)frameOverLife->GetValue(t);
		particle->frame = frame;
	}
	if ((forcesOverLife.size() == 1) && (forcesOverLife[0] != 0))
	{
		particle->forceOverLife0 = forcesOverLife[0]->GetValue(t);
	}
}

void ParticleLayer::Draw()
{
	if (additive)RenderManager::Instance()->SetBlendMode(BLEND_ONE, BLEND_ONE);
	else RenderManager::Instance()->SetBlendMode(BLEND_ONE, BLEND_ONE_MINUS_SRC_ALPHA);

	switch(type)
	{
		case TYPE_PARTICLES:
		{
			Particle * current = head;
			while(current)
			{
				sprite->SetPivotPoint(pivotPoint);
				current->Draw();
				current = current->next;
			}
			break;
		}
		case TYPE_SINGLE_PARTICLE:
		{
			if(head)
			{	
				sprite->SetPivotPoint(pivotPoint);
				head->Draw();
			}
			break;
		}
	}
}


void ParticleLayer::LoadFromYaml(YamlNode * node)
{
// 	PropertyLine<float32> * life;				// in seconds
// 	PropertyLine<float32> * lifeVariation;		// variation part of life that added to particle life during generation of the particle
// 
// 	PropertyLine<float32> * number;				// number of particles per second
// 	PropertyLine<float32> * numberVariation;		// variation part of number that added to particle count during generation of the particle
// 
// 	PropertyLine<Vector2> * size;				// size of particles in pixels 
// 	PropertyLine<Vector2> * sizeVariation;		// size variation in pixels
// 	PropertyLine<float32> * sizeOverLife;
// 
// 	PropertyLine<float32> * velocity;			// velocity in pixels
// 	PropertyLine<float32> * velocityVariation;	
// 	PropertyLine<float32> * velocityOverLife;
// 
// 	PropertyLine<Vector2> * weight;				// weight property from 
// 	PropertyLine<Vector2> * weightVariation;
// 	PropertyLine<float32> * weightOverLife;
// 
// 	PropertyLine<float32> * spin;				// spin of angle / second
// 	PropertyLine<float32> * spinVariation;
// 	PropertyLine<float32> * spinOverLife;
// 
// 	PropertyLine<float32> * motionRandom;		//
// 	PropertyLine<float32> * motionRandomVariation;
// 	PropertyLine<float32> * motionRandomOverLife;
// 
// 	PropertyLine<float32> * bounce;				//
// 	PropertyLine<float32> * bounceVariation;
// 	PropertyLine<float32> * bounceOverLife;	
	
	
	type = TYPE_PARTICLES;
	YamlNode * typeNode = node->Get("layerType");
	if (typeNode)
	{
		if (typeNode->AsString() == "single")
			type = TYPE_SINGLE_PARTICLE;
	}

	YamlNode * spriteNode = node->Get("sprite");
	if (spriteNode)
	{
		YamlNode * pivotPointNode = node->Get("pivotPoint");
		
		Sprite * _sprite = Sprite::Create(spriteNode->AsString());
		Vector2 pivotPointTemp;
		if(pivotPointNode)
		{
			pivotPointTemp = pivotPointNode->AsPoint();
		}
		SetSprite(_sprite);
		pivotPoint = Vector2(_sprite->GetWidth() / 2.0f + pivotPointTemp.x, _sprite->GetHeight() / 2.0f + pivotPointTemp.y);
		
		RenderManager::Instance()->LockNonMain();
		_sprite->GetTexture()->GenerateMipmaps();
		RenderManager::Instance()->UnlockNonMain();
        
        SafeRelease(_sprite);
	}


	colorOverLife = PropertyLineYamlReader::CreateColorPropertyLineFromYamlNode(node, "colorOverLife");
	colorRandom = PropertyLineYamlReader::CreateColorPropertyLineFromYamlNode(node, "colorRandom");
	alphaOverLife = PropertyLineYamlReader::CreateFloatPropertyLineFromYamlNode(node, "alphaOverLife");
	
	frameOverLife = PropertyLineYamlReader::CreateFloatPropertyLineFromYamlNode(node, "frameOverLife");	

	life = PropertyLineYamlReader::CreateFloatPropertyLineFromYamlNode(node, "life");	
	lifeVariation = PropertyLineYamlReader::CreateFloatPropertyLineFromYamlNode(node, "lifeVariation");	

	number = PropertyLineYamlReader::CreateFloatPropertyLineFromYamlNode(node, "number");	
	numberVariation = PropertyLineYamlReader::CreateFloatPropertyLineFromYamlNode(node, "numberVariation");	

	size = PropertyLineYamlReader::CreateVector2PropertyLineFromYamlNode(node, "size");	
	sizeVariation = PropertyLineYamlReader::CreateVector2PropertyLineFromYamlNode(node, "sizeVariation");	
	sizeOverLife = PropertyLineYamlReader::CreateFloatPropertyLineFromYamlNode(node, "sizeOverLife");	

	velocity = PropertyLineYamlReader::CreateFloatPropertyLineFromYamlNode(node, "velocity");	
	velocityVariation = PropertyLineYamlReader::CreateFloatPropertyLineFromYamlNode(node, "velocityVariation");	
	velocityOverLife = PropertyLineYamlReader::CreateFloatPropertyLineFromYamlNode(node, "velocityOverLife");	
	
	int32 forceCount = 0;
	YamlNode * forceCountNode = node->Get("forceCount");
	if (forceCountNode)
		forceCount = forceCountNode->AsInt();

	for (int k = 0; k < forceCount; ++k)
	{
		RefPtr< PropertyLine<Vector2> > force = PropertyLineYamlReader::CreateVector2PropertyLineFromYamlNode(node, Format("force%d", k) );	
		RefPtr< PropertyLine<Vector2> > forceVariation = PropertyLineYamlReader::CreateVector2PropertyLineFromYamlNode(node, Format("forceVariation%d", k));	
		RefPtr< PropertyLine<float32> > forceOverLife = PropertyLineYamlReader::CreateFloatPropertyLineFromYamlNode(node, Format("forceOverLife%d", k));	
		forces.push_back(force);
		forcesVariation.push_back(forceVariation);
		forcesOverLife.push_back(forceOverLife);
	}

	
	spin = PropertyLineYamlReader::CreateFloatPropertyLineFromYamlNode(node, "spin");	
	spinVariation = PropertyLineYamlReader::CreateFloatPropertyLineFromYamlNode(node, "spinVariation");	
	spinOverLife = PropertyLineYamlReader::CreateFloatPropertyLineFromYamlNode(node, "spinOverLife");	

	motionRandom = PropertyLineYamlReader::CreateFloatPropertyLineFromYamlNode(node, "motionRandom");	
	motionRandomVariation = PropertyLineYamlReader::CreateFloatPropertyLineFromYamlNode(node, "motionRandomVariation");	
	motionRandomOverLife = PropertyLineYamlReader::CreateFloatPropertyLineFromYamlNode(node, "motionRandomOverLife");	


	YamlNode * blend = node->Get("blend");
	if (blend)
	{
		if (blend->AsString() == "alpha")
			additive = false;
		if (blend->AsString() == "add")
			additive = true;
	}

	YamlNode * alignToMotionNode = node->Get("alignToMotion");
	if (alignToMotionNode)
		alignToMotion = DegToRad(alignToMotionNode->AsFloat());

	startTime = 0.0f;
	endTime = 100000000.0f;
	YamlNode * startTimeNode = node->Get("startTime");
	if (startTimeNode)
		startTime = startTimeNode->AsFloat();

	YamlNode * endTimeNode = node->Get("endTime");
	if (endTimeNode)
		endTime = endTimeNode->AsFloat();
	
	frameStart = 0;
	frameEnd = 0;

	YamlNode * frameNode = node->Get("frame");
	if (frameNode)
	{
		if (frameNode->GetType() == YamlNode::TYPE_STRING)
			frameStart = frameEnd = frameNode->AsInt();
		else if (frameNode->GetType() == YamlNode::TYPE_ARRAY)
		{
			frameStart = frameNode->Get(0)->AsInt();
			frameEnd = frameNode->Get(1)->AsInt();
		}
	}
	
}



}