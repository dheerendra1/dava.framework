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

#ifndef __DAVAENGINE_SOUND_SYSTEM_H__
#define __DAVAENGINE_SOUND_SYSTEM_H__

#include "Base/Singleton.h"
#include "Base/BaseTypes.h"



namespace DAVA
{

class SoundChannel;
class SoundInstance;
class Sound;
class SoundGroup;
class SoundSystem : public Singleton<SoundSystem>
{
public:
	SoundSystem(int32 maxChannels);
	virtual ~SoundSystem();

	void			SetVolume(float32 volume); // [0..1]
	float32			GetVolume();

	void			AddSoundInstance(SoundInstance * soundInstance);
	void			RemoveSoundInstance(SoundInstance * soundInstance);
	SoundChannel	* FindChannel(int32 priority);
	void			Update();
	void			Suspend();
	void			Resume();

	SoundGroup		* GroupFX();
	SoundGroup		* GroupMusic();

protected:
	int32 maxChannels;
	Deque<SoundChannel*> channelsPool;
	List<SoundInstance*> soundInstances;

	float32			volume;

	SoundGroup		* groupFX;
	SoundGroup		* groupMusic;
};



};

#endif //__DAVAENGINE_SOUND_SYSTEM_H__
