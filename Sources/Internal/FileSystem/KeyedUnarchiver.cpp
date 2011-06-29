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
#include "FileSystem/KeyedUnarchiver.h"
#include "Utils/Utils.h"

namespace DAVA 
{
	
KeyedUnarchiver::KeyedUnarchiver()
{
}
KeyedUnarchiver::~KeyedUnarchiver()
{
}
	
bool KeyedUnarchiver::UnarchiveFile(const String & pathName)
{
	File * archive = File::Create(pathName, File::OPEN|File::READ);
	if (!archive)return false;
	
	UnarchiveFile(archive);
	
	SafeRelease(archive);
	return true;
}
	
bool KeyedUnarchiver::UnarchiveFile(File *file)
{
	while(!file->IsEof())
	{
		VariantType key;
		key.Read(file);
		VariantType value;
		value.Read(file);
		objectMap[key.AsString()] = value;
	}
	return true;
}

	
bool KeyedUnarchiver::IsKeyExists(const String & key)
{
	Map<String, VariantType>::iterator t = objectMap.find(key);
	if (t != objectMap.end())
	{
		return true;
	}
	return false;
}
	
bool KeyedUnarchiver::DecodeBool(const String & key)
{
	return objectMap[key].AsBool();
}

int32 KeyedUnarchiver::DecodeInt(const String & key)
{
	return objectMap[key].AsInt();
}

float32 KeyedUnarchiver::DecodeFloat(const String & key)
{
	return objectMap[key].AsFloat();
}

const String & KeyedUnarchiver::DecodeString(const String & key)
{
	return objectMap[key].AsString();
}
const WideString & KeyedUnarchiver::DecodeWideString(const String & key)
{
	return objectMap[key].AsWideString();
}

const VariantType & KeyedUnarchiver::DecodeVariant(const String & key)
{
	return objectMap[key];
}


};

