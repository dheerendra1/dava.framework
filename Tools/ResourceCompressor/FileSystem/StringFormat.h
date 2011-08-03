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
#ifndef __LOGENGINE_STRINGFORMAT_H__
#define __LOGENGINE_STRINGFORMAT_H__

#include "BaseTypes.h"
#include <stdarg.h>

namespace Log
{
	//! String formating functions
	//! Functions for use together with Global::Log

	//! Formatting function (use printf syntax)
	//! Function support recursive calls as : 
	//! Format("%s", Format("%d: %d: %d", 10, 20, 33));

	const char8 * Format(const char8 * text, ... );
	const char8 * FormatVL(const char8 * text, va_list vl);

	//! Formatting function (use printf syntax)
	const char16 * Format(const char16 * text, ... );
	const char16 * FormatVL(const char16 * text, va_list vl);
	
	
	//! Function to get indent strings for usage in printf and similar functions
	const char8 * GetIndentString(char8 indentChar, int32 level);

}; // end of namespace

#endif // __LOGENGINE_STRINGFORMAT_H__