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
        * Created by Vitaliy Borodovsky 
=====================================================================================*/
#include "FileSystem/DynamicMemoryFile.h"
#include "Utils/StringFormat.h"

namespace DAVA 
{

DynamicMemoryFile * DynamicMemoryFile::Create(const int8 * data, int32 dataSize, uint32 attributes)
{
	DynamicMemoryFile *fl = new DynamicMemoryFile();
	fl->filename = Format("memoryfile_%p", fl);
	fl->Write(data, dataSize);
	fl->fileAttributes = attributes;
	fl->currentPtr = 0;
	
	return fl;
}

DynamicMemoryFile * DynamicMemoryFile::Create(uint32 attributes)
{
	DynamicMemoryFile *fl = new DynamicMemoryFile();
	fl->fileAttributes = attributes;
	fl->filename = Format("memoryfile_%p", fl);
	
	return fl;
}

DynamicMemoryFile::DynamicMemoryFile()
:File()
{
	currentPtr = 0;
	fileAttributes = File::WRITE;
}

DynamicMemoryFile::~DynamicMemoryFile()
{
	
}
	
void * DynamicMemoryFile::GetData()
{
	if(!data.size())
	{
		return NULL;
	}
	return &(data[0]);
}



const char8 * DynamicMemoryFile::GetFilename()
{
	return filename.c_str();
}

const char8 * DynamicMemoryFile::GetPathname()
{
	return filename.c_str();
}

uint32 DynamicMemoryFile::Write(const void * pointerToData, uint32 dataSize)
{
	if (!(fileAttributes & File::WRITE) && !(fileAttributes & File::APPEND))
	{
		return 0;
	}
	
	if(data.size() < currentPtr + dataSize)
	{
		data.resize(currentPtr + dataSize);
	}
	Memcpy(&(data[currentPtr]), pointerToData, dataSize);
	currentPtr += dataSize;
	
	return dataSize;
}

uint32 DynamicMemoryFile::Read(void * pointerToData, uint32 dataSize)
{
	if (!(fileAttributes & File::READ))
	{
		return 0;
	}
	
	uint32 realReadSize = dataSize;
	int32 size = data.size();
	if (currentPtr + realReadSize > size)
	{
		realReadSize = size - currentPtr;
	}
	if(realReadSize)
	{
		Memcpy(pointerToData, &(data[currentPtr]), realReadSize);
		currentPtr += realReadSize;

		return realReadSize;
	}
	
	return 0;
}

uint32 DynamicMemoryFile::GetPos()
{
	return currentPtr;
}

uint32 DynamicMemoryFile::GetSize()
{
	return (uint32)data.size();
}

bool DynamicMemoryFile::Seek(int32 position, uint32 seekType)
{
	int64 pos = 0;
	switch(seekType)
	{
		case SEEK_FROM_START:
			pos = (int64)position;
			break;
		case SEEK_FROM_CURRENT:
			pos = (int64)GetPos() + (int64)position;
			break;
		case SEEK_FROM_END:
			pos = (int64)GetSize() - 1 + (int64)position;
			break;
		default:
			return false;
	};
	
	
	if (pos < 0)return false;
	if (pos >= GetSize())return false;
	
	currentPtr = pos;
	return true;
	
}

bool DynamicMemoryFile::IsEof()
{
	return currentPtr >= data.size();
}
	
};