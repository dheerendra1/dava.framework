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
#include "FileSystem/FileSystem.h"
#include "FileSystem/FileList.h"
#include "Debug/DVAssert.h"
#include "Utils/Utils.h"
#include "Utils/StringFormat.h"
#include "FileSystem/ResourceArchive.h"


#if defined(__DAVAENGINE_MACOS__) || defined(__DAVAENGINE_IPHONE__)
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <copyfile.h>
#elif defined(__DAVAENGINE_WIN32__)
#include <direct.h>
#include <io.h> 
#include <sys/types.h>
#include <sys/stat.h>
#endif

namespace DAVA
{

	
	static String virtualBundlePath = "";
	
	void ReplaceBundleName(const String &newBundlePath)
	{
		virtualBundlePath = newBundlePath;	
	}
	
	
#if defined(__DAVAENGINE_IPHONE__)
	NSString * FilepathRelativeToBundleObjC(NSString * relativePathname)
	{
		NSString * filePath;
		if(virtualBundlePath.empty())
		{
				//		NSString * bundlePath = [[[NSBundle mainBundle] bundlePath] stringByAppendingString: @""];
			NSString * bundlePath = [[[NSBundle mainBundle] bundlePath] stringByAppendingString: @"/Data"];
			filePath = [bundlePath stringByAppendingString: relativePathname];
		}
		else 
		{
			NSString * bundlePath = [NSString stringWithUTF8String: virtualBundlePath.c_str()];
			filePath = [bundlePath stringByAppendingString: relativePathname];
		}
		
		return filePath;
	}
#elif defined(__DAVAENGINE_MACOS__)
	NSString * FilepathRelativeToBundleObjC(NSString * relativePathname)
	{
        NSString * filePath;
        if(virtualBundlePath.empty())
        {
            NSString * bundlePath = [[[NSBundle mainBundle] bundlePath] stringByAppendingString: @"/Contents/Resources/Data"];
            filePath = [bundlePath stringByAppendingString: relativePathname];
        }
        else
        {
            NSString * bundlePath = [NSString stringWithUTF8String: virtualBundlePath.c_str()];
            filePath = [bundlePath stringByAppendingString: relativePathname];
        }
		
		return filePath;
	}
#endif	
	
	
#if defined(__DAVAENGINE_IPHONE__) || defined(__DAVAENGINE_MACOS__)
	
	const char * FilepathRelativeToBundle(const char * relativePathname)
	{
		NSString * filePath = FilepathRelativeToBundleObjC([NSString stringWithUTF8String: relativePathname]);
		return [filePath UTF8String];
	}
	
	const char * FilepathRelativeToBundle(const String & relativePathname)
	{
		return FilepathRelativeToBundle(relativePathname.c_str());
	}
	
	NSString * FilepathInDocumentsObjC(NSString * relativePathname)
	{
		NSArray * paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
		NSString * bundlePath = [paths objectAtIndex:0];
			//	NSString * bundlePath2 = [bundlePath stringByAppendingString:@"/"]; removed for the bundle path compatibility
		NSString * filePath = [bundlePath stringByAppendingString: relativePathname];
		return filePath;
	}
	
	const char * FilepathInDocuments(const char * relativePathname)
	{
		NSString * filePath = FilepathInDocumentsObjC([NSString stringWithUTF8String: relativePathname]);
		return [filePath UTF8String];
	}
	
	const char * FilepathInDocuments(const String & relativePathname)
	{
		return FilepathInDocuments(relativePathname.c_str());
	}
	
#endif
	
#if defined(__DAVAENGINE_WIN32__)
	const char * FilepathRelativeToBundle(const char * relativePathname)
	{
		return Format("./Data/%s", relativePathname);
	}
	
	const char * FilepathRelativeToBundle(const String & relativePathname)
	{
		return Format("./Data/%s", relativePathname.c_str());
	}
	
	const char * FilepathInDocuments(const char * relativePathname)
	{
		return Format("./Documents/%s", relativePathname);
	}
	
