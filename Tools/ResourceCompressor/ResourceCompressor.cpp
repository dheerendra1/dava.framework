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
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cctype>

#include "ResourceArchive.h"
#include "AnsiFileSystem.h"

using namespace Log;
using namespace IO;

class HeaderNameMake
{
public:
	HeaderNameMake()
		: dot(false)
	{
	}

	char operator () (char c)
	{
		if (dot)return ' ';
		if (c == '/')return '_';
		if (c == '.')
		{
			dot = true;
			return ' ';
		}
		return c;
	}
private:
	bool dot;
};

class Packer
{
public:
	Packer(const String & _resourceName, const String & _packedDir, const String & _headerName)
		: resourceName(_resourceName), packedDir(_packedDir), headerName(_headerName), resourceCount(0)
	{
		fileSystem = Singleton<IO::IFileSystem>::Instance();
		
	}

	~Packer()
	{

	}
	

	void Pack(bool withPaths)
	{
		resourceArchive.StartResource(resourceName, withPaths, packedDir);
		PackAllFilesInDirectory(packedDir, ExtractName(packedDir));
		resourceArchive.FinishResource();
	

		std::cout << "Saving archive... " << std::endl;
		std::cout << "file count:" << resourceCount << std::endl;
			
		// saving process
		
		//std::ofstream headerFile;

		//headerFile.open(headerName.c_str());
		//
		//headerFile << "#ifdef __RESOURCE_H__" << std::endl;
		//headerFile << "#define __RESOURCE_H__" << std::endl << std::endl;
		
		List<String>::iterator name = fileNameList.begin();

		int32 resourceId = 0;
	
		int32 resourceRealSum = 0;
		int32 resourcePackedSum = 0;


		for (int file = 0; file < resourceCount; ++file)
		{
			int32 resourcePackedSize; 
			int32 resourceRealSize;
						
			if (-1 == resourceArchive.SaveProgress(&resourcePackedSize, &resourceRealSize))
			{
				printf("*** Resource Archive Error\n");
				std::cout << "file:" << * name;
				return;
			}

			resourceRealSum += resourceRealSize;
			resourcePackedSum += resourcePackedSize;

			// process information
			std::cout << "file:" << * name;
			std::cout << " size:" << resourceRealSize << " packed: " << resourcePackedSize << std::endl;
	

			// header file 
			String id = String("IDR") + *name;
			std::transform(id.begin(), id.end(), id.begin(), toupper);
			std::transform(id.begin(), id.end(), id.begin(), HeaderNameMake());
			//headerFile << "#define " << id << "\t\t" << resourceId ++ << std::endl;

			name++;
		}
		//headerFile << std::endl << std::endl;

		//headerFile << "#endif // __RESOURCE_H__" << std::endl;
		//headerFile.close();
		// pack summary
			
		std::cout << std::endl << "Summary: " << std::endl;
		std::cout << "size: " << resourceRealSum << std::endl;
		std::cout << "packed size: " << resourcePackedSum << std::endl;
		std::cout << "compression rate: " << ((float32)resourcePackedSum / (float32) resourceRealSum) << std::endl;
		
	}	

	String ExtractName(const String & name)
	{		
		String::size_type n = name.rfind("/");
		if (n == -1)return name;
		else
		{
			if (n == name.length())
				n = name.rfind("/", n - 1);

			return name.substr(n);
		}
	}
	
	void PackAllFilesInDirectory(const String & path, const String & redusedPath)
	{
		fileSystem->SetCurrentDirectory(path.c_str());
		IFileList * fileList = fileSystem->EnumFiles();
		for (int file = 0; file < fileList->GetCount(); ++file)
		{
			String filename = fileList->GetFilename(file);
			if (fileList->IsDirectory(file))
			{
				if ((filename != "..") && (filename != "."))
				{
					//
					std::cout << "Directory: " << filename << std::endl;
					
					PackAllFilesInDirectory(path + String("/") + filename, redusedPath + String("/") + filename);
				}
			}else
			{
				String pathname = path + String("/") + filename;
				String printPathName = redusedPath + String("/") + filename;
				
				std::cout << "file: " << printPathName << std::endl;
				fileNameList.push_back(printPathName);

				resourceArchive.AddFile(pathname);
				resourceCount++;
			}
		}
		fileList->Release();
	}

private:

	List<String>	fileNameList;

	String			resourceName;
	String			packedDir;
	String			headerName;
	IFileSystem *	fileSystem;
	ResourceArchive resourceArchive;
	int32			resourceCount;
};


