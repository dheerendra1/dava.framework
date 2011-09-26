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

#include "Sound/SoundChannel.h"
#include "Sound/ALUtils.h"
#include "Sound/Sound.h"
#include "Sound/SoundBuffer.h"

namespace DAVA
{

SoundChannel::SoundChannel()
:	state(STATE_FREE)
{
#ifdef __DAVASOUND_AL__
	AL_VERIFY(alGenSources(1, &source));
#endif
}

SoundChannel::~SoundChannel()
{
#ifdef __DAVASOUND_AL__
	AL_VERIFY(alDeleteSources(1, &source));
#endif
}

void SoundChannel::SetPriority(int32 newPriority)
{
	priority = newPriority;
}

int32 SoundChannel::GetProirity()
{
	return priority;
}

void SoundChannel::SetState(eState newState)
{
	state = newState;
}

SoundChannel::eState SoundChannel::GetState()
{
	return state;
}

void SoundChannel::Stop()
{
	if(STATE_PLAYING == state)
	{
		state = STATE_FREE;
#ifdef __DAVASOUND_AL__
		AL_VERIFY(alSourceStop(source));
		if(Sound::TYPE_STREAMED == buddySound->GetType())
		{
			ALint buffersLeft = 0;
			AL_VERIFY(alGetSourcei(source, AL_BUFFERS_PROCESSED, &buffersLeft));
			if(buffersLeft)
			{
				ALuint * bids = new ALuint[buffersLeft];
				AL_VERIFY(alSourceUnqueueBuffers(source, buffersLeft, bids));
			}
		}
		else
		{
			AL_VERIFY(alSourcei(source, AL_BUFFER, 0));
		}
#endif //__DAVASOUND_AL__
	}
}

void SoundChannel::Play(Sound * sound, bool _looping)
{
	buddySound = sound;
	looping = _looping;
    
	if(Sound::TYPE_STATIC == buddySound->GetType())
	{
		PlayStatic();
	}
	else if(Sound::TYPE_STREAMED == buddySound->GetType())
	{
		PlayStreamed();
	}
	state = STATE_PLAYING;
}

void SoundChannel::Update()
{
	if(Sound::TYPE_STATIC == buddySound->GetType())
	{
		UpdateStatic();
	}
	else if(Sound::TYPE_STREAMED == buddySound->GetType())
	{
		UpdateStreamed();
	}
}

void SoundChannel::PlayStatic()
{
#ifdef __DAVASOUND_AL__
	AL_VERIFY(alSourcei(source, AL_BUFFER, buddySound->buffer->GetALBuffer()));
	AL_VERIFY(alSourcei(source, AL_LOOPING, looping));
	AL_VERIFY(alSourcePlay(source));
#endif
}

void SoundChannel::PlayStreamed()
{
#ifdef __DAVASOUND_AL__
	ALuint bid = buddySound->buffer->GetALBuffer();
	ALuint bid2 = buddySound->streamBuffer->GetALBuffer();
	AL_VERIFY(alSourceQueueBuffers(source, 1, &bid));
	AL_VERIFY(alSourceQueueBuffers(source, 1, &bid2));
	AL_VERIFY(alSourcei(source, AL_LOOPING, false));
	AL_VERIFY(alSourcePlay(source));
#endif
}

void SoundChannel::UpdateStatic()
{
#ifdef __DAVASOUND_AL__
	ALint alState;
	AL_VERIFY(alGetSourcei(source, AL_SOURCE_STATE, &alState));
	if(alState == AL_PLAYING)
	{
		state = STATE_PLAYING;
	}
	else if(alState == AL_PAUSED)
	{
		state = STATE_PAUSED;
	}
	else
	{
		state = STATE_FREE;
	}
	if(STATE_FREE == state)
	{
		AL_VERIFY(alSourcei(source, AL_BUFFER, 0));
	}
#endif
}

void SoundChannel::UpdateStreamed()
{
#ifdef __DAVASOUND_AL__
	ALint alState;
	AL_VERIFY(alGetSourcei(source, AL_SOURCE_STATE, &alState));
	if(alState == AL_PLAYING)
	{
		state = STATE_PLAYING;
	}
	else if(alState == AL_PAUSED)
	{
		state = STATE_PAUSED;
	}
	else
	{
		state = STATE_FREE;
	}
	if(STATE_FREE == state)
	{
		return;
	}

	ALint buffersProcessed = 0;
	AL_VERIFY(alGetSourcei(source, AL_BUFFERS_PROCESSED, &buffersProcessed));
	if(buffersProcessed)
	{
		ALuint bid;
		AL_VERIFY(alSourceUnqueueBuffers(source, 1, &bid));
		buddySound->UpdateDynamicBuffers();
		bid = buddySound->streamBuffer->GetALBuffer();
		AL_VERIFY(alSourceQueueBuffers(source, 1, &bid));
	}
#endif
}

void SoundChannel::Pause(bool pause)
{
#ifdef __DAVASOUND_AL__
	if(pause)
	{
		AL_VERIFY(alSourcePause(source));
	}
	else
	{
		ALint state;
		AL_VERIFY(alGetSourcei(source, AL_SOURCE_STATE, &state));
		if(AL_PAUSED == state)
		{
			AL_VERIFY(alSourcePlay(source));
		}

	}
#endif
}

Sound	* SoundChannel::GetBuddySound()
{
	return buddySound;
}

void SoundChannel::SetVolume(float32 _volume)
{
	volume = Clamp(_volume, 0.f, 1.f);
#ifdef __DAVASOUND_AL__
	AL_VERIFY(alSourcef(source, AL_GAIN, volume));
#endif
}

float32 SoundChannel::GetVolume()
{
	return volume;
}



};
