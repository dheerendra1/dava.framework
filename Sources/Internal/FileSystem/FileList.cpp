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
#include "FileSystem/FileList.h"
#include "Utils/Utils.h"

#if defined(__DAVAENGINE_MACOS__) || defined(__DAVAENGINE_IPHONE__)
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

#elif defined(__DAVAENGINE_WIN32__)

#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <direct.h>

#endif

namespace DAVA
{

FileList::FileList(const String & filepath)
{
	path = FileSystem::Instance()->SystemPathForFrameworkPath(filepath);
// Windows version
#if defined(__DAVAENGINE_WIN32__)

	//char tmp[_MAX_PATH];
	//_getcwd(tmp, _MAX_PATH);
	//Path = tmp;
	String prevDir = FileSystem::Instance()->GetCurrentWorkingDirectory();
	BOOL res = SetCurrentDirectoryA(path.c_str());

	if (res)
	{
		struct _finddata_t c_file;
		intptr_t hFile;
		FileEntry entry;

		if( (hFile = _findfirst( "*", &c_file )) != -1L )
		{
			do
			{
				entry.name = c_file.name;
				entry.size = c_file.size;
				entry.isDirectory = (_A_SUBDIR & c_file.attrib) != 0;
				fileList.push_back(entry);
				//Logger::Debug("filelist: %s %s", filepath.c_str(), entry.name.c_str());
			}
			while( _findnext( hFile, &c_file ) == 0 );

			_findclose( hFile );
		}
	}
	FileSystem::Instance()->SetCurrentWorkingDirectory(prevDir);

	//TODO add drives
	//entry.Name = "E:\\";
	//entry.isDirectory = true;
	//Files.push_back(entry);
#elif defined(__DAVAENGINE_MACOS__) || defined(__DAVAENGINE_IPHONE__)
	struct dirent **namelist;
	FileEntry entry;

	int32 n = scandir(path.c_str(), &namelist, 0, alphasort);
	if (n >= 0)
	{
		while(n--)
		{
			entry.name = namelist[n]->d_name;
			entry.size = 0;
			entry.isDirectory = namelist[n]->d_type == DT_DIR;
			fileList.push_back(entry);
			free(namelist[n]);
		}
		free(namelist);
	}
#endif 
	directoryCount = 0;
	fileCount = 0;
	for (int fi = 0; fi < GetCount(); ++fi)
	{
		if (IsDirectory(fi))
		{
			String filename = GetFilename(fi);
			if ((filename != ".") && (filename != ".."))
				directoryCount++;
		}else
			fileCount++;
	}
}

FileList::~FileList()
{

}

int32 FileList::GetCount()
{
	return (int32)fileList.size();
}	

int32 FileList::GetFileCount()
{
	return fileCount;
}

int32 FileList::GetDirectoryCount()
{
	return directoryCount;
}

const String & FileList::GetFilename(int32 index)
{
	DVASSERT((index >= 0) && (index < (int32)fileList.size()));
	return fileList[index].name;
}

const String & FileList::GetPathname(int32 index) 
{
	DVASSERT((index >= 0) && (index < (int32)fileList.size()));
	if (fileList[index].pathName.size() < fileList[index].name.size())
	{
		// create full name
		fileList[index].pathName = path;

		if (path.size() > 3)
			fileList[index].pathName.append("/");

		fileList[index].pathName.append(fileList[index].name);
	}

	return fileList[index].pathName; 
}

bool FileList::IsDirectory(int32 index) 
{
	DVASSERT((index >= 0) && (index < (int32)fileList.size()));
	return fileList[index].isDirectory;
}
	
bool FileList::IsNavigationDirectory(int32 index)
{
	DVASSERT((index >= 0) && (index < (int32)fileList.size()));
	//bool isDir = fileList[index].isDirectory;
	//if (isDir)
	//{
	if ((fileList[index].name == ".") || (fileList[index].name == ".."))return true;
	//}
	return false;
}

}; // end of namespace DAVA
