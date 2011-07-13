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
#ifndef __LOGENGINE_FILESYSTEM_H__
#define __LOGENGINE_FILESYSTEM_H__

#include "BaseTypes.h"
#include "Singleton.h"

namespace Log
{
namespace IO
{


class IFile;
class IFileList;

//! \brief interface to work with filesystem 
//! you can get this class after creating IEngineSystem
//! using Global class
class	IFileSystem : public Singleton<IFileSystem>
{
public:
	//! destructor
	virtual ~IFileSystem() {};

	//! \brief get current working directory
	//! \return null-terminated string with current directory path
	virtual const	char8	*	GetCurrentDirectory() = 0;
	
	//! \brief set current working directory
	//! \param directory null-terminated string with directory path we want to set
	//! \return true if success otherwise false
	virtual bool				SetCurrentDirectory(const char8 * directory) = 0;
	
	
	//! \brief create directory path
	//! \param directoryPath path we want to create.
	//! example: directoryPath = "C:\\temp\\data1\\data2
	//! function will create C:\\temp, C:\\temp\\data1, C:\\temp\\data1\\data2 directories
	virtual void				CreateDirectory(const char8 * directoryPath) = 0;

	//! \brief check if file with name exists
	//! \param filename name of file we want to check
	virtual bool				IsFileExists(const char8 * filename) = 0;

    //! \brief	Creates a list of files and directories in the current working directory 
	//!			and returns it.
	//! \return a Pointer to the created IFileList is returned. After the list has been used
	//!			it has to be deleted using Release.
	virtual IFileList*			EnumFiles() = 0;

	//! \brief this function create or open file in current directory and return pointer to the created file
	//! \brief if file not created return NULL.
	//! \param filename name of file we want to create
	//! \param attributes attributes of file we want to create
	//! \return interface to this file
	virtual IFile *				CreateFile(const char8 * filename, uint32	attributes) = 0;


	//! \brief Function to create file in memory, this is the way to work with memory similar to files
	//! \param buffer buffer to read or write data
	//! \param size of buffer
	//! \attributes attributes of file \ref IO::eFileAttributes
	virtual IFile *				CreateMemoryFile(uint8 * buffer, uint32 size, uint32 attributes) = 0;


	//! \brief Function to attach ResourceArchive to filesystem
	//!
	//! \todo Put example of usage here (cant do it because dont know how) 
	//! 
	//! \param archiveName pathname or local filename of archive we want to attach
	//! \param attachPath path we attach our archive 
	virtual void				AttachArchive(const char8 * archiveName, const char8 * attachPath) = 0;


};


}; // end of namespace IO
}; // end of namespace Log


#endif // __LOGENGINE_FILESYSTEM_H__

