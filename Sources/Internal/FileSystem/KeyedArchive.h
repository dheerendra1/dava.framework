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
#ifndef __DAVAENGINE_KEYED_ARCHIVE_H__
#define __DAVAENGINE_KEYED_ARCHIVE_H__

#include "Base/BaseTypes.h"
#include "Base/BaseObject.h"
#include "FileSystem/VariantType.h"
#include "FileSystem/File.h"

namespace DAVA 
{
/**
	\ingroup filesystem
	\brief this is a class that should be used for serialization & deserialization of the items
 */
class KeyedArchive : public BaseObject
{
public:
	KeyedArchive();
	virtual ~KeyedArchive();
	
	/**
	 \brief Dumps archive consistent to console
	 */
	void Dump();

	/**
		\brief function to check if key is available in this archive
		\param[in] key string key
		\returns true if key available
	 */
	bool IsKeyExists(const String & key);
	
	/**
		\brief functions to get variable from archive
		\param[in] key string key
		\param[in] defaultValue this is value that is used if variable with this key do not exists in archive
		\returns value of variable or defaultValue if key isn't available
	 */
	bool GetBool(const String & key, bool defaultValue = false);
	/**
		\brief functions to get variable from archive
		\param[in] key string key
		\param[in] defaultValue this is value that is used if variable with this key do not exists in archive
		\returns value of variable or defaultValue if key isn't available
	 */
	int32 GetInt(const String & key, int32 defaultValue = 0);
	/**
		\brief functions to get variable from archive
		\param[in] key string key
		\param[in] defaultValue this is value that is used if variable with this key do not exists in archive
		\returns value of variable or defaultValue if key isn't available
	 */
	float32 GetFloat(const String & key, float32 defaultValue = 0.0f);
	/**
		\brief functions to get variable from archive
		\param[in] key string key
		\param[in] defaultValue this is value that is used if variable with this key do not exists in archive
		\returns value of variable or defaultValue if key isn't available
	 */
	const String & GetString(const String & key, const String & defaultValue = "");
	/**
		\brief functions to get variable from archive
		\param[in] key string key
		\param[in] defaultValue this is value that is used if variable with this key do not exists in archive
		\returns value of variable or defaultValue if key isn't available
	 */
	const WideString & GetWideString(const String & key, const WideString & defaultValue = L"");
	/**
	 \brief functions to get variable from archive
	 \param[in] key string key
	 \param[in] defaultValue this is value that is used if variable with this key do not exists in archive
	 \returns value of variable or defaultValue if key isn't available
	 */
	const uint8 *GetByteArray(const String & key, const uint8 *defaultValue = NULL);
	/**
	 \brief functions to get variable from archive
	 \param[in] key string key
	 \param[in] defaultValue this is value that is used if variable with this key do not exists in archive
	 \returns value of variable or defaultValue if key isn't available
	 */
	int32 GetByteArraySize(const String & key, int32 defaultValue = 0);
	
	
	/**
		\brief functions to get variable from archive
		\param[in] key string key
		\returns value of variable or default VariantType class if value isn't available
	 */
	const VariantType & GetVariant(const String & key);
	
	/**
		\brief function to set variable in archive
		\param[in] key string key
		\param[in] value we want to set for this key
	 */
	void SetBool(const String & key, bool value);
	/**
		\brief function to set variable in archive
		\param[in] key string key
		\param[in] value we want to set for this key
	 */
	void SetInt(const String & key, int32 value);
	/**
		\brief function to set variable in archive
		\param[in] key string key
		\param[in] value we want to set for this key
	 */
	void SetFloat(const String & key, float32 value);
	/**
		\brief function to set variable in archive
		\param[in] key string key
		\param[in] value we want to set for this key
	 */
	void SetString(const String & key, const String & value);
	/**
		\brief function to set variable in archive
		\param[in] key string key
		\param[in] value we want to set for this key
	 */
	void SetWideString(const String & key, const WideString & value);
	/**
	 \brief function to set variable in archive
	 \param[in] key string key
	 \param[in] value we want to set for this key
	 \param[in] arraySize size fo the array we want tot save
	 */
	void SetByteArray(const String & key, const uint8 * value, int32 arraySize);
	/**
		\brief function to set variable in archive
		\param[in] key string key
		\param[in] value we want to set for this key
	 */
	void SetVariant(const String & key, const VariantType & value);
	
	/**
		\brief this function loads data from given file
		\param[in] pathName relative pathname in application documents folder
	 */
	bool Load(const String & pathName);
	/**
		\brief this function saves data to given file
		\param[in] pathName relative pathname in application documents folder
	 */
	bool Save(const String & pathName);

	/**
	 \brief this function loads data from given file
	 \param[in] file to load from
	 */
	bool Load(File *file);
	/**
	 \brief this function saves data to given file
	 \param[in] file to save
	 */
	bool Save(File *file);
	

//	yaml
// 	/**
// 		\brief this function loads data from given yaml file
// 		\param[in] pathName relative pathname in application documents folder
// 	*/
// 	bool LoadFromYaml(const String & pathName);
// 
// 	/**
// 		\brief this function saes data to given yaml file
// 		\param[in] pathName relative pathname in application documents folder
// 	*/
// 	bool SaveToYaml(const String & pathName);

private:
	Map<String, VariantType> objectMap;
};
	
	
};

#endif // __DAVAENGINE_KEYED_ARCHIVE_H__