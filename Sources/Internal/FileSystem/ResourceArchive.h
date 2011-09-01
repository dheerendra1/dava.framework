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
#ifndef __DAVAENGINE_RESOURCE_ARCHIVE_H__
#define __DAVAENGINE_RESOURCE_ARCHIVE_H__

#include "Base/BaseTypes.h"
#include "FileSystem/Bitstream.h"
#include "Base/BaseObject.h"

namespace DAVA
{
/*
	Resource file format
	
	header
	-------
	uint8  signature[4];
	uint32 version;
	uint32 fileCount;
	uint32 flags;	
	
	dictionary
	------------
	for i = [0, fileCount)
		uint32 filePosition;
		uint32 fileSize;
		uint32 packedFileSize;
		uint32 fileFlags;
		String pathName;
	
	compressed files
	----------------
	for i = [0, fileCount)
		uint8 * fileData[packedFileSize[i]];


	compression method LZSS.
	compression rate (about 2 - 3 times)
	decompression speed O(n) 
*/

class FileSystem;
//! \brief class to work with resource archives
class ResourceArchive : public BaseObject
{
public:
	ResourceArchive();
	virtual ~ResourceArchive();


	// Loading resources functions
	

	//! \brief function to get this archive file count
	//! \return file count
	int32	GetFileCount() const;
	


	//! \brief Get resource relative pathname
	//! \param resourceIndex index of resource from what we want to get pathname
	//! \return pathName or String("") if any error occurred
	String GetResourcePathname(const uint32 resourceIndex) const;


	//! \brief function to load resource with relative path
	//! \param pathName relative pathname of resource we want to load
	//! \param data pointer to memory we want to store this resource (if null function return size of resource)
	//! \return -1 if error occurred otherwise return size of data
	int32	LoadResource(const String & pathName, void * data);


	//! \brief function to open resource archive
	//! \param resourceIndex we want to open
	//! \param data pointer to memory we want to store this resource (if null function return size of resource)
	//! \return -1 if error occurred otherwise return size of data
	int32	LoadResource(const uint32 resourceIndex, void * data);

	//! \brief function to open resource archive
	//! \param archiveName path to archive we want to open
	//! \param withPaths can use relative paths to open resources
	bool	Open(const String & archiveName);

	// Saving resources functions

	//! \brief function to create new resource archive
	//! \param archiveName path to archive we want to save
	//! \param packedCacheDir path to directory where we want to store temporary files or from where get whese files
	//! \param withPaths save relative paths to open resources
	void	StartResource(const String & archiveName, bool withPaths = true, const String & extrudePart = String(""), const String & packedCacheDir = String(""));

	//! \brief Add new file to archive. Can be called only between \ref StartResource and \ref SaveResource
	//! \param resourceFile name of resourceFile to save
	void	AddFile(const String & resourceFile);
	
	//! \brief function to finish resource archive and prepare it to saving to disk
	void	FinishResource();
	
	//! \brief function to check progress of saving archive to disk
	//! process of compression can be long so we need to have progress bar
	//! \param resourcePackedSize
	//! \return if succeed returns number from 0 to added resource file count. 
	//!			else return -1.
	int32	SaveProgress(int32 * resourcePackedSize = 0, int32 * resourceRealSize = 0);

private:

	// internal function to pack resource
	bool	PackResource(const String & resourceToPack, int32 * resourcePackedSize, int32 * resourceRealSize);
	bool	UnpackResource(int32 fileIndex, uint8 * data);


	int32	FindPathnameIndex(const String & pathname);
	int32	lastResourceIndex;
	String	lastResourceName;

	bool	WriteDictionary();
	bool	ReadDictionary();

	enum eHeaderFlags
	{
		EHF_WITHPATHS = 0x01,
	};

	struct Header
	{
		uint8  signature[4];
		uint32 version;
		uint32 fileCount;
		uint32 flags;
	};
	

	enum eFileFlags
	{
		EFF_PACKED = 0x1, // if flag is set this file is packed, else this file is unpacked
	};

	struct DictionaryNode
	{
		uint32 filePosition;
		uint32 fileSize;
		uint32 packedFileSize;
		uint32 fileFlags;
		String pathName;
		//String relativePathName;
	};

	
	static const int32 OFFSET_BITS = 10;
	static const int32 WINDOW_SIZE = 1 << OFFSET_BITS;
	static const int32 MATCHLEN_BITS = OFFSET_BITS;
	
	// common information (for loading archive files & for saving arhchive files)
	Header					header;
	Vector<DictionaryNode>			nodeArray;
	std::map<std::string,uint32>	nodeMap;


	// save information
	int32					saveResourceCounter;
	Vector<String>			fileArray;
	String					archiveFileName;
	String					packedCacheDir;
	String					extrudePart;
	bool					withPaths;


	File					*archiveFile;
	// global influences
	FileSystem				* fileSystem;
};


};


#endif // __DAVAENGINE_RESOURCE_ARCHIVE_H__


