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
#ifndef __LOGENGINE_ANSIFILESYSTEM_H__
#define __LOGENGINE_ANSIFILESYSTEM_H__

#include "IFileSystem.h"
#include "AnsiFileList.h"
#include "AnsiFile.h"
#include "ResourceArchive.h"

namespace Log
{
namespace IO
{

const	int32	FILE_SYSTEM_MAX_PATH	= 1024;

class	AnsiFileSystem : public IFileSystem
{
public:
	//! constructor
	AnsiFileSystem();
	//! destructor
	~AnsiFileSystem();

	//! get current working directory
	virtual const	char8	*	GetCurrentDirectory();

	//! set current working directory
	virtual bool				SetCurrentDirectory(const char8 * Directory);

	//! create directory
	virtual void				CreateDirectory(const char8 * DirectoryPath);

	//! \brief check if file with name \ref filename exists
	//! \param filename
	virtual bool				IsFileExists(const char8 * filename);

    //! Creates a list of files and directories in the current working directory 
	//! and returns it.
	//! \return a Pointer to the created IFileList is returned. After the list has been used
	//! it has to be deleted using delete.
	virtual IFileList*			EnumFiles();
	
	//! Creates file in current directory and 
	//! return pointer to the created file
	//! if file not created return NULL.
	virtual IFile *				CreateFile(const char8 * filename, uint32	attributes);

	//! \brief Function to create file in memory, this is the way to work with memory similar to files
	//! \param buffer buffer to read or write data
	//! \param size of buffer
	//! \attributes attributes of file \ref IO::eFileAttributes
	virtual IFile *				CreateMemoryFile(uint8 * buffer, uint32 size, uint32 attributes);


	//! \brief Function to attach ResourceArchive to filesystem
	//!
	//! \todo Put example of usage here (cant do it because dont know how) 
	//! 
	//! \param archiveName pathname or local filename of archive we want to attach
	//! \param attachPath path we attach our archive 
	virtual void				AttachArchive(const char8 * archiveName, const char8 * attachPath);


private:
	char8	WorkingDirectory[FILE_SYSTEM_MAX_PATH];

	struct ResourceArchiveItem
	{
		ResourceArchive * archive;
		String attachPath;
	};
		
	List<ResourceArchiveItem> resourceArchiveList;
};


}; // end of namespace IO
}; // end of namespace Log


#endif // __LOGENGINE_ANSIFILESYSTEM_H__




