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
#include "FileSystem/KeyedArchive.h"
#include "FileSystem/File.h"
#include "Utils/Utils.h"

namespace DAVA 
{
	
KeyedArchive::KeyedArchive()
{
}
KeyedArchive::~KeyedArchive()
{
}

bool KeyedArchive::Load(const String & pathName)
{
	File * archive = File::Create(pathName, File::OPEN|File::READ);
	if (!archive)return false;

	Load(archive);

	SafeRelease(archive);
	return true;
}
	
bool KeyedArchive::Load(File *archive)
{
	while(!archive->IsEof())
	{
		VariantType key;
		key.Read(archive);
		if (archive->IsEof())break;
		VariantType value;
		value.Read(archive);
		objectMap[key.AsString()] = value;
	}
	return true;
}
	
bool KeyedArchive::Save(const String & pathName)
{
	File * archive = File::Create(pathName, File::CREATE|File::WRITE);
	if (!archive)return false;

	Save(archive);

	SafeRelease(archive);
	return true;
}
	
bool KeyedArchive::Save(File *archive)
{
	for (Map<String, VariantType>::iterator it = objectMap.begin(); it != objectMap.end(); ++it)
	{
		VariantType key;
		key.SetString(it->first);
		key.Write(archive);
		it->second.Write(archive);
	}
	return true;
}


void KeyedArchive::SetBool(const String & key, bool value)
{
	VariantType variantValue;
	variantValue.SetBool(value);
	objectMap[key] = variantValue;
}

void KeyedArchive::SetInt(const String & key, int32 value)
{
	VariantType variantValue;
	variantValue.SetInt(value);
	objectMap[key] = variantValue;
}

void KeyedArchive::SetFloat(const String & key, float32 value)
{
	VariantType variantValue;
	variantValue.SetFloat(value);
	objectMap[key] = variantValue;
}

void KeyedArchive::SetString(const String & key, const String & value)
{
	VariantType variantValue;
	variantValue.SetString(value);
	objectMap[key] = variantValue;
}

void KeyedArchive::SetWideString(const String & key, const WideString & value)
{
	VariantType variantValue;
	variantValue.SetWideString(value);
	objectMap[key] = variantValue;
}

void KeyedArchive::SetByteArray(const String & key, const uint8 * value, int32 arraySize)
{
	VariantType variantValue;
	variantValue.SetByteArray(value, arraySize);
	objectMap[key] = variantValue;
}



void KeyedArchive::SetVariant(const String & key, const VariantType & value)
{
	objectMap[key] = value;
}
	
bool KeyedArchive::IsKeyExists(const String & key)
{
	Map<String, VariantType>::iterator t = objectMap.find(key);
	if (t != objectMap.end())
	{
		return true;
	}
	return false;
}

bool KeyedArchive::GetBool(const String & key, bool defaultValue)
{
	if (IsKeyExists(key))
		return objectMap[key].AsBool();
	return defaultValue;
}

int32 KeyedArchive::GetInt(const String & key, int32 defaultValue)
{
	if (IsKeyExists(key))
		return objectMap[key].AsInt();
	return defaultValue;
}

float32 KeyedArchive::GetFloat(const String & key, float32 defaultValue)
{
	if (IsKeyExists(key))
		return objectMap[key].AsFloat();
	return defaultValue;
}

const String & KeyedArchive::GetString(const String & key, const String & defaultValue)
{
	if (IsKeyExists(key))
		return objectMap[key].AsString();
	return String(defaultValue); // return new object instead of returning reference to defaultValue, because it's unsafe
}
const WideString & KeyedArchive::GetWideString(const String & key, const WideString & defaultValue)
{
	if (IsKeyExists(key))
		return objectMap[key].AsWideString();
	return WideString(defaultValue); // return new object instead of returning reference to defaultValue, because it's unsafe
}
	
const uint8 *KeyedArchive::GetByteArray(const String & key, const uint8 *defaultValue)
{
	if (IsKeyExists(key))
		return objectMap[key].AsByteArray();
	return defaultValue;
}
int32 KeyedArchive::GetByteArraySize(const String & key, int32 defaultValue)
{
	if (IsKeyExists(key))
		return objectMap[key].AsByteArraySize();
	return defaultValue;
}
	

const VariantType & KeyedArchive::GetVariant(const String & key)
{
	return objectMap[key];
}

void KeyedArchive::DeleteKey(const String & key)
{
	objectMap.erase(key);
}

void KeyedArchive::DeleteAllKeys()
{
	objectMap.clear();
}

	
void KeyedArchive::Dump()
{
	Logger::Info("============================================================");
	Logger::Info("--------------- Archive Currently contain ----------------");
	for(Map<String, VariantType>::iterator it = objectMap.begin(); it != objectMap.end(); ++it)
	{
		switch(it->second.type)
		{
			case VariantType::TYPE_BOOLEAN:
			{
				if(it->second.boolValue)
				{
					Logger::Debug("%s : true", it->first.c_str());
				}
				else 
				{
					Logger::Debug("%s : false", it->first.c_str());
				}

			}
				break;
			case VariantType::TYPE_INT:
			{
				Logger::Debug("%s : %d", it->first.c_str(), it->second.intValue);
			}
				break;	
			case VariantType::TYPE_FLOAT:
			{
				Logger::Debug("%s : %f", it->first.c_str(), it->second.floatValue);
			}
				break;	
			case VariantType::TYPE_STRING:
			{
				Logger::Debug("%s : %s", it->first.c_str(), it->second.stringValue.c_str());
			}
				break;	
			case VariantType::TYPE_WIDE_STRING:
			{
				Logger::Debug("%s : %S", it->first.c_str(), it->second.wideStringValue.c_str());
			}
				break;
		}
	}
	Logger::Info("============================================================");
}





	
	
};

