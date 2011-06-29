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

#ifndef __DAVAENGINE_SOUND_WV_PROVIDER_H__
#define __DAVAENGINE_SOUND_WV_PROVIDER_H__

#include "Sound/SoundDataProvider.h"

namespace DAVA
{
	
class File;
class SoundWVProvider : public SoundDataProvider
{
public:
	SoundWVProvider(const String & fileName);
	virtual ~SoundWVProvider();

	virtual bool Init();
	virtual int32 LoadData(int8 ** buffer, int32 desiredSize);
	virtual void Rewind() {};

private:
	File * file;

	// byte-align structures
#ifdef _MSC_VER
# pragma pack( push, packing )
# pragma pack( 1 )
# define PACK_STRUCT
#elif defined( __GNUC__ )
# define PACK_STRUCT __attribute__((packed))
#else
# error compiler not supported
#endif

	struct WaveFormat//non-compressed wave
	{
		uint32 riffTag;
		uint32 riffChunkSize;
		uint32 waveTag;
		uint32 fmtTag;
		uint32 fmtSize;
		uint16 compression;
		uint16 channels;
		uint32 sampleRate;
		uint32 bytesPerSecond;
		uint16 blockAlign;
		uint16 bitsPerSample;
		uint32 dataTag;
		uint32 dataSize;
	};

	// Default alignment
#ifdef _MSC_VER
# pragma pack( pop, packing )
#endif
#undef PACK_STRUCT
};

};

#endif //__DAVAENGINE_SOUND_WV_PROVIDER_H__