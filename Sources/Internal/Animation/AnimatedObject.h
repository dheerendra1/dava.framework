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
#ifndef __DAVAENGINE_ANIMATEDOBJECT_H__
#define __DAVAENGINE_ANIMATEDOBJECT_H__

#include "Base/BaseTypes.h"
#include "Base/BaseObject.h"
#include "Base/Message.h"
#include "FileSystem/YamlParser.h"

namespace DAVA
{
	
/**
	\defgroup animationsystem Animation System
 */
class Animation;
/**
	\ingroup animationsystem
	\brief AnimatedObject is base class for objects that need to be animated. For example UIControl, GameObject and so on. 
	All objects inside the system that has to be animated must be derived from this class. 
	You cannot use the AnimatedObject class directly.  It instead defines the common interface and 
	behavioral structure for all its subclasses.
 */
class AnimatedObject : public BaseObject
{
public:
	AnimatedObject();
	virtual ~AnimatedObject();

	/**
		\brief stop animations for this given AnimatedObject
		\param[in] track id of track you want to stop or -1 if you want to stop all tracks
	 */
	void	StopAnimations(int32 track = -1);
	/**
		\brief check if this animated object is animating right now
		\param[in] track id of track you want to check or -1 if you want to check if any tracks animating now
		\returns true if object is animating on the requested tracks
	 */
	bool	IsAnimating(int32 track = -1);
	/**
		\brief Load animated object animations from yaml
	 */
	void	LoadFromYaml(YamlNode * node);
	// TODO: void	Pause(int32 track = -1);

	/**
		\brief Returns animation with PLAYING state
		\param[in] track - id of track you want to check, -1 for all tracks. Warning: if track == -1, but multiple animations 
							are playing on different tracks, the function will return only one.
		\returns animation, or 0 if no animations are playing
	 */
	Animation * FindPlayingAnimation(int32 track = -1);
	
private:
	//AnimationsStorage * animationsStorage;
	
/*	
	\TODO optimization of animation subsystem
	
	Animation * activeAnimations; 
	Animation * 
 
 
	// each object support 4 tracks of animations
	static const int32 MAX_NUMBER_OF_TRACKS = 4;
	Animation * headAnimations[MAX_NUMBER_OF_TRACKS];
	Animation * headAllAnimations;

	Animation * FindLastAnimationForTrack(int track);
	
	void AddAnimation(Animation * animation);
	void RemoveAnimation(Animation * animation);

	void Update(float32 timeElapsed);
	
	friend class Animation;*/
};
	
};

#endif // __DAVAENGINE_INTERPOLATION_H__
