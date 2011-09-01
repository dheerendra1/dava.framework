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
#include "FileSystem/FileSystem.h"
#include "FileSystem/ResourceArchive.h"
#ifdef LINUX
#include "MacTypes.h"
#endif

namespace DAVA
{

ResourceArchive::ResourceArchive()
: archiveFile(0)
{
	fileSystem = FileSystem::Instance();
}

ResourceArchive::~ResourceArchive()
{
	SafeRelease(archiveFile);	
}


//! \brief function to create new resource archive
//! \param archiveName path to archive we want to save
//! \param packedCacheDir path to directory where we want to store temporary files or from where get whese files
void ResourceArchive::StartResource(const String & _archiveName, bool _withPaths, const String & _extrudePart, const String & _packedCacheDir)
{
	archiveFileName = _archiveName;
	extrudePart = _extrudePart;
	packedCacheDir = _packedCacheDir;
	withPaths = _withPaths;
	fileArray.clear();
}

//! \brief Add new file to archive. Can be called only between \ref StartResource and \ref SaveResource
//! \param resourceFile name of resourceFile to save
void ResourceArchive::AddFile(const String & resourceFile)
{
	fileArray.push_back(resourceFile);
}

//! \brief function to finish resource archive and prepare it to saving to disk
void ResourceArchive::FinishResource()
{
	saveResourceCounter = 0;

	for (int32 res = 0; res < (int32)fileArray.size(); ++res)
	{
		const String & fileName = fileArray[res];
		
		File * testOpen = File::Create(fileName, File::OPEN | File::READ);
		if(testOpen)
			saveResourceCounter = -1;
		SafeRelease(testOpen);
	}
}

//! \brief function to check progress of saving archive to disk
//! process of compression can be long so we need to have progress bar
//! \param resourcePackedSize
//! \return number from 0 to added resource file count. 
int32 ResourceArchive::SaveProgress(int32 * resourcePackedSize, int32 * resourceRealSize)
{
	if (saveResourceCounter == -1)
	{
		return -1;
	}

	if (saveResourceCounter == 0)
	{
		archiveFile = File::Create(archiveFileName, File::OPEN | File::READ);
		if (!archiveFile)
		{
			saveResourceCounter = -1;
			return -1;
		}
		header.signature[0] = 'R';
		header.signature[1] = 'A';
		header.signature[2] = '0';
		header.signature[3] = '0';

		header.version = 100;
		header.fileCount = (uint32)fileArray.size();
		header.flags |= (withPaths) ? (EHF_WITHPATHS) : (0);
			
		if (sizeof(Header) != archiveFile->Write(&header, sizeof(Header)))
		{
			// ERROR
			SafeRelease(archiveFile);
			saveResourceCounter = -1;
			return -1;
		}

		nodeArray.resize(header.fileCount);

		if (!WriteDictionary())
		{
			// ERROR
			SafeRelease(archiveFile);
			saveResourceCounter = -1;
			return -1;
		}

	}else if (archiveFile == 0)
	{
		// ERROR
		saveResourceCounter = -1;
		return -1;
	}


	if (saveResourceCounter < (int32)fileArray.size())
	{
		if (!PackResource(fileArray[saveResourceCounter], resourcePackedSize, resourceRealSize))
		{
			// ERROR
			SafeRelease(archiveFile);
			saveResourceCounter = -1;
			return -1;
		}
		saveResourceCounter++;
	}
	


	if (saveResourceCounter == (int32)fileArray.size())
	{
		if (!WriteDictionary())
		{
			// ERROR
			SafeRelease(archiveFile);
			saveResourceCounter = -1;
			return -1;
		}	
		SafeRelease(archiveFile);
	}

	return saveResourceCounter;
}

//! \brief function to get this archive file count
//! \return file count
int32 ResourceArchive::GetFileCount() const
{
	return header.fileCount;
}

//! \brief Get resource relative pathname
//! \param resourceIndex index of resource from what we want to get pathname
//! \return pathName or String("") if any error occurred
String ResourceArchive::GetResourcePathname(const uint32 resourceIndex) const
{
	if ( (resourceIndex < 0) && (resourceIndex >= header.fileCount))
		return "";

	return nodeArray[resourceIndex].pathName;
}

// Open code
bool ResourceArchive::Open(const String & archiveName)
{
	lastResourceIndex = -1;
	lastResourceName = "";

	archiveFile = File::Create(archiveName.c_str(), File::OPEN | File::READ);
	if (!archiveFile)return false;

	if (sizeof(Header) != archiveFile->Read(&header, sizeof(Header)))
	{
		SafeRelease(archiveFile);
		return false;
	}
	withPaths = header.flags & EHF_WITHPATHS;
	
	if (!ReadDictionary())return false;
		
	//
//	for (uint32 file = 0; file < header.fileCount; ++file)
//	{
//		printf("name:%s offset:%d size:%d\n", nodeArray[file].pathName.c_str(), nodeArray[file].filePosition, nodeArray[file].fileSize);
//		
//	}

	return true;
}


bool ResourceArchive::WriteDictionary()
{
	if (!archiveFile->Seek(sizeof(Header), File::SEEK_FROM_START))return false;
	


	for (uint32 node = 0; node < header.fileCount; ++node)
	{
		nodeArray[node].pathName = fileArray[node];
		String::size_type pos = nodeArray[node].pathName.find(extrudePart);
		if (pos != -1)
		{
			nodeArray[node].pathName = nodeArray[node].pathName.substr(extrudePart.length());
		}



		if (sizeof(uint32) != 
			archiveFile->Write(&nodeArray[node].filePosition, sizeof(uint32)))return false;
		
		if (sizeof(uint32) != 
			archiveFile->Write(&nodeArray[node].fileSize, sizeof(uint32)))return false;

		if (sizeof(uint32) != 
			archiveFile->Write(&nodeArray[node].packedFileSize, sizeof(uint32)))return false;

		if (sizeof(uint32) != 
			archiveFile->Write(&nodeArray[node].fileFlags, sizeof(uint32)))return false;
		
		if (withPaths)
			if (!archiveFile->WriteString(nodeArray[node].pathName))return false;
		
	}
	return true;
}


bool ResourceArchive::ReadDictionary()
{
	if (!archiveFile->Seek(sizeof(Header), File::SEEK_FROM_START))return false;

	nodeArray.resize(header.fileCount);

	for (uint32 node = 0; node < header.fileCount; ++node)
	{
		if (sizeof(uint32) != 
			archiveFile->Read(&nodeArray[node].filePosition, sizeof(uint32)))return false;

		if (sizeof(uint32) != 
			archiveFile->Read(&nodeArray[node].fileSize, sizeof(uint32)))return false;

		if (sizeof(uint32) != 
			archiveFile->Read(&nodeArray[node].packedFileSize, sizeof(uint32)))return false;

		if (sizeof(uint32) != 
			archiveFile->Read(&nodeArray[node].fileFlags, sizeof(uint32)))return false;

		if (withPaths)
		{
			char8 tmp[2048];
			if (!archiveFile->ReadString(tmp, sizeof(tmp)))return false;
			nodeArray[node].pathName = tmp;
		}
		
		nodeMap[nodeArray[node].pathName] = node;
	}
	return true;

}



// internal function to pack resource
bool ResourceArchive::PackResource(const String & resourceToPack, int32 * resourcePackedSize, int32 * resourceRealSize)
{
	DictionaryNode & dictNode = nodeArray[saveResourceCounter];
	dictNode.filePosition = archiveFile->GetPos();
	dictNode.fileFlags = 0;

	File * packFile = File::Create(resourceToPack, File::OPEN | File::READ);
	if (packFile)
	{
		int32 packFileSize = (int32)packFile->GetSize();	
		dictNode.fileSize = packFileSize;
	
		*resourcePackedSize = 0;
		*resourceRealSize = packFileSize;

		uint8 * data = new uint8[packFileSize];
			
		if (packFileSize != packFile->Read(data, packFileSize))return false;

		dictNode.packedFileSize = packFileSize;
		*resourcePackedSize = packFileSize;
		if (packFileSize != archiveFile->Write(data, packFileSize))return false;
		
		//printf("data:");
		//for (int pi = 0; pi < dictNode.packedFileSize; ++pi)
		//{
		//	printf("%x ", packedData[pi]);
		//}
		//printf("\n");


		delete [] data;
		data = 0;
//		delete [] packedData;
//		packedData = 0;
	}
	SafeRelease(packFile);
	return true;
}


bool ResourceArchive::UnpackResource(int32 resourceIndex, uint8 * data)
{
	//uint8 * packedData = new uint8[nodeArray[resourceIndex].packedFileSize];

	DictionaryNode node = nodeArray[resourceIndex];
	archiveFile->Seek(nodeArray[resourceIndex].filePosition, File::SEEK_FROM_START);
	archiveFile->Read(data, nodeArray[resourceIndex].packedFileSize);

	return false;
}

int32 ResourceArchive::LoadResource(const uint32 resourceIndex, void * data)
{
	if (!archiveFile)return -1;
	if ( (resourceIndex < 0) || (resourceIndex >= header.fileCount) )return -1;
	if (data == 0)return nodeArray[resourceIndex].fileSize;
	
	
	if (!UnpackResource(resourceIndex, (uint8 *) data))return -1;
	else return nodeArray[resourceIndex].fileSize;
}	

//! \brief function to load resource with relative path
//! \param pathName relative pathname of resource we want to load
//! \param data pointer to memory we want to store this resource (if null function return size of resource)
//! \return -1 if error occurred otherwise return size of data
int32	ResourceArchive::LoadResource(const String & pathName, void * data)
{	
	if (!withPaths)return -1;
	
	uint32 resourceIndex;
	if (lastResourceIndex != -1)
	{
		if (pathName == lastResourceName)
		{
			resourceIndex = lastResourceIndex;
		}
	}
	
	if (pathName != lastResourceName)
	{
		std::map<std::string,uint32>::iterator it = nodeMap.find(pathName);
		if ( it != nodeMap.end())
		{
			resourceIndex = nodeMap.find(pathName)->second;//FindPathnameIndex(pathName);
			lastResourceIndex = resourceIndex;
			lastResourceName = pathName;
		}
		else
		{
			resourceIndex = header.fileCount;
		}
	}
	return LoadResource(resourceIndex, data);
}

int32	ResourceArchive::FindPathnameIndex(const String & pathName)
{
	//! \todo make Hash of names to increase speed of search
	
	if (lastResourceIndex != -1)
	{
		if (pathName == lastResourceName)
		{
			return lastResourceIndex;
		}
	}

	for (uint32 res = 0; res < header.fileCount; ++res)
	{
		if (nodeArray[res].pathName == pathName)
		{
			lastResourceIndex = res;
			lastResourceName = pathName;
			return res;
		}
	}
	return header.fileCount; // return wrong value to return to user wrong value.
}


};