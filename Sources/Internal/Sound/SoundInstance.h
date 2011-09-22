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
        * Created by Ivan Petrochenko
=====================================================================================*/

#ifndef __DAVAENGINE_SOUND_INSTANCE_H__
#define __DAVAENGINE_SOUND_INSTANCE_H__

#include "Base/BaseTypes.h"
#include "Base/BaseObject.h"
#include "Base/EventDispatcher.h"
#include "Animation/Animation.h"

namespace DAVA
{

class Sound;
class SoundSystem;
class SoundChannel;
#if defined(__DAVAENGINE_IPHONE__)
class MusicIos;
#endif

/**
	\ingroup sound
	\brief SoundInstance
*/
class SoundInstance : public AnimatedObject
{
public:
	enum eState
	{
		STATE_PLAYING = 0,		//!< Sound in playing.
		STATE_COMPLETED,		//!< Sound is stopped (after completion or calling Stop()).
		STATE_FORCED_STOPPED	//!< Sound is stopped by calling Stop(), but next Update() is not yet called.
	};

	enum eEvent
	{
		EVENT_COMPLETED = 1 //!< Event is performed when sound playback is completed.
	};

	/**
		\brief Set new volume.
		\param[in] volume New volume.
	*/
	void	SetVolume(float32 volume);
	
	/**
		\brief Get current volume.
		\returns Current volume.
	*/
	float32	GetVolume();

	/**
		\brief Stop playback.
	*/
	void	Stop();

	/**
		\brief Pause or resume playback.
		\param[in] pause pause(true) or resume(false) playback.
	*/
	void	Pause(bool pause);

	/**
		\brief Starts animation of the volume.
		\param[in] newVolume Volume value at the end of the animation.
		\param[in] time Animation time.
		\param[in] track Animation track.
		\returns Animation object.
	*/
	Animation * VolumeAnimation(float32 newVolume, float32 time, int32 track = 0);

	/**
		\brief Get playback state.
		\returns Current playback state.
	*/
	eState GetState();

	IMPLEMENT_EVENT_DISPATCHER(soundInstanceEventDispatcher);

private:
	

	SoundInstance();
	virtual ~SoundInstance() {};

	bool Update(); //true if alive

	eState state;
	SoundChannel * buddyChannel;

	float32 animatedVolume;
	void OnVolumeAnimationEnded(BaseObject * caller, void * userData, void * callerData);

friend class Sound;
friend class SoundSystem;
#if defined(__DAVAENGINE_IPHONE__)
    friend class MusicIos;
    MusicIos * buddyMusic;
#endif
};

};

#endif //__DAVAENGINE_SOUND_INSTANCE_H__
