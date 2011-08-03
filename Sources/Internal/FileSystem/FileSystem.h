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
#ifndef __DAVAENGINE_FILESYSTEM_H__
#define __DAVAENGINE_FILESYSTEM_H__

#include "Base/BaseTypes.h"
#include "Base/Singleton.h"
#include "FileSystem/File.h"

/**
	\defgroup filesystem File System
 */
namespace DAVA 
{
class ResourceArchive;
/**
	\ingroup filesystem
	\brief FileSystem is a wrapper class that allow to perform all basic filesystem operations
	
	Class is platform dependent but it must used in all places where you want to be sure that portability is an issue

	Supported platforms:
		Windows, MacOS X, iPhone OS
 
	\todo add functions to enumerate files in directories to be full functional FileSystem
	\todo refactoring of utils and ~res:/ ~doc:/ access for the project files
	\todo add support for pack files
*/
class FileSystem : public Singleton <FileSystem>
{
public:
	FileSystem();
	virtual ~FileSystem();
	
	/**
		\brief Function to delete file from filesystem
		\param[in] filePath full path for the file we want to delete
		\returns true if deletion was successful
	 */
	virtual bool DeleteFile(const String & filePath);
	
	
	/*
		\brief Function to delete directory
		
		If isRecursive variable is false, function will succeed only in case if directory is empty.
	 
		\param[in] path full path to the directory you want to delete
		\param[in] isRecursive if true trying to delete all subfolders, if not just trying to delete this directory
		\returns true if this directory was deleted
	 */
	virtual bool DeleteDirectory(const String & path, bool isRecursive = true);

	
	/*
		\brief Deletes all files in given directory
		if isRecursive is set function walks into all child directories and delete files there also.
		This funciton do not delete directoris, it delete only files
		\param[in] isRecursive if true go into child directories and delete files there also, false by default
		\returns number of deleted files
	*/ 
	virtual uint32 DeleteDirectoryFiles(const String & path, bool isRecursive = false);

	enum eCreateDirectoryResult
	{
		DIRECTORY_CANT_CREATE = 0,
		DIRECTORY_EXISTS = 1,
		DIRECTORY_CREATED = 2,
	};
	/**
		\brief Function to create directory at filePath you've requested
		\param[in] filepath where you want to create a directory
		\returns true if directory created successfully
	 */
	virtual eCreateDirectoryResult CreateDirectory(const String & filePath, bool isRecursive = false);
	
	/**
		\brief Function to retrieve current working directory
		\returns current working directory
	 */
	virtual const String & GetCurrentWorkingDirectory();

	/**
		\brief Function to set current working directory
		\param[in] newWorkingDirectory new working directory to be set
		\returns true if directory set successfully
	 */
	virtual bool SetCurrentWorkingDirectory(const String & newWorkingDirectory);
	
	/**
		\brief Function to compute CRC32 
	 */
		
	/**
		\brief Return canonical path name of \a path.

		RealPath expands all symbolic links and resolves references to '/./', '/../' and extra '/' characters in
		the string named by path and returns the canonicalized absolute pathname.
		The resulting path will have no symbolic link, '/./' or '/../' components, also no trailing ones.
		Nor will it  end on a slash: if the result is the root then the returned path is empty,
		and unless the result is empty, it will always start with a slash.
	 */
	static String RealPath(const String & path);
	
	/**
		\brief Split path to file into path and filename
		\param[in] filepath inputpath to some file 
		\param[out] path path to the input file (always with trailing backslash character ('/').)
		\param[out] filename filename of the input file
	 */
	static void	SplitPath(const String & filepath, String & path, String & filename);

	/**
		\brief Extract extension from the file path
		Function returns extension with ".", so if you'll call GetExtension("filename.png") function will return ".png". 
		\returns extension of the file
	 */
	static String GetExtension(const String & filename);

	/**
		\brief Replace extension for the given filename
		
	 */
	static String ReplaceExtension(const String & filename, const String & newExt);

	
	/**
	 \brief Returns absolute system path from the framework specific path
	 parse paths with specific discs ~res:/ and ~doc:/
	 */
	const String & SystemPathForFrameworkPath(const String & frameworkPath);
	
	File *CreateFileForFrameworkPath(const String & frameworkPath, uint32 attributes);

	/**
		\brief Copies an existing file to a new file.
		\param[in] existingFile The name of an existing file.
		\param[out] newFile The name of the new file.
		\returns true if file was successfully copied, false otherwise
	*/
	virtual bool CopyFile(const String & existingFile, const String & newFile);
    
    
    /**
        \brief Read whole file contents into new buffer. 
        If function returns zero error happened and it haven't loaded the file
        After you'll finish using the date you should DELETE returned buffer using function SafeDeleteArray.  
     
        \param[in] pathname path to the file we want to read
        \param[out] fileSize
        \returns pointer to newly created buffer with file contents
     */ 
    uint8 * ReadFileContents(const String & pathname, uint32 & fileSize);

	/**
		\brief Function to attach ResourceArchive to filesystem
	
		\param[in] archiveName pathname or local filename of archive we want to attach
		\param[in] attachPath path we attach our archive 
	*/ 
	virtual void AttachArchive(const String & archiveName, const String & attachPath);
    
private:
	String tempRetPath;
	String currentWorkingDirectory;

	struct ResourceArchiveItem
	{
		ResourceArchive * archive;
		String attachPath;
	};

	List<ResourceArchiveItem> resourceArchiveList;

	friend class File;
};
	
};

#endif // __DAVAENGINE_FILESYSTEM_H__