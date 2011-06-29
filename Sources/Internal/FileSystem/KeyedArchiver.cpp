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
#include "FileSystem/KeyedArchiver.h"
#include "Utils/Utils.h"

namespace DAVA
{
	

KeyedArchiver::KeyedArchiver()
: archive(0)
{

}

KeyedArchiver::~KeyedArchiver()
{
	
}

bool KeyedArchiver::StartEncodingToFile(const String & pathName)
{
	archive = File::Create(pathName, File::CREATE|File::WRITE);
	if (!archive)return false;
	return true;
}
	
bool KeyedArchiver::StartEncodingToFile(File *file)
{
	archive = SafeRetain(file);
	return true;
}

	

void KeyedArchiver::EncodeBool(const String & key, bool value)
{
	VariantType keyMT;
	keyMT.SetString(key);
	keyMT.Write(archive);
	
	VariantType valueMT;
	valueMT.SetBool(value);
	valueMT.Write(archive);
}

void KeyedArchiver::EncodeInt(const String & key, int32 value)
{
	VariantType keyMT;
	keyMT.SetString(key);
	keyMT.Write(archive);
	
	VariantType valueMT;
	valueMT.SetInt(value);
	valueMT.Write(archive);
}

void KeyedArchiver::EncodeFloat(const String & key, float32 value)
{
	VariantType keyMT;
	keyMT.SetString(key);
	keyMT.Write(archive);
	
	VariantType valueMT;
	valueMT.SetFloat(value);
	valueMT.Write(archive);
}

void KeyedArchiver::EncodeString(const String & key, const String & value)
{
	VariantType keyMT;
	keyMT.SetString(key);
	keyMT.Write(archive);
	
	VariantType valueMT;
	valueMT.SetString(value);
	valueMT.Write(archive);
}

void KeyedArchiver::EncodeWideString(const String & key, const WideString & value)
{
	VariantType keyMT;
	keyMT.SetString(key);
	keyMT.Write(archive);
	
	VariantType valueMT;
	valueMT.SetWideString(value);
	valueMT.Write(archive);
}
	
void KeyedArchiver::EncodeVariant(const String & key, const VariantType & value)
{
	VariantType keyMT;
	keyMT.SetString(key);
	keyMT.Write(archive);
	
	value.Write(archive);
}

void KeyedArchiver::FinishEncoding()
{
	SafeRelease(archive);
}
	
}