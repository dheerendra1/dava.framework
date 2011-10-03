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

#include "Sound/Sound.h"
#include "Sound/SoundWVProvider.h"
#if defined(__DAVAENGINE_WIN32__) || defined(__DAVAENGINE_MACOS__)
#include "Sound/SoundOVProvider.h"
#endif
#include "Sound/SoundBuffer.h"
#include "Sound/SoundInstance.h"
#include "Sound/SoundSystem.h"
#include "Sound/SoundChannel.h"
#include "Sound/SoundGroup.h"
#if defined(__DAVAENGINE_IPHONE__)
#include "Sound/MusicIos.h"
#endif

namespace DAVA
{

Sound * Sound::Create(const String & fileName, eType type, int32 priority)
{
	Sound * sound = new Sound(fileName, type, priority);
	sound->Init();
	return sound;
}

Sound	* Sound::CreateFX(const String & fileName, eType type, int32 priority /*= 0*/)
{
	Sound * sound = new Sound(fileName, type, priority);
	SoundSystem::Instance()->GroupFX()->AddSound(sound);
	sound->Init();
	return sound;
}

Sound	* Sound::CreateMusic(const String & fileName, eType type, int32 priority /*= 0*/)
{
#if defined(__DAVAENGINE_IPHONE__)
    Sound * sound = new MusicIos(fileName);
    SoundSystem::Instance()->GroupMusic()->AddSound(sound);
    return sound;
#else
	Sound * sound = new Sound(fileName, type, priority);
	SoundSystem::Instance()->GroupMusic()->AddSound(sound);
	sound->Init();
	return sound;
#endif //#if defined(__DAVAENGINE_IPHONE__)
}

Sound::Sound(const String & _fileName, eType _type, int32 _priority)
:	fileName(_fileName),
	type(_type),
	buffer(0),
	streamBuffer(0),
	priority(_priority),
	provider(0),
	volume(1.f),
	looping(false),
	group(0)
{
	
}

Sound::~Sound()
{
	if(group)
	{
		group->RemoveSound(this);
	}

	SafeRelease(buffer);
	SafeRelease(streamBuffer);
	SafeDelete(provider);
}

void Sound::Init()
{
	int32 strLength = (int32)fileName.length();
	String ext = fileName.substr(strLength-4, strLength);
	if(".wav" == ext)
	{
		provider = new SoundWVProvider(fileName);
	}
#if defined(__DAVAENGINE_WIN32__) || defined(__DAVAENGINE_MACOS__)
	else if(".ogg" == ext)
	{
		provider = new SoundOVProvider(fileName);
	}
#endif

	if(TYPE_STATIC == type)
	{
		PrepareStaticBuffer();
	}
}

void Sound::PrepareStaticBuffer()
{
	buffer = SoundBuffer::CreateStatic(fileName);
	if(1 == buffer->GetRetainCount()) 
	{
		provider->Init();
		buffer->FullFill(provider);
	}
}

void Sound::PrepareDynamicBuffers()
{
	buffer = SoundBuffer::CreateStreamed();
	buffer->Fill(provider, provider->GetStreamBufferSize());
	streamBuffer = SoundBuffer::CreateStreamed();
	streamBuffer->Fill(provider, provider->GetStreamBufferSize());
}

void Sound::UpdateDynamicBuffers()
{
	buffer->Release();
	buffer = streamBuffer;
	streamBuffer = SoundBuffer::CreateStreamed();
	bool filled = streamBuffer->Fill(provider, provider->GetStreamBufferSize());
	if(!filled)
	{
		if(looping)
		{
			provider->Rewind();
			streamBuffer->Release();
			streamBuffer = SoundBuffer::CreateStreamed();
			streamBuffer->Fill(provider, provider->GetStreamBufferSize());
		}
		else
		{
			streamBuffer->Release();
			streamBuffer = SoundBuffer::CreateEmpty();
		}
	}
}

SoundInstance * Sound::Play()
{
	if(TYPE_STREAMED == type && soundInstances.size())
	{
		return soundInstances.front();
	}

	SoundChannel * ch = SoundSystem::Instance()->FindChannel(priority);
	if(!ch)
	{
		return 0;
	}

	if(TYPE_STREAMED == type)
	{
		provider->Init();
		provider->Rewind();
		PrepareDynamicBuffers();
	}

	SoundInstance * inst = new SoundInstance();
	inst->buddyChannel = ch;
	AddSoundInstance(inst);
	ch->SetVolume(volume);
	ch->Play(this, looping);
	return inst;
}

Sound::eType Sound::GetType()
{
	return type;
}

void Sound::SetVolume(float32 _volume)
{
	volume = Clamp(_volume, 0.f, 1.f);
	List<SoundInstance*>::iterator sit;
	List<SoundInstance*>::iterator sitEnd = soundInstances.end();
	for(sit = soundInstances.begin(); sit != sitEnd; ++sit)
	{
		(*sit)->SetVolume(volume);
	}
	
}

float32 Sound::GetVolume()
{
	return volume;
}

void Sound::AddSoundInstance(SoundInstance * soundInstance)
{
	soundInstances.push_back(soundInstance);
}

void Sound::RemoveSoundInstance(SoundInstance * soundInstance)
{
	soundInstances.remove(soundInstance);
}

void Sound::SetLooping(bool _looping)
{
	looping = _looping;
}

void Sound::Stop()
{
	List<SoundInstance*>::iterator sit;
	List<SoundInstance*>::iterator sitEnd = soundInstances.end();
	for(sit = soundInstances.begin(); sit != sitEnd; ++sit)
	{
		(*sit)->Stop();
	}
	soundInstances.clear();

	if(TYPE_STREAMED == type)
	{
		SafeRelease(buffer);
		SafeRelease(streamBuffer);
	}
}

void Sound::SetSoundGroup(SoundGroup * _group)
{
	group = _group;
}

int32 Sound::Release()
{
    if(GetRetainCount() == 1)
    {
        Stop();
    }
    return BaseObject::Release();
}

void Sound::Pause(bool pause)
{
	List<SoundInstance*>::iterator sit;
	List<SoundInstance*>::iterator sitEnd = soundInstances.end();
	for(sit = soundInstances.begin(); sit != sitEnd; ++sit)
	{
		(*sit)->Pause(pause);
	}
}

};