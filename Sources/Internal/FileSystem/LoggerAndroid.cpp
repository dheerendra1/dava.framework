#include "FileSystem/Logger.h"
#include <stdarg.h>

#include <android/log.h>

#include "Utils/StringFormat.h"

namespace DAVA 
{

#if defined(__DAVAENGINE_ANDROID__)

    static DAVA::String androidLogTag = "";    
    

int32 LogLevelToAndtoid(Logger::eLogLevel ll)
{
	int32 androidLL = ANDROID_LOG_DEFAULT;
	switch (ll)
	{
		case Logger::LEVEL_DEBUG:
			androidLL = ANDROID_LOG_DEBUG;
			break;

		case Logger::LEVEL_INFO:
			androidLL = ANDROID_LOG_INFO;
			break;

		case Logger::LEVEL_WARNING:
			androidLL = ANDROID_LOG_WARN;
			break;

		case Logger::LEVEL_ERROR:
			androidLL = ANDROID_LOG_ERROR;
			break;
	}

	return androidLL;
}

void Logger::Logv(eLogLevel ll, const char8* text, va_list li)
{
	if (ll < logLevel)return; 

	char tmp[4096];
	vsprintf(tmp, text, li);
	strcat(tmp, "\n");
	__android_log_print(LogLevelToAndtoid(ll), androidLogTag.c_str(), tmp);
}

void Logger::Logv(eLogLevel ll, const char16* text, va_list li)
{
	//TODO: VK: add code

	if (ll < logLevel)return; 

 	wchar_t tmp[4096];
	vswprintf(tmp, 4096, text, li);
 	wcscat(tmp, L"\n");

// 	char tmpChar[4096];
// 	wcstombs(tmpChar, tmp, 4096);
// 	__android_log_print(LogLevelToAndtoid(ll), LOG_TAG, tmpChar);
}

void Logger::SetTag(const char8 *logTag)
{
    androidLogTag = Format("%s", logTag);
}
    
#endif //#if defined(__DAVAENGINE_ANDROID__)

}
