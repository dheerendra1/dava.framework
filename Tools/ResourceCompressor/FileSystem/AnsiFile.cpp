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
=====================================================================================*/

#include "AnsiFile.h"

namespace Log
{
namespace IO
{

AnsiFile::AnsiFile()
{
	file = NULL;
};

AnsiFile::~AnsiFile()
{
	if (file)
	{
		fclose(file);
		file = 0;
	}
}

bool AnsiFile::Create(const char8 * _filename, uint32	attributes)
{
	filename = _filename;
	file = NULL;
	size = 0;
	if ((attributes & EFA_CREATE) && (attributes & EFA_WRITE))
	{
		file = fopen(filename.c_str(),"wb");
		if (!file)return false;
	}

	if ((attributes & EFA_APPEND) && (attributes & EFA_WRITE))
	{
		file = fopen(filename.c_str(),"ab");
		if (!file)return false;
	}
	

	if ((attributes & EFA_OPEN) && (attributes & EFA_READ))
	{
		file = fopen(filename.c_str(),"rb");
		if (!file)return false;
		fseek(file, 0, SEEK_END);
		size = ftell(file);
		fseek(file, 0, SEEK_SET);
	}

	return true;
}


const char8 * AnsiFile::GetFilename()
{
	return filename.c_str();
}
    
const char8 * AnsiFile::GetPathname()
{
//	Log::Global::Log("[AnsiFile::GetPathname] not implemented; allways return NULL;");
	return 0;
}
	
uint32 AnsiFile::Write(void * pointerToData, uint32 dataSize)
{
	//! Do not change order fread return not bytes -- items
	uint32 lSize = (uint32) fwrite(pointerToData, 1, dataSize, file);
	size += lSize;
	return lSize;
}
	
uint32 AnsiFile::Read(void * pointerToData, uint32 dataSize)
{
	//! Do not change order fread return not bytes -- items
	return (uint32) fread(pointerToData, 1, dataSize, file);
}

//! Get current file position
uint32 AnsiFile::GetPos()
{
	if (!file) return 0;
	return (uint32) ftell(file);
}

//! Get file size
uint32	AnsiFile::GetSize()
{
	return size;
}

//! Set current file position
bool AnsiFile::Seek(uint32 position, uint32 seekType) 
{
	if (!file)return false;
	
	int realSeekType = 0;
	switch(seekType)
	{
	case EFS_START:
		realSeekType = SEEK_SET;
		break;
	case EFS_CURRENT:
		realSeekType = SEEK_CUR;
		break;
	case EFS_END:
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

//! return true if end of file reached and false in another case
bool AnsiFile::IsEof()
{
	return (feof(file) != 0);
}

}; // end of namespace IO
}; // end of namespace Log


