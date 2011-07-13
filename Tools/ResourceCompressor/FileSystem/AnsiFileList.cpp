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
#include "AnsiFileList.h"

#ifdef LINUX
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#endif

#ifdef WIN32
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <direct.h>
#endif

namespace Log
{
namespace IO
{

AnsiFileList::AnsiFileList()
{
	// Windows version
	#ifdef WIN32

	char tmp[_MAX_PATH];
	_getcwd(tmp, _MAX_PATH);
	Path = tmp;

	struct _finddata_t c_file;
	intptr_t hFile;
	FileEntry entry;

	if( (hFile = _findfirst( "*", &c_file )) != -1L )
	{
		do
		{
			entry.Name = c_file.name;
			entry.Size = c_file.size;
			entry.IsDirectory = (_A_SUBDIR & c_file.attrib) != 0;
			FileList.push_back(entry);
		}
		while( _findnext( hFile, &c_file ) == 0 );

		_findclose( hFile );
	}

	//TODO add drives
	//entry.Name = "E:\\";
	//entry.isDirectory = true;
	//Files.push_back(entry);
	#endif
	
	// --------------------------------------------
	// Linux version
	#ifdef LINUX
	struct dirent **namelist;
	FileEntry entry;

	int32 n = scandir(".", &namelist, 0, alphasort);
	if (n >= 0)
	{
		while(n--)
		{
			entry.Name = namelist[n]->d_name;
			entry.Size = 0;
			entry.IsDirectory = namelist[n]->d_type == DT_DIR;
			FileList.push_back(entry);
			free(namelist[n]);
		}
		free(namelist);
	}
	#endif 
}
AnsiFileList::~AnsiFileList()
{

}

int32 AnsiFileList::GetCount()
{
	return (int32 )FileList.size();
}	

const char8* AnsiFileList::GetFilename(int32 Index)
{
	if ((Index < 0) || (Index > (int32)FileList.size()))return 0; 
	return FileList[Index].Name.c_str();
}

const char8* AnsiFileList::GetPathname(int32 Index) 
{
	if ((Index < 0) || (Index > (int32)FileList.size()))return 0; 
	if (FileList[Index].PathName.size() < FileList[Index].Name.size())
	{
		// create full name
		FileList[Index].PathName = Path;

		if (Path.size() > 3)
			FileList[Index].PathName.append("/");

		FileList[Index].PathName.append(FileList[Index].Name);
	}

	return FileList[Index].PathName.c_str(); 
}

bool AnsiFileList::IsDirectory(int32 Index) 
{
	if ((Index < 0) || (Index > (int32)FileList.size()))return 0; 
	return FileList[Index].IsDirectory;
}

}; // end of namespace IO
}; // end of namespace Log
