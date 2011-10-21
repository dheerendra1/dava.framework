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
#include "FileSystem/File.h"
#include "FileSystem/FileSystem.h"

#include "FileSystem/DynamicMemoryFile.h"

#if defined(__DAVAENGINE_ANDROID__)


namespace DAVA
{

File * File::CreateFromSystemPath(zip *APKArchive, const String &filename)
{
//	Logger::Debug("[File::CreateFromSystemPath] assets file %s", filename.c_str());

	if(!APKArchive)
	{
		Logger::Error("[File::CreateFromSystemPath] APKArchive is NULL");
		return NULL;
	}

	zip_file * zipfile = zip_fopen(APKArchive, filename.c_str(), 0);
	if(zipfile)
	{
		DynamicMemoryFile *fileInstance = NULL;
		struct zip_stat fileStat = {0};
		int retCode = zip_stat(APKArchive, filename.c_str(), ZIP_FL_NOCASE, &fileStat);
//		Logger::Debug("[File::CreateFromSystemPath] retCode = %d", retCode);
		if(0 == retCode)
		{
			int32 dataSize = fileStat.size;
			uint8 *data = new uint8[dataSize];

			uint32 readSize = zip_fread(zipfile, data, dataSize);
			if(readSize != dataSize)
			{
				Logger::Error("[File::CreateFromSystemPath] error of reading from file, readSize = %d, datasize = %d", readSize, dataSize);
			}

			fileInstance = DynamicMemoryFile::Create((int8*)data, readSize, File::READ); 
//			Logger::Debug("[File::CreateFromSystemPath] file created, size = %d", readSize);
		}
		
		zip_fclose(zipfile);
		return fileInstance;
	}
	else
	{
		Logger::Error("[File::CreateFromSystemPath] can't open file %s", filename.c_str());
	}

	Logger::Error("[File::CreateFromSystemPath] returns NULL");
	return NULL;
}

}

#endif //#if defined(__DAVAENGINE_ANDROID__)
