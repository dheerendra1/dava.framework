#include "Utils/HTTPDownloader.h"

#if defined(__DAVAENGINE_ANDROID__) 

#include "Platform/TemplateAndroid/CorePlatformAndroid.h"

namespace DAVA 
{

	HTTPDownloader::HTTPDownloader()
	{
		downloader = NULL;
	}

	HTTPDownloader::~HTTPDownloader()
	{

	}

	void HTTPDownloader::DownloadFile(const String &address, HTTPDownloaderDelegate *delegate)
	{
	}

	bool DownloadFileFromURLToDocuments(const String & url, const String & documentsPathname)
	{
		CoreAndroidPlatform *core = (CoreAndroidPlatform *)Core::Instance();
		if(core)
		{
			return core->DownloadHttpFile(url, documentsPathname);
		}

		return false;
	}

};

#endif //#if defined(__DAVAENGINE_ANDROID__)
