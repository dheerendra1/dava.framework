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

#include "Sound/SoundSystem.h"
#include "Sound/SoundChannel.h"
#include "Sound/ALUtils.h"
#include "Sound/SoundInstance.h"
#include "Sound/SoundGroup.h"


namespace DAVA
{

#ifdef __DAVASOUND_AL__
ALCcontext * context;
ALCdevice * device;
#endif
SoundSystem::SoundSystem(int32 _maxChannels)
:	maxChannels(_maxChannels),
	volume(1.f)
{
#ifdef __DAVASOUND_AL__
	device = alcOpenDevice(0);
	if(device)
	{
		context = alcCreateContext(device, 0);
		AL_CHECKERROR();
		AL_VERIFY(alcMakeContextCurrent(context));
	}
#endif

	for(int32 i = 0; i < maxChannels; ++i)
	{
		SoundChannel * ch = new SoundChannel();
		channelsPool.push_back(ch);
	}


	groupFX = new SoundGroup();
	groupMusic = new SoundGroup();
}

SoundSystem::~SoundSystem()
{
	SafeDelete(groupMusic);
	SafeDelete(groupFX);

	for(int32 i = 0; i < maxChannels; ++i)
	{
		SoundChannel * ch = channelsPool[i];
		delete(ch);
	}
#ifdef __DAVASOUND_AL__
	alcMakeContextCurrent(0);
	alcDestroyContext(context);
	alcCloseDevice(device);
#endif
}

SoundChannel * SoundSystem::FindChannel(int32 priority)
{
	Deque<SoundChannel*>::iterator it;
	Deque<SoundChannel*>::iterator itEnd = channelsPool.end();
	for(it = channelsPool.begin(); it != itEnd; ++it)
	{
		SoundChannel * ch = *it;
		if(SoundChannel::STATE_FREE == ch->GetState())
		{
			return ch;
		}
	}

	for(it = channelsPool.begin(); it != itEnd; ++it)
	{
		SoundChannel * ch = *it;
		if(ch->GetProirity() < priority)
		{
			ch->Stop();
			return ch;
		}
	}

	return 0;
}

void SoundSystem::Update()
{
	Deque<SoundChannel*>::iterator it;
	Deque<SoundChannel*>::iterator itEnd = channelsPool.end();
	for(it = channelsPool.begin(); it != itEnd; ++it)
	{
		SoundChannel * ch = *it;
		if(SoundChannel::STATE_PLAYING == ch->GetState())
		{
			ch->Update();
		}
	}

	List<SoundInstance*>::iterator sit = soundInstances.begin();
	List<SoundInstance*>::iterator sEnd = soundInstances.end();
	while(sit != sEnd)
	{
		if(!(*sit)->Update())
		{
			sit = soundInstances.begin();
			continue;
		}
		++sit;
	}
}

void SoundSystem::AddSoundInstance(SoundInstance * soundInstance)
{
	soundInstances.push_back(soundInstance);
}

void SoundSystem::RemoveSoundInstance(SoundInstance * soundInstance)
{
	soundInstances.remove(soundInstance);
}

void SoundSystem::Suspend()
{
	Deque<SoundChannel*>::iterator it;
	Deque<SoundChannel*>::iterator itEnd = channelsPool.end();
	for(it = channelsPool.begin(); it != itEnd; ++it)
	{
		SoundChannel * ch = *it;
		if(SoundChannel::STATE_PLAYING == ch->GetState())
		{
			ch->Pause(true);
		}
	}
#ifdef __DAVASOUND_AL__
	alcSuspendContext(context);
#endif
}

void SoundSystem::Resume()
{
#ifdef __DAVASOUND_AL__
	alcProcessContext(context);
	Deque<SoundChannel*>::iterator it;
	Deque<SoundChannel*>::iterator itEnd = channelsPool.end();
	for(it = channelsPool.begin(); it != itEnd; ++it)
	{
		SoundChannel * ch = *it;
		if(SoundChannel::STATE_PAUSED == ch->GetState())
		{
			ch->Pause(false);
		}
	}
#endif
}

void SoundSystem::SetVolume(float32 _volume)
{
	volume = Clamp(_volume, 0.f, 1.f);
#ifdef __DAVASOUND_AL__
	AL_VERIFY(alListenerf(AL_GAIN, volume));
#endif
}

float32 SoundSystem::GetVolume()
{
	return volume;
}

SoundGroup		* SoundSystem::GroupFX()
{
	return groupFX;
}

SoundGroup		* SoundSystem::GroupMusic()
{
	return groupMusic;
}

};
