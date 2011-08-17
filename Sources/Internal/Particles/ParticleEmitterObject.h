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
#ifndef __DAVAENGINE_PARTICLE_EMITTER_OBJECT_H__
#define __DAVAENGINE_PARTICLE_EMITTER_OBJECT_H__

#include "Scene2D/GameObject.h"
#include "Particles/ParticleEmitter.h"

namespace DAVA 
{
	
/**
	\ingroup particlesystem
	\brief Wrapper class for ParticleEmitter to embed it to Scene2D classes
	This class can help you to use particle emitters inside GameObjects hierarchy.
 
	\todo example of usage 
 */
class ParticleEmitterObject : public GameObject
{
public:	
	ParticleEmitterObject();
	virtual ~ParticleEmitterObject();
	
	/**
		\brief Function sets emitter to particle emitter object.
		This function retain emitter and release old emitter that was stored in this object.
	 
		Be carefull, this function do not clone emitter, so if you want to create new emitter you need to 
		call Clone function in emitter before passing it to ParticleEmitterObject. 
		\param[in] emitter emitter to be set
	 */
	virtual void SetEmitter(ParticleEmitter * emitter);
	/**
		\brief Function loads emitter from yaml file to ParticleEmitterObject.
		This function retain emitter and release old emitter that was stored in this object.
		\param[in] pathName path to resource you want to load
	 */
	virtual void LoadFromYaml(const String & pathName);
	/** 
		\brief Function marks that this object must be deleted after N repeats of emitter animation.
		\param[in] numberOfRepeats number of times we need to repeat emitter animation before deletion
	 */
	virtual void RemoveAfterNRepeats(int32 numberOfRepeats);
	
	/**
		\brief Function marks that this object must be deleted when number of particles will be equal to 0
	 */
	virtual void RemoveWhenEmpty();
	
	/**
		\brief Get emitter stored in ParticleEmitterObject.
		You may need this function if you want to clone emitter from this object to pass it to another object
	 */
	ParticleEmitter * GetEmitter();
	
	virtual void RecalcHierarchy(Sprite::DrawState & parentDrawState);
	/**
		\brief Overloaded function of GameObject to update emitter when GameObjectManager updates GameObject.
		\param[in] timeElapsed time in seconds
	 */
	virtual void Update(float32 timeElapsed);
	/**
		\brief Overloaded function of GameObject to draw emitter when GameObjectManager draws GameObject.
	*/
	virtual void Draw();
	
protected:
	ParticleEmitter * emitter;
	int32 removeAfterNRepeats;
	bool isRemoveWhenEmpty;
};	
	
	
};

#endif // __DAVAENGINE_PARTICLE_EMITTER_OBJECT_H__
