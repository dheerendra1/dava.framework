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
#include "FileSystem/Logger.h"
#include <stdarg.h>


#if defined(__DAVAENGINE_WIN32__)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#endif 

namespace DAVA 
{

#if defined(__DAVAENGINE_WIN32__)
void Logger::Logv(eLogLevel ll, const char8* text, va_list li)
{
	if (ll < logLevel)return; 
	//NSString * string = [NSString stringWithCString: GetLogLevelString(ll)];
	//NSString * stringOut = [string stringByAppendingString:[NSString stringWithCString:text]];
	//NSString * string = [NSString stringWithFormat:@"[%s] %@", GetLogLevelString(ll), [NSString stringWithCString:text]];
	//NSLogv(string, li);
	char tmp[4096];
	vsprintf(tmp, text, li);
	strcat(tmp, "\n");
	OutputDebugStringA(tmp);
}

void Logger::Logv(eLogLevel ll, const char16* text, va_list li)
{
	if (ll < logLevel)return; 
	//NSString * ss = [NSString stringWithCString:(const char *)text encoding: NSUTF32BigEndianStringEncoding];
	//NSString * str = [NSString stringWithFormat:@"[%s] %@", GetLogLevelString(ll), [NSString stringWithCString:(const char8*)text encoding: NSUTF32LittleEndianStringEncoding]];
	//NSLogv(string, li);
	//vwprintf((wchar_t*)text, li); printf("\n");
	wchar_t tmp[4096];
	wvsprintf(tmp, text, li);
	wcscat(tmp, L"\n");
	OutputDebugStringW(tmp);
}

#endif 

static const char8 * logLevelString[4] =
{	
	"debug",
	"info",
	"warning",
	"error" 
};
	
Logger::Logger()
{
	logLevel = LEVEL_DEBUG;
}

Logger::~Logger()
{
	
}
	
Logger::eLogLevel Logger::GetLogLevel()
{
	return logLevel;
}
	
const char8 * Logger::GetLogLevelString(eLogLevel ll)
{
	return logLevelString[ll];
}
	
void Logger::SetLogLevel(eLogLevel ll)
{
	logLevel = ll;
}
	
void Logger::Log(eLogLevel ll, const char8* text, ...)
{
	if (ll < logLevel)return; 
	
	va_list vl;
	va_start(vl, text);
	Logv(ll, text, vl);
	va_end(vl);
}	

void Logger::Log(eLogLevel ll, const char16* text, ...)
{
	if (ll < logLevel)return; 
	
	va_list vl;
	va_start(vl, text);
	Logv(ll, text, vl);
	va_end(vl);
}
	
	
void Logger::Debug(const char8 * text, ...)
{
	va_list vl;
	va_start(vl, text);
	Logger::Instance()->Logv(LEVEL_DEBUG, text, vl);
	va_end(vl);
}
	
void Logger::Info(const char8 * text, ...)
{
	va_list vl;
	va_start(vl, text);
	Logger::Instance()->Logv(LEVEL_INFO, text, vl);
	va_end(vl);
}	
	
void Logger::Warning(const char8 * text, ...)
{
	va_list vl;
	va_start(vl, text);
	Logger::Instance()->Logv(LEVEL_WARNING, text, vl);
	va_end(vl);
}
	
void Logger::Error(const char8 * text, ...)
{
	va_list vl;
	va_start(vl, text);
	Logger::Instance()->Logv(LEVEL_ERROR, text, vl);
	va_end(vl);
}

void Logger::Debug(const char16 * text, ...)
{
	va_list vl;
	va_start(vl, text);
	Logger::Instance()->Logv(LEVEL_DEBUG, text, vl);
	va_end(vl);
}

void Logger::Info(const char16 * text, ...)
{
	va_list vl;
	va_start(vl, text);
	Logger::Instance()->Logv(LEVEL_INFO, text, vl);
	va_end(vl);
}	

void Logger::Warning(const char16 * text, ...)
{
	va_list vl;
	va_start(vl, text);
	Logger::Instance()->Logv(LEVEL_WARNING, text, vl);
	va_end(vl);
}

void Logger::Error(const char16 * text, ...)
{
	va_list vl;
	va_start(vl, text);
	Logger::Instance()->Logv(LEVEL_ERROR, text, vl);
	va_end(vl);
}


}



