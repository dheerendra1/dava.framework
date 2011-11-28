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
#include "Animation/LinearAnimation.h"

namespace DAVA 
{
ParticleEmitter::ParticleEmitter()
{
	type = EMITTER_POINT;
    emissionVector = RefPtr<PropertyLineValue<Vector3> >(new PropertyLineValue<Vector3>(Vector3(1.0f, 0.0f, 0.0f)));
	emissionAngle = RefPtr<PropertyLineValue<float32> >(new PropertyLineValue<float32>(0.0f));
	emissionRange = RefPtr<PropertyLineValue<float32> >(new PropertyLineValue<float32>(360.0f));
	size = RefPtr<PropertyLineValue<Vector3> >(0);
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
    is3D = false;
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
    emitter->emissionVector = emissionVector;
	if (emissionAngle)
		emitter->emissionAngle = emissionAngle->Clone();
	if (emissionRange)
		emitter->emissionRange = emissionRange->Clone();
	if(colorOverLife)
		emitter->colorOverLife = colorOverLife->Clone();
	if (radius)
		emitter->radius = radius->Clone();
    if (size)
        emitter->size = size->Clone();
	
	emitter->type = type;
	emitter->lifeTime = lifeTime;
	emitter->emitPointsCount = emitPointsCount;
	emitter->isPaused = isPaused;
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
        if(!(*it)->isDisabled)
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
    if (!is3D)
    {
        Vector3 tempPosition = particlesFollow ? Vector3() : position;
        if (type == EMITTER_POINT)
        {
            particle->position = tempPosition;
        }
        else if (type == EMITTER_LINE)
        {
            // TODO: add emitter angle support
            float32 rand05 = ((float32)(Rand() & 255) / 255.0f) - 0.5f; // [-0.5f, 0.5f]
            Vector3 lineDirection(0, 0, 0);
            if(size)
                lineDirection = size->GetValue(time)*rand05;
            particle->position = tempPosition + lineDirection;
        }
        else if (type == EMITTER_RECT)
        {
            // TODO: add emitter angle support
            float32 rand05_x = (float32)Random::Instance()->RandFloat() - 0.5f; // [-0.5f, 0.5f]
            float32 rand05_y = (float32)Random::Instance()->RandFloat() - 0.5f; // [-0.5f, 0.5f]
            float32 rand05_z = (float32)Random::Instance()->RandFloat() - 0.5f; // [-0.5f, 0.5f]
            Vector3 lineDirection(0, 0, 0);
            if(size)
                lineDirection = Vector3(size->GetValue(time).x * rand05_x, size->GetValue(time).y * rand05_y, size->GetValue(time).z * rand05_z);
            particle->position = tempPosition + lineDirection;
        }
        else if (type == EMITTER_ONCIRCLE)
        {
            // here just set particle position
            particle->position = tempPosition;
            //if (
        }
        
        Vector3 vel;
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
        }
        else
        {
            particleAngle += range * (float32)emitIndex / (float32)emitPointsCount;
        }
        
        
        vel.x = cosf(particleAngle);
        vel.y = sinf(particleAngle);
        vel.z = 0;
        
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
    else
    {
        Vector3 tempPosition = particlesFollow ? Vector3() : position;
        if (type == EMITTER_POINT)
        {
            particle->position = tempPosition;
        }
        else if (type == EMITTER_LINE)
        {
            // TODO: add emitter angle support
            float32 rand05 = (float32)Random::Instance()->RandFloat() - 0.5f; // [-0.5f, 0.5f]
            Vector3 lineDirection(0, 0, 0);
            if(size)
                lineDirection = size->GetValue(time)*rand05;
            particle->position = tempPosition + lineDirection;
        }
        else if (type == EMITTER_RECT)
        {
            // TODO: add emitter angle support
            float32 rand05_x = (float32)Random::Instance()->RandFloat() - 0.5f; // [-0.5f, 0.5f]
            float32 rand05_y = (float32)Random::Instance()->RandFloat() - 0.5f; // [-0.5f, 0.5f]
            float32 rand05_z = (float32)Random::Instance()->RandFloat() - 0.5f; // [-0.5f, 0.5f]
            Vector3 lineDirection(0, 0, 0);
            if(size)
                lineDirection = Vector3(size->GetValue(time).x * rand05_x, size->GetValue(time).y * rand05_y, size->GetValue(time).z * rand05_z);
            particle->position = tempPosition + lineDirection;
        }
        else if (type == EMITTER_ONCIRCLE)
        {
            // here just set particle position
            particle->position = tempPosition;
        }
        
        Vector3 vel = emissionVector->GetValue(0);
        
        Vector3 rotVect(0, 0, 1);
        float32 phi = PI*2*(float32)Random::Instance()->RandFloat();
        if(vel.x != 0)
        {
            rotVect.y = sinf(phi);
            rotVect.z = cosf(phi);
            rotVect.x = - rotVect.y*vel.y/vel.x - rotVect.z*vel.z/vel.x;
        }
        else if(vel.y != 0)
        {
            rotVect.x = cosf(phi);
            rotVect.z = sinf(phi);
            rotVect.y = - rotVect.z*vel.z/vel.y;
        }
        else if(vel.z != 0)
        {
            rotVect.x = cosf(phi);
            rotVect.y = sinf(phi);
            rotVect.z = 0;
        }
        rotVect.Normalize();
        
        float32 range = DegToRad(emissionRange->GetValue(time) + angle);
        float32 rand05 = (float32)Random::Instance()->RandFloat() - 0.5f;
        
        Vector3 q_v(rotVect*sinf(range*rand05/2));
        float32 q_w = cosf(range*rand05/2);

        Vector3 q1_v(q_v);
        float32 q1_w = -q_w;
        q1_v /= (q_v.SquareLength() + q_w*q_w);
        q1_w /= (q_v.SquareLength() + q_w*q_w);
        
        Vector3 v_v(vel);
        
        Vector3 qv_v = q_v.CrossProduct(v_v) + q_w*v_v;
        float32 qv_w = - q_v.DotProduct(v_v);
        
        Vector3 qvq1_v = qv_v.CrossProduct(q1_v) + qv_w*q1_v + q1_w*qv_v;
        
        qvq1_v *= velocity;
        particle->velocity = qvq1_v;

        if (type == EMITTER_ONCIRCLE)
        {
            qvq1_v.Normalize();
            particle->position += qvq1_v * radius->GetValue(time);
        }
       
        if(is3D)
            particle->angle = 0.0f;
        else
            particle->angle = atan2f(qvq1_v.y, qvq1_v.x);
    }
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
        
		if (emitterNode->Get("emissionVector"))
			emissionVector = PropertyLineYamlReader::CreateVector3PropertyLineFromYamlNode(emitterNode, "emissionVector");
        
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
        
        is3D = false;
		YamlNode * _3dNode = emitterNode->Get("3d");
		if (_3dNode)
		{	
			is3D = _3dNode->AsBool();
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
		
        size = PropertyLineYamlReader::CreateVector3PropertyLineFromYamlNode(emitterNode, "size");
        
        if(size == 0)
        {
            Vector3 _size(0, 0, 0);
            YamlNode * widthNode = emitterNode->Get("width");
            if (widthNode)
                _size.x = widthNode->AsFloat();

            YamlNode * heightNode = emitterNode->Get("height");
            if (heightNode)
                _size.y = heightNode->AsFloat();

            YamlNode * depthNode = emitterNode->Get("depth");
            if (depthNode)
                _size.y = depthNode->AsFloat();
            
            size = new PropertyLineValue<Vector3>(_size);
        }
        
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
	
Vector<ParticleLayer*> & ParticleEmitter::GetLayers()
{
	return layers;
}
	
float32 ParticleEmitter::GetLifeTime()
{
	return lifeTime;
}
    
void ParticleEmitter::SetLifeTime(float32 time)
{
    lifeTime = time;
}
    
float32 ParticleEmitter::GetTime()
{
    return time;
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

Vector3 ParticleEmitter::GetSize()
{
    if(size)
        return size->GetValue(0);
    return Vector3(0, 0, 0);
}
    
Vector3 ParticleEmitter::GetSize(float32 time)
{
    if(size)
        return size->GetValue(time);
    return Vector3(0, 0, 0);
}
    
void ParticleEmitter::SetSize(const Vector3& _size)
{
	size = new PropertyLineValue<Vector3>(_size);
}

Animation * ParticleEmitter::SizeAnimation(const Vector3 & newSize, float32 time, Interpolation::FuncType interpolationFunc /*= Interpolation::LINEAR*/, int32 track /*= 0*/)
{
    Vector3 _size(0, 0, 0);
    if(size)
        _size = size->GetValue(0);
	LinearAnimation<Vector3> * animation = new LinearAnimation<Vector3>(this, &_size, newSize, time, interpolationFunc);
	animation->Start(track);
	return animation;
}

}