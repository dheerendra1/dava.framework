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
#ifndef __DAVAENGINE_VARIANTTYPE_H__
#define __DAVAENGINE_VARIANTTYPE_H__

#include "Base/BaseTypes.h"
#include "FileSystem/File.h"

namespace DAVA 
{

/**
	\ingroup filesystem
	\brief Class to store value of all basic types in one instance. Can be used for various serialization / deserialization purposes.
 */
class VariantType
{
public:
	VariantType();
	~VariantType();
	enum
	{
		TYPE_NONE = 0,
		TYPE_BOOLEAN,
		TYPE_INT,
		TYPE_FLOAT,
		TYPE_STRING,
		TYPE_WIDE_STRING,
		TYPE_BYTE_ARRAY
	};
	uint8 type;
	union  
	{
		bool boolValue;
		int32 intValue;
		float32 floatValue;
	};
	String stringValue;	
	WideString wideStringValue;
	Vector<uint8> arrayValue;
	
	// Functions
	
	/**
		\brief Function to set bool value to variant type variable
		\param[in] value	value to set
	 */
	void SetBool(bool value);

	/**
		\brief Function to set int value to variant type variable
		\param[in] value	value to set
	 */
	void SetInt(int32 value);

	/**
		\brief Function to set float value to variant type variable
		\param[in] value	value to set
	 */
	void SetFloat(float32 value);

	/**
		\brief Function to set string value to variant type variable
		\param[in] value	value to set
	 */
	void SetString(const String & value);

	/**
		\brief Function to set wide string value to variant type variable
		\param[in] value	value to set
	 */
	void SetWideString(const WideString & value);

	/**
	 \brief Function to set byte array value to variant type variable
	 \param[in] value	value to set
	 \param[in] arraySizeInBytes	size of the array in bytes
	 */
	void SetByteArray(const uint8 *array, int32 arraySizeInBytes);
	
	/**
		\brief Function to return bool value from variable
		\returns value of variable, or generate assert if variable type is different
	 */
	bool AsBool();

	/**
		\brief Function to return int value from variable
		\returns value of variable, or generate assert if variable type is different
	 */
	int32 AsInt();

	/**
		\brief Function to return float value from variable
		\returns value of variable, or generate assert if variable type is different
	 */
	float32 AsFloat();

	/**
		\brief Function to return string value from variable
		\returns value of variable, or generate assert if variable type is different
	 */
	const String & AsString();

	/**
		\brief Function to return wide string value from variable
		\returns value of variable, or generate assert if variable type is different
	 */
	const WideString & AsWideString();

	/**
	 \brief Function to return array from variable
	 \returns value of variable, or generate assert if variable type is different
	 */
	const uint8 *AsByteArray();
	/**
	 \brief Function to return array size from variable
	 \returns array size in bytes variable, or generate assert if variable type is different
	 */
	int32 AsByteArraySize();
	
	// File read & write helpers
	
	/**
		\brief Function to write variable to file, from it current position
		\returns true if variable written successfully
	 */
	bool Write(File * fp) const;
	
	/**
		\brief Function to read variable to file, to it current position
		\returns true if variable read successfully
	 */
	bool Read(File * fp);
};
	
	
};

#endif // __DAVAENGINE_VARIANTTYPE_H__