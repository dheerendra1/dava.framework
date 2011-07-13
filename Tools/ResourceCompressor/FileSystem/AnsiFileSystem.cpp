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
#include "AnsiFileSystem.h"
//#include "Global.h"
#include "MemoryFile.h"
#include <sys/stat.h>

#ifdef WIN32
#include <direct.h> // for _chdir
#endif

#ifdef LINUX
#include <unistd.h>
#endif
 

namespace Log
{
namespace IO
{


AnsiFileSystem::AnsiFileSystem()
{
//	Global::Log("[AnsiFileSystem] create successfull\n");
}
AnsiFileSystem::~AnsiFileSystem()
{
	for (List<ResourceArchiveItem>::iterator ai = resourceArchiveList.begin();
		ai != resourceArchiveList.end(); ++ai)
	{
		ResourceArchiveItem & item = *ai;
		SafeRelease(item.archive);
	}
	resourceArchiveList.clear();

//	Global::Log("[AnsiFileSystem] release successfull\n");
}


const char8* AnsiFileSystem::GetCurrentDirectory()
{
#ifdef WIN32
	_getcwd(WorkingDirectory, FILE_SYSTEM_MAX_PATH);
	return WorkingDirectory;
#endif

#ifdef LINUX
	getcwd(WorkingDirectory, (size_t)FILE_SYSTEM_MAX_PATH);
	return WorkingDirectory;
#endif

	return 0; 
}

bool AnsiFileSystem::SetCurrentDirectory(const char8 * Directory)
{
#ifdef WIN32
	return (_chdir(Directory) == 0);
#endif

#ifdef LINUX
	return !(chdir(Directory) == 0);
#endif

	return false; 
}

void AnsiFileSystem::CreateDirectory(const char8 * directoryPath)
{
	String directory = directoryPath;

	List<String> directoryCreateList;
	
	directoryCreateList.push_back(directory);
	while(1)
	{
		String::size_type pos = directory.rfind('/');
		if (pos != -1)
		{
			String dirCreate = directory.substr(0, pos);
			directoryCreateList.push_front(dirCreate);
			directory = dirCreate;
		}else
		{
			break;
		}
	}
	for (List<String>::iterator dir = directoryCreateList.begin(); 
								dir != directoryCreateList.end(); ++dir)
	{
#ifdef WIN32
		mkdir((*dir).c_str());
#endif
#ifdef LINUX
		mkdir((*dir).c_str(), (S_IRWXU | S_IRWXG | S_IRWXO));
#endif
	}
}

IFileList* AnsiFileSystem::EnumFiles()
{
	return new AnsiFileList();
}

IFile * AnsiFileSystem::CreateFile(const char8 * filename, uint32	attributes)
{
	// find file in attached archives

	for (List<ResourceArchiveItem>::iterator ai = resourceArchiveList.begin();
		ai != resourceArchiveList.end(); ++ai)
	{
		ResourceArchiveItem & item = *ai;
			
		String filenamecpp = filename;

		String::size_type pos = filenamecpp.find(item.attachPath);
		if (pos == 0)
		{
			String relfilename = filenamecpp.substr(item.attachPath.length());
			uint32 size = item.archive->LoadResource(relfilename, 0);
			if ( size == -1 )
			{
				return 0;
			}
			MemoryFile * file = new MemoryFile(size, attributes);
			if (file)
			{
				item.archive->LoadResource(relfilename, file->GetPointer());
				return file;
			}
			SafeRelease(file);
			return 0;
		}
	}

	AnsiFile * file = new AnsiFile();
	if (file->Create(filename, attributes))return file;
	SafeRelease(file);
	return 0;
}

//! \brief Function to create file in memory, this is the way to work with memory similar to files
//! \param buffer buffer to read or write data
//! \param size of buffer
//! \attributes attributes of file \ref IO::eFileAttributes
IFile *	AnsiFileSystem::CreateMemoryFile(uint8 * buffer, uint32 size, uint32 attributes)
{
	if (0 == buffer)return 0;
	if (0 == size)return 0;
	return new MemoryFile(buffer, size, attributes);
}

void AnsiFileSystem::AttachArchive(const char8 * archiveName, const char8 * attachPath)
{	
	ResourceArchive * resourceArchive = new ResourceArchive();
	
	if (!resourceArchive->Open(archiveName)) 
	{
		delete resourceArchive;
		resourceArchive = 0;
		return;
	}
	ResourceArchiveItem item;
	item.attachPath = attachPath;
	item.archive = resourceArchive;
	resourceArchiveList.push_back(item);
};



//! \brief check if file with name \ref filename exists
//! \param filename
bool AnsiFileSystem::IsFileExists(const char8 * filename)
{	
	FILE * fp = fopen(filename, "rb");
	if (fp)
	{
		fclose(fp);
		return true;
	}
	return false;
}	




}; // end of namespace IO
}; // end of namespace Log