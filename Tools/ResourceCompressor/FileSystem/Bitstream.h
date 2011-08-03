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
#ifndef __LOGENGINE_BITSTREAM_H__
#define __LOGENGINE_BITSTREAM_H__

#include "BaseTypes.h"

namespace Log
{
namespace IO
{


//! \brief class to work with bitstreams
//!
class Bitstream
{
public:

	Bitstream(void * bitstreamPointer,uint32 bitstreamLength);
	virtual ~Bitstream();

	//! \brief function reset bitstream to start position
	void Reset(); 
	
	//! \brief function return size of bitstream
	uint32 Length();
	
	//! \brief function return current bitstream position
	uint32 GetPosition();

	//! \brief function to skip bits, by default skip 1 bit
	//! \param bitCount bit count to skip
	void Skip(uint32 bitCount = 1);
	
	//! \brief function to skip bits in writing mode by default skip 1 bit
	//! \param bitCount bit count to skip
	void Forward(uint32 bitCount = 1);

	
	//! \brief function to read bits from stream
	//! \param bitCount bit count to read
	uint32 ReadBits(uint32 bitCount = 1);

	//! \brief function to show next [bitCount] bits from stream
	//! \param bitCount bit count to show
	uint32 ShowBits(uint32 bitCount = 1);


	//! \brief function to write bitCount bits to stream
	//! \param bitCount bit count to write
	void WriteBits(uint32 bitValue, uint32 bitCount);
	

	//! \brief function to write 1 bit to stream
	void WriteBit(uint32 bitValue);

	//! \brief Align current stream to byte boundary
	void AlignToByte();


private:

	// Bitstream pointer
	uint32	*head;
	uint32	*tail;
	uint32	position;
	uint32	length;
	uint32	bufferA;		// Read Buffer [0]
	uint32	bufferB;		// Read Buffer [1]
	uint32	writeBuffer;	// Write Buffer

	// Reset params
	uint32	initialPosition;
};

};
};

#endif // __LOGENGINE_BITSTREAM_H__