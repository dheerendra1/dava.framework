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
#include "FileSystem/VariantType.h"

namespace DAVA 
{

VariantType::VariantType()
:	type(TYPE_NONE)
,	intValue(0)
{
}

VariantType::~VariantType()
{


}

void VariantType::SetBool(bool value)
{
	type = TYPE_BOOLEAN;
	boolValue = value;
}
void VariantType::SetInt(int32 value)
{
	type = TYPE_INT;
	intValue = value;
}

void VariantType::SetFloat(float32 value)
{
	type = TYPE_FLOAT;
	floatValue = value;
}

void VariantType::SetString(const String & value)
{
	type = TYPE_STRING;
	stringValue = value;
}
void VariantType::SetWideString(const WideString & value)
{
	type = TYPE_WIDE_STRING;
	wideStringValue = value;
}

void VariantType::SetByteArray(const uint8 *array, int32 arraySizeInBytes)
{
	type = TYPE_BYTE_ARRAY;
	for (int32 i = 0; i < arraySizeInBytes; i++) 
	{
		arrayValue.push_back(array[i]);
	}
}

	
bool VariantType::AsBool()
{
	DVASSERT(type == TYPE_BOOLEAN);
	return boolValue;
}

int32  VariantType::AsInt()
{
	DVASSERT(type == TYPE_INT);
	return intValue;
}

float32  VariantType::AsFloat()
{
	DVASSERT(type == TYPE_FLOAT);
	return floatValue;
}

const String &  VariantType::AsString()
{
	DVASSERT(type == TYPE_STRING);
	return stringValue;
}

const WideString & VariantType::AsWideString()
{
	DVASSERT(type == TYPE_WIDE_STRING);
	return wideStringValue;
}
	
const uint8 *VariantType::AsByteArray()
{
	DVASSERT(type == TYPE_BYTE_ARRAY);
	return &arrayValue[0];
}
	
int32 VariantType::AsByteArraySize()
{
	DVASSERT(type == TYPE_BYTE_ARRAY);
	return (int32)arrayValue.size();
}
	


	
bool VariantType::Write(File * fp) const
{
	DVASSERT(type != TYPE_NONE)
	int32 written = fp->Write(&type, 1);
	if (written != 1)return false;
	
	switch(type)
	{
	case TYPE_BOOLEAN:
		{
			written = fp->Write(&boolValue, 1);
			if (written != 1)return false;
		}
		break;
	case TYPE_INT:
		{
			written = fp->Write(&intValue, 4);
			if (written != 4)return false;
		}
		break;	
	case TYPE_FLOAT:
		{
			written = fp->Write(&floatValue, 4);
			if (written != 4)return false;
		}
		break;	
	case TYPE_STRING:
		{
			int32 len = (int32)stringValue.length();
			written = fp->Write(&len, 4);
			if (written != 4)return false;
			
			written = fp->Write(stringValue.c_str(), len);
			if (written != len)return false;
		}
		break;	
	case TYPE_WIDE_STRING:
		{
			int32 len = (int32)wideStringValue.length();
			written = fp->Write(&len, 4);
			if (written != 4)return false;
			
			written = fp->Write(wideStringValue.c_str(), len * sizeof(wchar_t));
			if (written != len * (int)sizeof(wchar_t))return false;
		}
		break;
	case TYPE_BYTE_ARRAY:
		{
			int32 len = (int32)arrayValue.size();
			written = fp->Write(&len, 4);
			if (written != 4)return false;
			
			written = fp->Write(&arrayValue[0], len);
			if (written != len)return false;
		}
		break;	
	}
	return true;
}

bool VariantType::Read(File * fp)
{
	int32 read = fp->Read(&type, 1);
	if (read == 0)return false;
	
	switch(type)
	{
		case TYPE_BOOLEAN:
		{
			read = fp->Read(&boolValue, 1);
			if (read != 1)return false;
		}
		break;
		case TYPE_INT:
		{
			read = fp->Read(&intValue, 4);
			if (read != 4)return false;
		}
			break;	
		case TYPE_FLOAT:
		{
			read = fp->Read(&floatValue, 4);
			if (read != 4)return false;
		}
			break;	
		case TYPE_STRING:
		{
			int32 len;
			read = fp->Read(&len, 4);
			if (read != 4)return false;
			
			char *buf = new char[len + 1];
			read = fp->Read(buf, len);
			buf[len] = 0;
			stringValue = buf;
			delete [] buf;
			if (read != len)return false;
			
		}
			break;	
		case TYPE_WIDE_STRING:
		{ 
			int32 len;
			read = fp->Read(&len, 4);
			if (read != 4)return false;
			
			wideStringValue.resize(len);
			for (int k = 0; k < len; ++k)
			{
				wchar_t c;
				read = fp->Read(&c, sizeof(wchar_t));
				if (read != sizeof(wchar_t))return false;
				wideStringValue[k] = c;
			}
		}
			break;
		case TYPE_BYTE_ARRAY:
		{
			int32 len;
			read = fp->Read(&len, 4);
			if (read != 4)return false;
			
			arrayValue.resize(len);
			read = fp->Read(&arrayValue[0], len);
			if (read != len)return false;
		}
			break;	
		default:
		{
			//DVASSERT(0 && "Something went wrong with VariantType");
		}
	}
	return true;
}
	
	
	
};