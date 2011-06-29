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

#include "Utils/Utils.h"
#include "Utils/StringFormat.h"

namespace DAVA
{

	

void Split(const String & inputString, const String & delims, Vector<String> & tokens)
{
	// Skip delims at beginning, find start of first token
	String::size_type lastPos = inputString.find_first_not_of(delims, 0);
	// Find next delimiter @ end of token
	String::size_type pos     = inputString.find_first_of(delims, lastPos);
	// output vector
	// Vector<String> tokens;
	
	while (String::npos != pos || String::npos != lastPos)
	{
		// Found a token, add it to the vector.
		tokens.push_back(inputString.substr(lastPos, pos - lastPos));
		// Skip delims.  Note the "not_of". this is beginning of token
		lastPos = inputString.find_first_not_of(delims, pos);
		// Find next delimiter at end of token.
		pos     = inputString.find_first_of(delims, lastPos);
	}	
}

	
}; // end of namespace DAVA