	const char * FilepathInDocuments(const String & relativePathname)
	{
		return Format("./Documents/%s", relativePathname.c_str());
	}
	
#endif
	
	
	
FileSystem::FileSystem()
{
}

FileSystem::~FileSystem()
{	
	for (List<ResourceArchiveItem>::iterator ai = resourceArchiveList.begin();
		ai != resourceArchiveList.end(); ++ai)
	{
		ResourceArchiveItem & item = *ai;
		SafeRelease(item.archive);
	}
	resourceArchiveList.clear();
}

FileSystem::eCreateDirectoryResult FileSystem::CreateDirectory(const String & filePath, bool isRecursive)
{
	if (!isRecursive)
	{
#ifdef __DAVAENGINE_WIN32__
		BOOL res = ::CreateDirectoryA(filePath.c_str(), 0);
		return (res == 0) ? DIRECTORY_CANT_CREATE : DIRECTORY_CREATED;
#elif defined(__DAVAENGINE_MACOS__) || defined(__DAVAENGINE_IPHONE__) 
		int res = mkdir(filePath.c_str(), 0777);
		if (res == EEXIST)
			return DIRECTORY_EXISTS;
		return (res == 0) ? (DIRECTORY_CREATED) : (DIRECTORY_CANT_CREATE);
#endif
	}

	String path = filePath;
	std::replace(path.begin(), path.end(),'\\','/');
	const String & delims="/";
	String::size_type lastPos = path.find_first_not_of(delims, 0);
	String::size_type pos     = path.find_first_of(delims, lastPos);
	Vector<String> tokens;
	while (String::npos != pos || String::npos != lastPos)
	{
		tokens.push_back(path.substr(lastPos, pos - lastPos));
		lastPos = path.find_first_not_of(delims, pos);
		pos     = path.find_first_of(delims, lastPos);
	}
	
#if defined(__DAVAENGINE_WIN32__)
	String dir = "";
#elif defined(__DAVAENGINE_MACOS__) || defined(__DAVAENGINE_IPHONE__) 
	String dir = "/";
#endif
	
	for (size_t k = 0; k < tokens.size(); ++k)
	{
		dir += tokens[k] + "/";
#if defined(__DAVAENGINE_WIN32__)
		BOOL res = ::CreateDirectoryA(dir.c_str(), 0);
		if (k == tokens.size() - 1)
		{
			return (res == 0) ? DIRECTORY_CANT_CREATE : DIRECTORY_CREATED;
		}
#elif defined(__DAVAENGINE_MACOS__) || defined(__DAVAENGINE_IPHONE__) 
		int res = mkdir(dir.c_str(), 0777);
		if (k == tokens.size() - 1)
		{
			if (res == EEXIST)
				return DIRECTORY_EXISTS;
			return (res == 0) ? (DIRECTORY_CREATED) : (DIRECTORY_CANT_CREATE);
		}
#endif
	}
	return DIRECTORY_CANT_CREATE;
}

bool FileSystem::CopyFile(const String & existingFile, const String & newFile)
{
#ifdef __DAVAENGINE_WIN32__
	BOOL ret = ::CopyFileA(existingFile.c_str(), newFile.c_str(), true);
	return ret != 0;
#else
    int ret = copyfile(existingFile.c_str(), newFile.c_str(), NULL, COPYFILE_ALL | COPYFILE_EXCL);
    return ret==0;
	//DVASSERT(0 && "FileSystem::CopyFile not implemented for current platform");
#endif
}
	
bool FileSystem::DeleteFile(const String & filePath)
{
	// function unlink return 0 on success, -1 on error
	int res = remove(SystemPathForFrameworkPath(filePath).c_str());
	return (res == 0);
}
	
bool FileSystem::DeleteDirectory(const String & path, bool isRecursive)
{
	FileList * fileList = new FileList(path);
	for(int i = 0; i < fileList->GetCount(); ++i)
	{
		if(fileList->IsDirectory(i))
		{
			if(!fileList->IsNavigationDirectory(i))
			{
				if(isRecursive)
				{
					Logger::Debug("- try to delete directory: %s / %s", fileList->GetPathname(i).c_str(), fileList->GetFilename(i).c_str());
					bool success = DeleteDirectory(fileList->GetPathname(i), isRecursive);
					Logger::Debug("- delete directory: %s / %s- %d", fileList->GetPathname(i).c_str(), fileList->GetFilename(i).c_str(), success ? (1): (0));
					if (!success)return false;
				}
			}
		}
		else 
		{
			bool success = DeleteFile(fileList->GetPathname(i));
			Logger::Debug("- delete file: %s / %s- %d", fileList->GetPathname(i).c_str(), fileList->GetFilename(i).c_str(), success ? (1): (0));
			if(!success)return false;
		}
	}
	SafeRelease(fileList);
#ifdef __DAVAENGINE_WIN32__
	String sysPath = SystemPathForFrameworkPath(path);
	int32 chmodres = _chmod(sysPath.c_str(), _S_IWRITE); // change read-only file mode
	int32 res = _rmdir(sysPath.c_str());
	return (res == 0);
	/*int32 res = ::RemoveDirectoryA(path.c_str());
	if (res == 0)
	{
		Logger::Warning("Failed to delete directory: %s error: 0x%x", path.c_str(), GetLastError());
	}
	return (res != 0);*/
#elif defined(__DAVAENGINE_MACOS__) || defined(__DAVAENGINE_IPHONE__) 
	int32 res = rmdir(SystemPathForFrameworkPath(path).c_str());
	return (res == 0);
#endif
}
	
uint32 FileSystem::DeleteDirectoryFiles(const String & path, bool isRecursive)
{
	uint32 fileCount = 0; 
	
	FileList * fileList = new FileList(path);
	for(int i = 0; i < fileList->GetCount(); ++i)
	{
		if(fileList->IsDirectory(i))
		{
			if(!fileList->IsNavigationDirectory(i))
			{
				if(isRecursive)
				{
					fileCount += DeleteDirectoryFiles(fileList->GetPathname(i), isRecursive);
				}
			}
		}
		else 
		{
			bool success = DeleteFile(fileList->GetPathname(i));
			if(success)fileCount++;
		}
	}
	SafeRelease(fileList);

	return fileCount;
}


	
File *FileSystem::CreateFileForFrameworkPath(const String & frameworkPath, uint32 attributes)
{
	return File::CreateFromSystemPath(SystemPathForFrameworkPath(frameworkPath), attributes);
}

const String & FileSystem::SystemPathForFrameworkPath(const String & frameworkPath)
{
	DVASSERT(frameworkPath.size() > 0);
	if(frameworkPath[0] != '~')
	{
		return frameworkPath;
	}
	tempRetPath = frameworkPath;
	size_t find = tempRetPath.find("~res:");

	if(find != tempRetPath.npos)
	{
		tempRetPath = tempRetPath.erase(0, 5);
		tempRetPath = FilepathRelativeToBundle("") + tempRetPath;
	}
	else
	{
		find = tempRetPath.find("~doc:");
		if(find != tempRetPath.npos)
		{
			tempRetPath = tempRetPath.erase(0, 5);
			tempRetPath = FilepathInDocuments("") + tempRetPath;
		}
	}
	return tempRetPath;
}
	
const String & FileSystem::GetCurrentWorkingDirectory()
{
	char tempDir[2048];
#if defined(__DAVAENGINE_WIN32__)
	::GetCurrentDirectoryA(2048, tempDir);
	currentWorkingDirectory = tempDir;
	return currentWorkingDirectory;
#elif defined(__DAVAENGINE_MACOS__) || defined(__DAVAENGINE_IPHONE__) 
	getcwd(tempDir, 2048);
	currentWorkingDirectory = tempDir;
	return currentWorkingDirectory;
#endif
	return currentWorkingDirectory; 
}

bool FileSystem::SetCurrentWorkingDirectory(const String & newWorkingDirectory)
{
#if defined(__DAVAENGINE_WIN32__)
	BOOL res = ::SetCurrentDirectoryA(newWorkingDirectory.c_str());
	return (res != 0);
#elif defined(__DAVAENGINE_MACOS__) || defined(__DAVAENGINE_IPHONE__) 
	return !(chdir(newWorkingDirectory.c_str()) == 0);
#endif
	return false; 
}
	
String FileSystem::RealPath(const String & _path)
{
	
	String path = (_path);
	std::replace(path.begin(), path.end(),'\\','/');
	
	const String & delims="/";
	
	// Skip delims at beginning, find start of first token
	String::size_type lastPos = path.find_first_not_of(delims, 0);
	// Find next delimiter @ end of token
	String::size_type pos     = path.find_first_of(delims, lastPos);
	
	// output vector
	Vector<String> tokens;
	
	
	while (String::npos != pos || String::npos != lastPos)
	{
		// Found a token, add it to the vector.
		tokens.push_back(path.substr(lastPos, pos - lastPos));
		// Skip delims.  Note the "not_of". this is beginning of token
		lastPos = path.find_first_not_of(delims, pos);
		// Find next delimiter at end of token.
		pos     = path.find_first_of(delims, lastPos);
	}
	
	if (tokens.size() > 0)
	{
		if (tokens[0] == ".")
		{
			tokens[0] = FileSystem::Instance()->GetCurrentWorkingDirectory();
			std::replace(tokens[0].begin(), tokens[0].end(),'\\','/');
		}
	}

	String result;
	
	for (int i = 0; i < (int)tokens.size(); ++i)
	{
		if (tokens[i] == String("."))
		{		
			for (int k = i + 1; k < (int)tokens.size(); ++k)
			{
				tokens[k - 1] = tokens[k];
			}
			i--;
			tokens.pop_back();
			continue;
		}
		if (tokens[i] == String(".."))
		{		
			for (int k = i + 1; k < (int)tokens.size(); ++k)
			{
				tokens[k - 2] = tokens[k];
			}
			i-=2;
			tokens.pop_back();
			tokens.pop_back();
			continue;
		}	
	}
#if !defined(__DAVAENGINE_WIN32__)
	result = "/";
#endif
	for (int k = 0; k < (int)tokens.size(); ++k)
	{
		result += tokens[k];
		if (k + 1 != (int)tokens.size())
			result += String("/");
	}
	return result;
}

String FileSystem::ReplaceExtension(const String & filename, const String & newExt)
{
	String::size_type dotpos = filename.rfind(".");
	if (dotpos == String::npos)
		return String();
	
	String result = filename.substr(0, dotpos) + newExt;
	return result;
}	

String	FileSystem::GetExtension(const String & filename)
{
	String::size_type dotpos = filename.rfind(".");
	if (dotpos == String::npos)
		return String();
	return filename.substr(dotpos);
}
	
void  FileSystem::SplitPath(const String & filePath, String & path, String & filename)
{
	String fullPath(filePath);
	std::replace(fullPath.begin(),fullPath.end(),'\\','/');
	// now only Unix style slashes
	String::size_type lastSlashPos = fullPath.find_last_of('/');
	
	if (lastSlashPos==String::npos)
	{
		path = "";
		filename = fullPath;
	}
	else
	{
		path = fullPath.substr(0, lastSlashPos) + '/';
		filename = fullPath.substr(lastSlashPos + 1, fullPath.size() - lastSlashPos - 1);
	}
}

uint8 * FileSystem::ReadFileContents(const String & pathname, uint32 & fileSize)
{
    File * fp = File::Create(pathname, File::OPEN|File::READ);
	if (!fp)
	{
		Logger::Error("Failed to open file: %s", pathname.c_str());
		return 0;
	}
	fileSize = fp->GetSize();
	uint8 * bytes = new uint8[fileSize];
	uint32 dataRead = fp->Read(bytes, fileSize);
    
	if (dataRead != fileSize)
	{
		Logger::Error("Failed to read data from file: %s", pathname.c_str());
		return 0;
	}

	SafeRelease(fp);
    return bytes;
};

void FileSystem::AttachArchive(const String & archiveName, const String & attachPath)
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
}

}




