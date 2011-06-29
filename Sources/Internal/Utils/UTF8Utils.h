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
#ifndef __DAVAENGINE_UTF8UTILS_H__
#define __DAVAENGINE_UTF8UTILS_H__

#include "Base/BaseTypes.h"


namespace DAVA 
{
/**
	 \ingroup utils
	 \brief Class to work with UTF8 strings
 */
class UTF8Utils
{
public:

	/**
		\brief get length of UTF8 string
		\param[in] string string in UTF8 format
		\param[in] size size of buffer allocated for this string
		\returns length of this UTF8 string in characters
	 */
	static int32 GetStringLength(uint8 * string, int32 size);
	
	/**
		\brief convert UTF8 string to WideString
		\param[in] string string in UTF8 format
		\param[in] size size of buffer allocated for this string
		\param[out] resultString result unicode string
	 */
	static void  EncodeToWideString(uint8 * string, int32 size, WideString & resultString);
	
};



};


#endif // __DAVAENGINE_UTF8UTILS_H__