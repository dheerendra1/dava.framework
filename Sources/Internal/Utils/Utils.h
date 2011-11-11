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
#ifndef __DAVAENGINE_UTILS_H__
#define __DAVAENGINE_UTILS_H__

/**
	\defgroup utils Utilities
 */

#include "Base/BaseTypes.h"

namespace DAVA 
{
//const char * FilepathRelativeToBundle(const char * relativePathname);
//const char * FilepathRelativeToBundle(const String & relativePathname);

//const char * FilepathInDocuments(const char * relativePathname);
//const char * FilepathInDocuments(const String & relativePathname);

bool IsDrawThread();

inline WideString StringToWString(const String& s);
inline String WStringToString(const WideString& s);

WideString GetDeviceName();
	
#if defined(__DAVAENGINE_IPHONE__)
void DisableSleepTimer();
void EnableSleepTimer();
#endif
	
//int SplitString(const String& input, const String& delimiter, std::vector<String>& results, bool includeEmpties = true);
void Split(const String & inputString, const String & delims, Vector<String> & tokens);

void ReplaceBundleName(const String &newBundlePath);
	
//implementation

inline WideString StringToWString(const String& s)
{
	WideString temp(s.length(),L' ');
	std::copy(s.begin(), s.end(), temp.begin());
	return temp; 
}

inline void StringReplace(String & repString,const String & needle, const String & s)
{
	String::size_type lastpos = 0, thispos;
	while ((thispos = repString.find(needle, lastpos)) != String::npos)
	{
		repString.replace(thispos, needle.length(), s);
		lastpos = thispos + 1;
	}
}

inline String WStringToString(const WideString& s)
{
	size_t len = s.length();
	String temp(len, ' ');
	//std::copy(s.begin(), s.end(), temp.begin());
	for (size_t i = 0; i < len; ++i)
		temp[i] = (char)s[i];
	return temp; 
}

};

#endif // __DAVAENGINE_UTILS_H__

