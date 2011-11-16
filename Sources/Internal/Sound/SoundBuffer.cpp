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

#include "Sound/SoundBuffer.h"
#include "Sound/SoundDataProvider.h"
#include "Sound/ALUtils.h"

namespace DAVA
{

Map<String, SoundBuffer*> soundBuffersMap;
SoundBuffer * SoundBuffer::CreateStatic(const String & fileName)
{
	SoundBuffer * ret = 0;

	Map<String, SoundBuffer*>::iterator it;
	it = soundBuffersMap.find(fileName);
	if (it != soundBuffersMap.end())
	{
		ret = it->second;
		ret->Retain();
		return ret;
	}

	ret = new SoundBuffer();
	ret->type = TYPE_STATIC;
	ret->fileName = fileName;
	soundBuffersMap[fileName] = ret;

	return ret;
}

SoundBuffer * SoundBuffer::CreateStreamed()
{
	SoundBuffer * ret = new SoundBuffer();
	ret->type = TYPE_STREAMED;

	return ret;
}

SoundBuffer * SoundBuffer::CreateEmpty()
{
	SoundBuffer * ret = new SoundBuffer();
	return ret;
}

SoundBuffer::SoundBuffer()
:	data(0)
{
#ifdef __DAVASOUND_AL__
	buffer = 0;
#endif
}

SoundBuffer::~SoundBuffer()
{
	Free(data);
}

int32 SoundBuffer::Release()
{
	if(GetRetainCount() == 1)
	{
		if(TYPE_STATIC == type)
		{
			soundBuffersMap.erase(fileName);
		}
	}

	return BaseObject::Release();
}

void SoundBuffer::FullFill(SoundDataProvider * provider)
{
	int32 actualSize = provider->LoadData(&data, -1);
#ifdef __DAVASOUND_AL__
	AL_VERIFY(alGenBuffers(1, &buffer));
	AL_VERIFY(alBufferData(buffer, provider->GetFormat(), data, actualSize, provider->GetSampleRate()));
#endif
	Free(data);
	data = 0;
}

#ifdef __DAVASOUND_AL__
ALuint SoundBuffer::GetALBuffer()
{
	return buffer;
}
#endif 

bool SoundBuffer::Fill(SoundDataProvider * provider, int32 size)
{
	int32 actualSize = provider->LoadData(&data, size);

	if(0 == actualSize)
	{
		Free(data);
		return false;
	}

#ifdef __DAVASOUND_AL__
	AL_VERIFY(alGenBuffers(1, &buffer));
	AL_VERIFY(alBufferData(buffer, provider->GetFormat(), data, actualSize, provider->GetSampleRate()));
#endif
	
	Free(data);
	data = 0;
	return true;
}

};