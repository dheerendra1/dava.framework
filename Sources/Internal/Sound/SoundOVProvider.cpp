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

#include "Sound/SoundOVProvider.h"
#include "FileSystem/File.h"
#include "Utils/StringFormat.h"

namespace DAVA
{

size_t OVReadFunc(void *ptr, size_t size, size_t nmemb, void *datasource)
{
	File * file = (File*)datasource;
	return (size_t)file->Read(ptr, (uint32)(size*nmemb));
}

int32 OVSeekFunc(void *datasource, ogg_int64_t offset, int whence)
{
	File::eFileSeek seek = File::SEEK_FROM_START;
	if(SEEK_CUR == whence)
	{
		seek = File::SEEK_FROM_CURRENT;
	}
	else if(SEEK_SET == whence)
	{
		seek = File::SEEK_FROM_START;
	}
	else if(SEEK_END == whence)
	{
		seek = File::SEEK_FROM_END;
	}
	File * file = (File*)datasource;
	bool res = file->Seek((int32)offset, seek);

	return (res ? 0 : -1);
}

int32 OVCloseFunc(void *datasource)
{
	File * file = (File*)datasource;
	file->Release();

	return 0;
}

long OVTellFunc(void *datasource)
{
	File * file = (File*)datasource;
	return (long)file->GetPos();
}

SoundOVProvider::SoundOVProvider(const String & _fileName)
:	SoundDataProvider(_fileName),
	file(0)
{

}

SoundOVProvider::~SoundOVProvider()
{
	if(file)
	{
		ov_clear(&oggFile);
	}
}

bool SoundOVProvider::Init()
{
	if(isInited)
		return true;

	SoundDataProvider::Init();

	file = File::Create(fileName, File::OPEN | File::READ);
	if(0 == file)
		return false;

	ov_callbacks callbacks;
	callbacks.read_func = OVReadFunc;
	callbacks.seek_func = OVSeekFunc;
	callbacks.close_func = OVCloseFunc;
	callbacks.tell_func = OVTellFunc;
	if(int32 result = ov_open_callbacks(file, &oggFile, 0, 0, callbacks) < 0)
	{
		Logger::Error("SoundOVProvider::Init ov_open_callbacks returned %d", result);
		return false;
	}

	vorbis_info * info = ov_info(&oggFile, -1);
	channelsCount = info->channels;
	sampleSize = 16;
	sampleRate = info->rate;

	//250 ms
	streamBufferSize = channelsCount*sampleRate*sampleSize/8;

	return true;
}

int32 SoundOVProvider::LoadData(int8 ** buffer, int32 desiredSize)
{
	bool infiniteLoad = false;
	static const int32 OV_CHUNK_SIZE = 4096;
	if(-1 == desiredSize)
	{
		infiniteLoad = true;
		desiredSize = OV_CHUNK_SIZE;
	}

	int8 * data = (int8*)Alloc(desiredSize);
	DVASSERT_MSG(data, "SoundOVProvider::LoadData Alloc failed");
	int32 size = 0;
	int32 currentSection;

	while(size < desiredSize)
	{
		int32 result = ov_read(&oggFile, (char*)(data + size), desiredSize - size, 0, 2, 1, &currentSection);

		if(result > 0)
		{
			size += result;
			if(infiniteLoad && size == desiredSize)
			{
				desiredSize += OV_CHUNK_SIZE;
				data = (int8*)Realloc(data, desiredSize);
				DVASSERT_MSG(data, Format("SoundOVProvider::LoadData Realloc failed for size %d", desiredSize));
			}
		}
		else if(result < 0)
		{
			Logger::Error("int32 SoundOVProvider::LoadData failed for file %s with code %d", fileName.c_str(), result);
			break;
		}	
		else
		{
			break;
		}
	}

	*buffer = data;

	return size;
}

void SoundOVProvider::Rewind()
{
	ov_raw_seek(&oggFile, 0);
}

};