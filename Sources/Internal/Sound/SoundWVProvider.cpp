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

#include "Sound/SoundWVProvider.h"
#include "FileSystem/File.h"
#include "Sound/SoundBuffer.h"

namespace DAVA
{

SoundWVProvider::SoundWVProvider(const String & _fileName)
:	SoundDataProvider(_fileName),
	file(0)
{

}

SoundWVProvider::~SoundWVProvider()
{
	SafeRelease(file);
}

bool SoundWVProvider::Init()
{
	if(isInited)
		return true;

	SoundDataProvider::Init();

	file = File::Create(fileName, File::OPEN | File::READ);
	WaveFormat waveFormat;
	if(0 == file)
		return false;

	if(sizeof(waveFormat) != file->Read(&waveFormat, sizeof(waveFormat)))
		return false;

	//"RIFF" 0x52494646
	if(0x46464952 != waveFormat.riffTag)
		return false;

	//"WAVE" 0x57415645
	if(0x45564157 != waveFormat.waveTag)
		return false;

	//"fmt" 0x666D7420
	if(0x20746d66 != waveFormat.fmtTag)
		return false;

	//standard format
	if(16 != waveFormat.fmtSize)
		return false;

	//PCM uncompressed
	if(1 != waveFormat.compression)
		return false;

	channelsCount = waveFormat.channels;
	sampleRate = waveFormat.sampleRate;
	sampleSize = waveFormat.bitsPerSample;

	//"data" 0x64617461
	if(0x61746164 != waveFormat.dataTag)
		return false;

	leftDataSize = dataSize = waveFormat.dataSize;

	//250 ms
	streamBufferSize = channelsCount*sampleRate*sampleSize/8/4;

	return true;
}

int32 SoundWVProvider::LoadData(int8 ** buffer, int32 desiredSize)
{
	if(-1 == desiredSize)
	{
		desiredSize = dataSize;
	}

	desiredSize = Min(desiredSize, leftDataSize);

	int8 * data = (int8*)Alloc(desiredSize);
	int32 actualSize = file->Read(data, desiredSize);
	leftDataSize -= actualSize;

	*buffer = data;

	return actualSize;
}

};