int main(int argc, char* argv[])
{
	
	if (argc <= 2)
	{
		printf("Usage: resourcepacker.exe [-d] [-u] [resourceDirectoryPath] [archiveName]\n");
		printf("Written by Borodovsky Vitaliy (C) 2006.\n");
		printf("Example:\n");
		printf("resourcepacker.exe -d data data.re\n\t- pack directory data to archive data.re\n");
		printf("resourcepacker.exe -u data data.re\n\t- unpack archive data.re to directory data\n");
		printf("Options:\n");
		printf("\t -d don't include to archive relative pathnames\n");
		printf("\t -u unpack archive with contents\n");
		return 0;
	}
	
	bool	packPaths = true;
	bool	showMode = false;
	String	directoryPath;
	String  archiveName;
	String	headerName;
	
	int32 pathIndex = 1;
	
	for (int flag = 0; flag < 2; ++flag)
	{
		if (String(argv[pathIndex]) == "-d")
		{
			pathIndex++;
			packPaths = false;
		}
		else if (String(argv[pathIndex]) == "-u")
		{
			pathIndex++;
			showMode = true;
		}
	}
	

//	IEngineSystem * es = CreateConsoleEngine();
	new AnsiFileSystem();
	
	IFileSystem * fs = Singleton<IO::IFileSystem>::Instance();
	String programmPath = fs->GetCurrentDirectory();

	if (showMode)	// show mode of ResourcePacker tool
	{
		directoryPath = argv[pathIndex];	
		
		if (directoryPath.find(':') == -1)
		{
			directoryPath = programmPath + "/" + directoryPath;
		}

		if ((pathIndex + 1) > (argc - 1))
		{
			String::size_type pos = directoryPath.rfind('/');
			if (pos == -1)
			{
				archiveName = directoryPath + ".re";
			}else
			{
				archiveName = directoryPath.substr(pos + 1) + ".re";
			}
		}else
		{
			archiveName = argv[pathIndex + 1];
			String::size_type pos = archiveName.rfind('.');
			if (pos == -1)
			{
				headerName = archiveName;
				archiveName += ".re";
				headerName += ".h";
			}
		}


		printf("===================================================\n");
		printf("=== Unpacker started\n");
		printf("=== Unpack directory: %s\n", directoryPath.c_str());
		printf("=== Unpack archiveName: %s\n", archiveName.c_str());
		printf("===================================================\n");


		fs->CreateDirectory(directoryPath.c_str());

		ResourceArchive ra;
		ra.Open(programmPath + "/" + archiveName);
		
		

		for (int file = 0; file < ra.GetFileCount(); ++file)
		{
			String pathName = ra.GetResourcePathname(file);
			pathName = directoryPath + pathName;
			
			String::size_type pos = pathName.rfind('/');
			if (pos == -1)
			{
			}else
			{
				String dirCreate = pathName.substr(0, pos);
				fs->CreateDirectory(dirCreate.c_str());
			}

			int32 size = ra.LoadResource(file, 0);
			uint8 * data = new uint8[size];
			ra.LoadResource(file, data);
					
			IFile * unpackFile = fs->CreateFile(pathName.c_str(), IO::EFA_CREATE | IO::EFA_WRITE);
			if (unpackFile)
			{
				unpackFile->Write(data, size);
				SafeRelease(unpackFile);
			}
			else
			{
				
			}

			delete [] data;
		}

	}else	// archive mode of ResourcePacker tool
	{
		directoryPath = argv[pathIndex];	

		if (directoryPath.find(':') == -1)
		{
			directoryPath = programmPath + "/" + directoryPath;
		}


		if ((pathIndex + 1) > (argc - 1))
		{
			String::size_type pos = directoryPath.rfind('/');
			if (pos == -1)
			{
				archiveName = directoryPath + ".re";
				headerName = directoryPath + ".h";
			}else
			{
				archiveName = directoryPath.substr(pos + 1) + ".re";
				headerName = directoryPath.substr(pos + 1) + ".h";
			}
		}else
		{
			archiveName = argv[pathIndex + 1];
			String::size_type pos = archiveName.rfind('.');
			if (pos == -1)
			{
				headerName = archiveName;
				archiveName += ".re";
				headerName += ".h";
			}else
			{
				headerName = archiveName.substr(0, pos) + ".h";
			}
		}

		printf("===================================================\n");
		printf("=== Packer started\n");
		printf("=== Include relative filepaths %s\n", (packPaths == true)? ("enabled"):("disabled"));
		printf("=== Pack directory: %s\n", directoryPath.c_str());
		printf("=== Pack archiveName: %s\n", archiveName.c_str());
		printf("=== Pack headerName: %s\n", headerName.c_str());
		printf("===================================================\n");

		Packer * packer = new Packer(programmPath + "/" + archiveName, directoryPath, programmPath + "/" + headerName);

		packer->Pack(packPaths);

		delete packer;
		packer = 0;
	}
	

//	SafeRelease(es);

	return 0;
}