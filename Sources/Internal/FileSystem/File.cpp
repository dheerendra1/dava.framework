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
#include "FileSystem/File.h"
#include "FileSystem/FileSystem.h"


namespace DAVA 
{
	
File::File()
{
	file = NULL;
};

File::~File()
{
	if (file)
	{
		fclose(file);
		file = 0;
	}
}
	
File * File::Create(const String &filePath, uint32 attributes)
{
	return FileSystem::Instance()->CreateFileForFrameworkPath(filePath, attributes);
}


File * File::CreateFromSystemPath(const String &filename, uint32	attributes)
{
	FILE * file = 0;
	uint32 size = 0;
	if ((attributes & File::OPEN) && (attributes & File::READ))
	{
		file = fopen(filename.c_str(),"rb");
		if (!file)return NULL;
		fseek(file, 0, SEEK_END);
		size = ftell(file);
		fseek(file, 0, SEEK_SET);
	}
	else if ((attributes & File::CREATE) && (attributes & File::WRITE))
	{
		file = fopen(filename.c_str(),"wb");
		if (!file)return NULL;
	}
	else if ((attributes & File::APPEND) && (attributes & File::WRITE))
	{
		file = fopen(filename.c_str(),"ab");
		if (!file)return NULL;
	}
	else 
	{
		return NULL;
	}


	File * fileInstance = new File();
	fileInstance->filename = filename;
	fileInstance->size = size;
	fileInstance->file = file;
	return fileInstance;
}

const char8 * File::GetFilename()
{
	return filename.c_str();
}

const char8 * File::GetPathname()
{
	Logger::Debug("[AnsiFile::GetPathname] not implemented; allways return NULL;");
	return 0;
}

uint32 File::Write(const void * pointerToData, uint32 dataSize)
{
	//! Do not change order fread return not bytes -- items
	uint32 lSize = (uint32) fwrite(pointerToData, 1, dataSize, file);
	size += lSize;
	return lSize;
}

uint32 File::Read(void * pointerToData, uint32 dataSize)
{
	//! Do not change order (1, dataSize), cause fread return count of size(2nd param) items
	//! May be performance issues
	return (uint32) fread(pointerToData, 1, dataSize, file);
}

uint32 File::ReadString(char8 * destinationBuffer, uint32 destinationBufferSize)
{
	uint32 writeIndex = 0;
	while(!IsEof())
	{
		uint8 currentChar;
		Read(&currentChar, 1);
		
		if (writeIndex < destinationBufferSize)
		{
			destinationBuffer[writeIndex] = currentChar;
		}else 
		{
			Logger::Warning("File::ReadString buffer size is too small for this string.");
		}
		writeIndex++;
		if(currentChar == 0)break;	
	}
	return writeIndex - 1;
}

uint32 File::ReadLine(void * pointerToData, uint32 bufferSize)
{
	uint8 *inPtr = (uint8*)pointerToData;
	while(!IsEof())
	{
		uint8 nextChar;
		Read(&nextChar, 1);
		if(nextChar == '\n')break;
		if(nextChar == 0)break;

		if(nextChar == '\r')
		{
			
			Read(&nextChar, 1);
			if(nextChar != '\n')
			{
				Seek(-1, File::SEEK_FROM_CURRENT);
			}
			break;
		}
		*inPtr = nextChar;
		inPtr++;
	}
	*inPtr = 0;

	return (uint32)(inPtr - (uint8*)pointerToData);
}

uint32 File::GetPos()
{
	if (!file) return 0;
	return (uint32) ftell(file);
}

uint32	File::GetSize()
{
	return size;
}

bool File::Seek(int32 position, uint32 seekType) 
{
	if (!file)return false;
	
	int realSeekType = 0;
	switch(seekType)
	{
		case SEEK_FROM_START:
			realSeekType = SEEK_SET;
			break;
		case SEEK_FROM_CURRENT:
			realSeekType = SEEK_CUR;
			break;
		case SEEK_FROM_END:
			realSeekType = SEEK_END;
			break;
		default:
			return false;
			break;
	}
	if (0 == fseek( file, position, realSeekType))
	{
		return true;
	}
	return false;
}

bool File::IsEof()
{
	return (feof(file) != 0);
}

}