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

#ifndef __DAVAENGINE_SOUND_H__
#define __DAVAENGINE_SOUND_H__

#include "Base/BaseTypes.h"
#include "Base/BaseObject.h"

namespace DAVA
{

class SoundDataProvider;
class SoundChannel;
class SoundBuffer;
class SoundInstance;
class SoundGroup;

/**
	\ingroup sound
	\brief Sound
*/
class Sound : public BaseObject
{
public:

	/**
	 \enum Sound memory management types.
	 */
	enum eType
	{
		TYPE_STATIC = 0, //!< Whole data is loaded into memory when sound is created.
		TYPE_STREAMED,   //!< Only two buffers with summary size of 0.5sec are holded in memory.
		TYPE_MANAGED	 //!< Data is loaded into memory only before playback.
	};

	static Sound	* Create(const String & fileName, eType type, int32 priority = 0);
	static Sound	* CreateFX(const String & fileName, eType type, int32 priority = 0);
	static Sound	* CreateMusic(const String & fileName, eType type, int32 priority = 0);

	virtual SoundInstance	* Play();
	virtual void			Stop();
	virtual void			SetVolume(float32 volume); // [0..1]
	virtual float32			GetVolume();
	virtual void			SetLooping(bool looping);

	/**
		\brief Pause or resume playback.
		\param[in] pause pause(true) or resume(false) playback.
	*/
	virtual void			Pause(bool pause);
    
    virtual int32           Release();

	eType			GetType();

protected:
	Sound(const String & fileName, eType type, int32 priority = 0);
	virtual ~Sound();

	void Init();
	void PrepareStaticBuffer();
	void PrepareDynamicBuffers();
	void UpdateDynamicBuffers();

	void AddSoundInstance(SoundInstance * soundInstance);
	void RemoveSoundInstance(SoundInstance * soundInstance);

	List<SoundInstance*> soundInstances;
	String fileName;
	eType type;
	SoundDataProvider * provider;
	SoundBuffer * buffer;
	SoundBuffer * streamBuffer;
	int32 priority;
	float32 volume;
	bool looping;

	SoundGroup * group;
	void SetSoundGroup(SoundGroup * group);

friend class SoundChannel;
friend class SoundInstance;
friend class SoundGroup;
};

};

#endif //__DAVAENGINE_SOUND_H__