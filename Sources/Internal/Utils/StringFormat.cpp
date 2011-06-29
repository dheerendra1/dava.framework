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
#include <stdarg.h>
#include <stdio.h>
#include "Utils/StringFormat.h"

namespace DAVA
{


static const int32 FORMAT_STRING_SIZE = 2048;
static const int32 FORMAT_STRING_MAX_LEN = 256;

static char8 formatString8[FORMAT_STRING_SIZE];
static char16 formatString16[FORMAT_STRING_SIZE];
static int32 formatString8Position = 0;
static int32 formatString16Position = 0;

//! formatting function (use printf syntax)
const char8 * Format(const char8 * text, ...)
{
	if (formatString8Position + FORMAT_STRING_MAX_LEN >= FORMAT_STRING_SIZE)
	{
		formatString8Position = 0;
	}
	va_list ll;

	va_start(ll, text);
	int32 len = vsprintf(&formatString8[formatString8Position],  text, ll);
	formatString8Position += (len + 1);

	va_end(ll);

	return &formatString8[formatString8Position  - (len + 1)];
}
	
const char8 * GetIndentString(char8 indentChar, int32 level)
{
	if (formatString8Position + FORMAT_STRING_MAX_LEN >= FORMAT_STRING_SIZE)
	{
		formatString8Position = 0;
	}
	if (level > FORMAT_STRING_MAX_LEN)level = FORMAT_STRING_MAX_LEN - 1;
	
	for (int k = 0; k < level; k++)
	{
		formatString8[formatString8Position + k] = indentChar;
	}
	formatString8[level] = 0;
	formatString8Position += (level + 1);
	return &formatString8[formatString8Position  - (level + 1)];
}
	
	
	


//! formatting function (use printf syntax)
const char16 * Format(const char16 * text, ...)
{
	if (formatString16Position + FORMAT_STRING_MAX_LEN >= FORMAT_STRING_SIZE)
	{
		formatString16Position = 0;
	}
	va_list ll;

	va_start(ll, text);

#if defined(_WIN32)
	int32 len = vswprintf((wchar_t *)&formatString16[formatString16Position], (wchar_t *)text, ll);
#else // MAC_OS & other nix systems
	int32 len = vswprintf((wchar_t *)&formatString16[formatString16Position], FORMAT_STRING_MAX_LEN, (wchar_t *)text, ll);
#endif
	
	formatString16Position += (len + 1);
	va_end(ll);
	return &formatString16[formatString16Position - (len + 1)];
}

const char8 * FormatVL(const char8 * text, va_list ll)
{
	if (formatString8Position + FORMAT_STRING_MAX_LEN >= FORMAT_STRING_SIZE)
	{
		formatString8Position = 0;
	}
	int32 len = vsprintf(&formatString8[formatString8Position],  text, ll);
	formatString8Position += (len + 1);

	return &formatString8[formatString8Position  - (len + 1)];
}

const char16 * FormatVL(const char16 * text, va_list ll)
{
	if (formatString16Position + FORMAT_STRING_MAX_LEN >= FORMAT_STRING_SIZE)
	{
		formatString16Position = 0;
	}
#if defined(_WIN32)
	int32 len = vswprintf((wchar_t *)&formatString16[formatString16Position], (wchar_t *)text, ll);
#else // MAC_OS & other nix systems
	int32 len = vswprintf((wchar_t *)&formatString16[formatString16Position], FORMAT_STRING_MAX_LEN, (wchar_t *)text, ll);
#endif
	
	formatString16Position += (len + 1);
	return &formatString16[formatString16Position - (len + 1)];
}
	
	

}; // end of namespace Log
