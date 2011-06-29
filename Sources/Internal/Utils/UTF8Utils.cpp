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
#include "Utils/UTF8Utils.h"
#include "FileSystem/Logger.h"

namespace DAVA 
{
	
	/*
	 * Decode a UTF-8 character.  Check RFC 3629
	 * (http://www.ietf.org/rfc/rfc3629.txt) for more details.
	 *
	 * The following table (taken from the RFC) is used for
	 * decoding.
	 *
	 *    Char. number range |        UTF-8 octet sequence
	 *      (hexadecimal)    |              (binary)
	 *   --------------------+------------------------------------
	 *   0000 0000-0000 007F | 0xxxxxxx
	 *   0000 0080-0000 07FF | 110xxxxx 10xxxxxx
	 *   0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
	 *   0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
	 *
	 * Additionally, the characters in the range 0xD800-0xDFFF
	 * are prohibited as they are reserved for use with UTF-16
	 * surrogate pairs.
	 */
	

int32 UTF8Utils::GetStringLength(uint8 * string, int32 size)
{
	int32 length = 0;
	int32 bytesleft = size;
	while(bytesleft > 0)
	{
		unsigned int value = 0;
		
		char8 octet = string[0];
		int32 width =   (octet & 0x80) == 0x00 ? 1 :
						(octet & 0xE0) == 0xC0 ? 2 :
						(octet & 0xF0) == 0xE0 ? 3 :
						(octet & 0xF8) == 0xF0 ? 4 : 0;
		
		/* Check if the leading octet is valid. */
		
		if (!width)
		{
			Logger::Error("UTF8Utils:: invalid leading UTF-8 octet");
			return 0;
		}
		
		/* Check if the raw buffer contains an incomplete character. */
		
		if (width > size) 
		{
			Logger::Error("incomplete UTF-8 octet sequence");
			break;
		}
		
		/* Decode the leading octet. */
		
		value = (octet & 0x80) == 0x00 ? octet & 0x7F :
		(octet & 0xE0) == 0xC0 ? octet & 0x1F :
		(octet & 0xF0) == 0xE0 ? octet & 0x0F :
		(octet & 0xF8) == 0xF0 ? octet & 0x07 : 0;
		
		/* Check and decode the trailing octets. */
		
		for (int32 k = 1; k < width; k ++)
		{
			octet = string[k];
			
			/* Check if the octet is valid. */
			
			if ((octet & 0xC0) != 0x80)
			{
				Logger::Error("invalid trailing UTF-8 octet");
			}
			/* Decode the octet. */
			
			value = (value << 6) + (octet & 0x3F);
		}
		
		/* Check the length of the sequence against the value. */
		
		if (!((width == 1) ||
			  (width == 2 && value >= 0x80) ||
			  (width == 3 && value >= 0x800) ||
			  (width == 4 && value >= 0x10000)))
		{
			Logger::Error("invalid length of a UTF-8 sequence");
		}
		/* Check the range of the value. */
		
		if ((value >= 0xD800 && value <= 0xDFFF) || value > 0x10FFFF)
		{
			Logger::Error("invalid Unicode character");
		}
		if (value == 0)break;
		string += width;
		bytesleft -= width;
		length ++;
	}
	return length;
}
	
void  UTF8Utils::EncodeToWideString(uint8 * string, int32 size, WideString & resultString)
{
	int32 realLenght = GetStringLength(string, size);
	/* Determine the length of the UTF-8 sequence. */
	int32 bytesleft = size;
	resultString.resize(realLenght);
	
	int strPos = 0;
	while(bytesleft > 0)
	{
		unsigned int value = 0;
		
		char8 octet = string[0];
		int32 width =   (octet & 0x80) == 0x00 ? 1 :
						(octet & 0xE0) == 0xC0 ? 2 :
						(octet & 0xF0) == 0xE0 ? 3 :
						(octet & 0xF8) == 0xF0 ? 4 : 0;
		
		/* Check if the leading octet is valid. */
		
		if (!width)
		{
			Logger::Error("UTF8Utils:: invalid leading UTF-8 octet");
			return;
		}
		
		/* Check if the raw buffer contains an incomplete character. */
		
		if (width > size) 
		{
			Logger::Error("incomplete UTF-8 octet sequence");
			break;
		}
		
		/* Decode the leading octet. */
		
		value = (octet & 0x80) == 0x00 ? octet & 0x7F :
				(octet & 0xE0) == 0xC0 ? octet & 0x1F :
				(octet & 0xF0) == 0xE0 ? octet & 0x0F :
				(octet & 0xF8) == 0xF0 ? octet & 0x07 : 0;
		
		/* Check and decode the trailing octets. */
		
		for (int32 k = 1; k < width; k ++)
		{
			octet = string[k];
			
			/* Check if the octet is valid. */
			
			if ((octet & 0xC0) != 0x80)
			{
				Logger::Error("invalid trailing UTF-8 octet");
			}
			/* Decode the octet. */
			
			value = (value << 6) + (octet & 0x3F);
		}
		
		/* Check the length of the sequence against the value. */
		
		if (!((width == 1) ||
			  (width == 2 && value >= 0x80) ||
			  (width == 3 && value >= 0x800) ||
			  (width == 4 && value >= 0x10000)))
		{
			Logger::Error("invalid length of a UTF-8 sequence");
		}
		/* Check the range of the value. */
		
		if ((value >= 0xD800 && value <= 0xDFFF) || value > 0x10FFFF)
		{
			Logger::Error("invalid Unicode character");
		}
		if (value == 0)break;
		string += width;
		bytesleft -= width;
		resultString[strPos++] = value;
	}

};

};