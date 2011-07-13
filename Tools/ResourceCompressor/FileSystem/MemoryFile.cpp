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
#include "MemoryFile.h"



namespace Log
{
namespace IO
{


MemoryFile::MemoryFile(uint8 * _data, uint32 _dataSize, uint32 _attributes)
: dataStart(_data), dataCurrent(_data), fileSize(_dataSize), attributes(_attributes), releaseBuffer(false)
{
	memoryFilename = Format("memoryfile_%p", dataStart);
}

MemoryFile::MemoryFile(uint32 _dataSize, uint32 _attributes)
: fileSize(_dataSize), attributes(_attributes), releaseBuffer(true)
{
	dataStart = new uint8[fileSize];
	dataCurrent = dataStart;
	memoryFilename = Format("memoryfile_%p", dataStart);
}


MemoryFile::~MemoryFile()
{
	if (releaseBuffer)
	{
		delete [] dataStart;
		dataStart = 0;
	}
}

//! \brief Get this file name
const char8 * MemoryFile::GetFilename()
{
	return memoryFilename.c_str();
}

//! \brief Get this file full pathname
const char8 * MemoryFile::GetPathname()
{
	return memoryFilename.c_str();
}

//! \brief Write [dataSize] bytes to this file from [pointerToData]
//! \param pointerToData function get data from this pointer
//! \param dataSize size of data we want to write
//! \return number of bytes actually written
uint32 MemoryFile::Write(void * pointerToData, uint32 writeDataSize)
{
	if (!(attributes & IO::EFA_WRITE))return 0;

	uint32 realWriteSize = writeDataSize;
	uint32 currentPos = GetPos();
	if (currentPos + writeDataSize >= fileSize)
	{
		realWriteSize -= (currentPos + writeDataSize - fileSize);
	}
	
	Memcpy(dataCurrent, pointerToData, realWriteSize);
	dataCurrent += realWriteSize;
	return realWriteSize;
}

//! \brief Read [dataSize] bytes from this file to [pointerToData] 
//! \param pointerToData function write data to this pointer
//! \param dataSize size of data we want to read
//! \return number of bytes actually read
uint32 MemoryFile::Read(void * pointerToData, uint32 readDataSize)
{
	if (!(attributes & IO::EFA_READ))return 0;

	uint32 realReadSize = readDataSize;
	uint32 currentPos = GetPos();
	if (currentPos + readDataSize >= fileSize)
	{
		realReadSize -= (currentPos + readDataSize - fileSize);
	}

	Memcpy(pointerToData, dataCurrent, realReadSize);
	dataCurrent += realReadSize;
	return realReadSize;
}

//! \brief Get current file position
uint32 MemoryFile::GetPos()
{
	return (uint32)(dataCurrent - dataStart);
}

//! \brief Get current file size if writing
//! \brief and get real file size if file for reading
uint32 MemoryFile::GetSize()
{
	return fileSize;
}

//! \brief Set current file position
//! \param position position to set
//! \param seekType \ref IO::eFileSeek flag to set type of positioning
//! \return true if successful otherwise false.
bool MemoryFile::Seek(uint32 position, uint32 seekType)
{
	int64 pos = 0;
	switch(seekType)
	{
	case EFS_START:
		pos = (int64)position;
		break;
	case EFS_CURRENT:
		pos = (int64)GetPos() + (int64)position;
		break;
	case EFS_END:
		pos = (int64)fileSize + (int64)position;
		break;
	};

	
	if (pos < 0)return false;
	if (pos > fileSize)return false;
	
	dataCurrent = dataStart + (uint32)pos;
	return true;
}

//! return true if end of file reached and false in another case
bool MemoryFile::IsEof()
{
	return ((dataStart + fileSize) == dataCurrent);
}

};
};