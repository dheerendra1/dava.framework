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
#ifndef __DAVAENGINE_MEMORY_FILE_H__
#define __DAVAENGINE_MEMORY_FILE_H__

#include "Base/BaseTypes.h"
#include "Base/BaseObject.h"
#include "FileSystem/File.h"


namespace DAVA 
{
	class DynamicMemoryFile : public File 
	{
		DynamicMemoryFile();
		virtual ~DynamicMemoryFile();
		
	public:
		
		/** 
		 \brief funciton to create a file instance with give attributes
		 \param[in] data pointer to data to create file from
		 \param[in] dataSize size of data to create file from 
		 \param[in] attributes combinations of eFileAttributes
		 \returns file instance
		 */
		static DynamicMemoryFile * Create(const int8 * data, int32 dataSize, uint32 attributes);

		/** 
		 \brief funciton to create a file instance with give attributes
		 \param[in] attributes combinations of eFileAttributes
		 \returns file instance
		 */
		static DynamicMemoryFile * Create(uint32 attributes);
		

		/** 
		 \brief returns pointer to the data contained in the memory file
		 \returns pointer to the first byte of the file data if file is empty returns NULL
		 */
		void * GetData();
		
		
		/**
		 \brief Get this file name
		 \returns filename of this file 
		 */
		virtual	const char8 * GetFilename();
		
		/**
		 \brief Get this file full pathname
		 \returns filename of this file 
		 */
		virtual const char8 * GetPathname();
		
		/** 
		 \brief Write [dataSize] bytes to this file from [pointerToData]
		 \param[in] pointerToData function get data from this pointer
		 \param[in] dataSize size of data we want to write
		 \returns number of bytes actually written
		 */
		virtual uint32 Write(const void * pointerToData, uint32 dataSize);
		
		/** 
		 \brief Read [dataSize] bytes from this file to [pointerToData] 
		 \param[in, out] pointerToData function write data to this pointer
		 \param[in] dataSize size of data we want to read
		 \return number of bytes actually read
		 */
		virtual uint32 Read(void * pointerToData, uint32 dataSize);
		
		/** 
		 \brief Get current file position
		 */
		virtual uint32 GetPos();
		
		/** 
		 \brief Get current file size if writing
		 \brief and get real file size if file for reading
		 */
		virtual	uint32 GetSize();
		
		/** 
		 \brief Set current file position
		 \param position position to set
		 \param seekType \ref IO::eFileSeek flag to set type of positioning
		 \return true if successful otherwise false.
		 */
		virtual bool Seek(int32 position, uint32 seekType);
		
		//! return true if end of file reached and false in another case
		virtual bool IsEof();
		
	private:
		int32 currentPtr;
		Vector<int8> data;
		uint32 fileAttributes;
	};
};


#endif