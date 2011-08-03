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
#ifndef __LOGENGINE_FILE_H__
#define __LOGENGINE_FILE_H__

#include "BaseTypes.h"
#include "IBase.h"

namespace Log
{
namespace IO
{

//! File attributes enumeration
//! Remember engine support only 
//! EFA_OPEN | EFA_READ
//! EFA_CREATE | EFA_WRITE
//! EFA_APPEND | EFA_WRITE

enum eFileAttributes
{
	EFA_CREATE	= 0x1,
	EFA_OPEN	= 0x2,
	EFA_READ	= 0x4,
	EFA_WRITE	= 0x8,
	EFA_APPEND	= 0x10
};

//! File seek enumeration
enum eFileSeek
{
	EFS_START		= 1, //! Seek from start of file
	EFS_END			= 2, //! Seek from end of file
    EFS_CURRENT		= 3, //! Seek from current file position relatively
};

//! \brief File interface, to access for device file-subsystem
//! Files in our system is always binary, and text management
//! operations must be implemented by engine-hi-end-user.
//! this interface is maximally simplified
class	IFile : public IBase
{
public:

	//! destructor
	virtual ~IFile() {};

	//! \brief Get this file name
	virtual	const char8 *		GetFilename() = 0;
    
	//! \brief Get this file full pathname
	virtual const char8 *		GetPathname() = 0;
	
	//! \brief Write [dataSize] bytes to this file from [pointerToData]
	//! \param pointerToData function get data from this pointer
	//! \param dataSize size of data we want to write
	//! \return number of bytes actually written
	virtual uint32				Write(void * pointerToData, uint32 dataSize) = 0;
	
	//! \brief Read [dataSize] bytes from this file to [pointerToData] 
	//! \param pointerToData function write data to this pointer
	//! \param dataSize size of data we want to read
	//! \return number of bytes actually read
	virtual uint32				Read(void * pointerToData, uint32 dataSize) = 0;

	//! \brief Get current file position
	virtual uint32				GetPos() = 0;

	//! \brief Get current file size if writing
	//! \brief and get real file size if file for reading
	virtual	uint32				GetSize() = 0;

	//! \brief Set current file position
	//! \param position position to set
	//! \param seekType \ref IO::eFileSeek flag to set type of positioning
	//! \return true if successful otherwise false.
	virtual bool				Seek(uint32 position, uint32 seekType) = 0;


	//! return true if end of file reached and false in another case
	virtual bool				IsEof() = 0;

	//! \brief Read string
	//! read null-terminated string from current position in file
	//! implemented inside this class using Read function
	//! \param[in] string - string data loaded to this variable
	//! \return true if success otherwise false
	virtual bool				ReadString(String & string);

	//! \brief Write string
	//! write null-terminated string from current position in file
	//! implemented inside this class using Write function
	//! \param[in] string - string data loaded to this variable
	//! \return true if success otherwise false
	virtual bool				WriteString(const String & string);


};


}; // end of namespace IO
}; // end of namespace Log


#endif // __LOGENGINE_FILE_H__


