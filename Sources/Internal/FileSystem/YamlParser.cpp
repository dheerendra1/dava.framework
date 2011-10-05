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
#include "FileSystem/YamlParser.h"
#include "FileSystem/Logger.h"
#include "Utils/Utils.h"
#include <sstream>
#include "yaml.h"
#include "Utils/UTF8Utils.h"
#include "FileSystem/FileSystem.h"

namespace DAVA 
{

YamlNode::YamlNode(eType _type)
{
	type = _type;
	mapIndex = 0;
	mapCount = 0;
}

YamlNode::~YamlNode()
{
	for (Map<String, YamlNode*>::iterator t = objectMap.begin(); t != objectMap.end(); ++t)
	{
		YamlNode * object = t->second;
		SafeRelease(object);
	}
	objectMap.clear();
	
	int32 size = (int32)objectArray.size();
	for (int32 k = 0; k < size; ++k)
	{
		SafeRelease(objectArray[k]);
	}
	objectArray.clear();
}
	
void YamlNode::Print(int32 identation)
{
	if (type == TYPE_STRING)
	{
		const char * str = nwStringValue.c_str();
		printf("%s", str);
	}else if (type == TYPE_ARRAY)
	{
		printf("[");
		for (int32 k = 0; k < (int32)objectArray.size(); ++k)
		{
			objectArray[k]->Print(identation + 1);
			printf(",");
		}
		printf("]");
	}else if (type == TYPE_MAP)
	{
		printf("{");
		for (Map<String, YamlNode*>::iterator t = objectMap.begin(); t != objectMap.end(); ++t)
		{
			printf("key(%s, %d): ", t->first.c_str(), t->second->mapIndex);
			t->second->Print(identation + 1);
			printf(",");
		}
		printf("}");
	}
}
	
int32 YamlNode::GetCount()
{
	switch (type) 
	{
		case TYPE_MAP: return (int32)objectMap.size();
		case TYPE_ARRAY: return (int32)objectArray.size();
	}
	return 1;
}
int32  YamlNode::AsInt()
{
	int32 val;
	std::istringstream os(nwStringValue);
	os >> val;
	return val;
}

float32	YamlNode::AsFloat()
{
//	float32 val;
//	std::istringstream os(nwStringValue);
//	os >> val;
//	return val;
	
	float32 ret;
	sscanf(nwStringValue.c_str(), "%f", &ret);
	return ret;
}

const String & YamlNode::AsString()
{
	return nwStringValue;
}

bool YamlNode::AsBool()
{
	return ("true" == nwStringValue);
}

const WideString & YamlNode::AsWString()
{
	return stringValue;
}
	
Vector2	YamlNode::AsPoint()
{
	Vector2 result;
	if (type == TYPE_ARRAY)
	{
		YamlNode * x = Get(0);
		if (x)result.x = x->AsFloat();
		YamlNode * y = Get(1);
		if (y)result.y = y->AsFloat();
	}
	return result;
}

Vector2 YamlNode::AsVector2()
{
	return AsPoint();
}
	
Rect	YamlNode::AsRect()
{
	Rect result;
	if (type == TYPE_ARRAY)
	{
		YamlNode * x = Get(0);
		if (x)result.x = x->AsFloat();
		YamlNode * y = Get(1);
		if (y)result.y = y->AsFloat();
		YamlNode * dx = Get(2);
		if (dx)result.dx = dx->AsFloat();
		YamlNode * dy = Get(3);
		if (dy)result.dy = dy->AsFloat();
	}
	return result;
}
	
Vector<YamlNode*> & YamlNode::AsVector()
{
	return objectArray;
}

Map<String, YamlNode*> & YamlNode::AsMap()
{
	return objectMap;
}

	

YamlNode * YamlNode::Get(int32 index)
{
	if (type == TYPE_ARRAY)
	{
		return objectArray[index];
	}else if (type == TYPE_MAP)
	{
		return objectArray[index];
		/*Map<String, YamlNode*>::const_iterator end = objectMap.end();
		for (Map<String, YamlNode*>::iterator t = objectMap.begin(); t != end; ++t)
		{	
			YamlNode * n = t->second;
			if (n->mapIndex == index)return n;
		}*/
	}
	return 0;
}
	
static String emptyString = String("");
		
const String &	YamlNode::GetItemKeyName(int32 index)
{
	if (type == TYPE_MAP)
	{
		Map<String, YamlNode*>::const_iterator end = objectMap.end();
		for (Map<String, YamlNode*>::iterator t = objectMap.begin(); t != end; ++t)
		{	
			YamlNode * n = t->second;
			if (n->mapIndex == index)return t->first;
		}
	}	
	return emptyString;
}
	
YamlNode * YamlNode::Get(const String & name)
{
	if (type == TYPE_MAP)
	{
		Map<String, YamlNode*>::iterator t;
		if ((t = objectMap.find(name)) != objectMap.end())
		{
			return t->second;
		}
	}	
	return 0;
}


	
	
/* Set a generic reader. */
int read_handler(void *ext, unsigned char *buffer, size_t size, size_t *length)
{
	YamlParser::YamlDataHolder * holder = (YamlParser::YamlDataHolder*)ext;
	int32 sizeToWrite = Min((uint32)size, holder->fileSize-holder->dataOffset);
	memcpy(buffer, holder->data+holder->dataOffset, sizeToWrite);
	*length = sizeToWrite;

	holder->dataOffset += sizeToWrite;

	return 1;
}
	
YamlParser * YamlParser::Create(const String & fileName)
{
	YamlParser * parser = new YamlParser();
	if (parser)
	{
		bool parseResult = parser->Parse(fileName);
		if(!parseResult)
		{
			SafeRelease(parser);
			return 0;
		}
	}
	return parser;
}
	
bool YamlParser::Parse(const String & pathName)
{
	yaml_parser_t parser;
	yaml_event_t event;
	
	int done = 0;
	
	/* Create the Parser object. */
	yaml_parser_initialize(&parser);
	
	yaml_parser_set_encoding(&parser, YAML_UTF8_ENCODING);
	
	/* Set a string input. */
	//yaml_parser_set_input_string(&parser, (const unsigned char*)pathName.c_str(), pathName.length());
	

	File * yamlFile = File::Create(pathName, File::OPEN | File::READ);
    if (!yamlFile)
    {
        return false;
    }
    
	dataHolder.fileSize = yamlFile->GetSize();
	dataHolder.data = new uint8[dataHolder.fileSize];
	dataHolder.dataOffset = 0;
	yamlFile->Read(dataHolder.data, dataHolder.fileSize);
	yamlFile->Release();
	/* Set a file input. */
	//FILE *input = fopen(FileSystem::Instance()->SystemPathForFrameworkPath(pathName).c_str(), "rb");
	//if (!input)
	//	return false;
	//yaml_parser_set_input_file(&parser, input);
	
	yaml_parser_set_input(&parser, read_handler, &dataHolder);
	YamlNode * mapKey = 0;
//	YamlNode * mapValue = 0;

	/* Read the event sequence. */
	while (!done) 
	{
		
		/* Get the next event. */
		if (!yaml_parser_parse(&parser, &event))
		{
			Logger::Error("[YamlParser::Parse] error: type: %d %s line: %d pos: %d", parser.error, parser.problem, parser.problem_mark.line, parser.problem_mark.column);
			break;
		}
		
		/*if (event.encoding != YAML_UTF8_ENCODING)
		{
			Logger::Debug("wrong encoding");
		}*/
		
		switch(event.type)
		{
		case YAML_ALIAS_EVENT:
			Logger::Debug("alias: %s", event.data.alias.anchor);
			break;
		
		case YAML_SCALAR_EVENT:
			{
				YamlNode * node = new YamlNode(YamlNode::TYPE_STRING);

				/*CFStringRef s = CFStringCreateWithBytes(NULL, event.data.scalar.value, event.data.scalar.length, kCFStringEncodingUTF8, false);
				int32 length = CFStringGetLength(s); 
				node->stringValue.resize(length); 
				for (int i = 0; i < length; i++) 
				{
					UniChar uchar = CFStringGetCharacterAtIndex(s, i);
					node->stringValue[i] = (wchar_t)uchar;
				}
				CFRelease(s);
				node->nwStringValue = String((const char*)event.data.scalar.value); */
				node->nwStringValue = String((const char*)event.data.scalar.value);
				UTF8Utils::EncodeToWideString((uint8*)event.data.scalar.value, (int32)event.data.scalar.length, node->stringValue);
				
				
				if (objectStack.size() == 0)
				{
					rootObject = node;
				}else
				{
					YamlNode * topContainer = objectStack.top();
					if (topContainer->type == YamlNode::TYPE_MAP)
					{
						if (mapKey == 0)mapKey = node;
						else
						{
							if (topContainer->Get(mapKey->nwStringValue))
							{
								Logger::Error("[YamlParser::Parse] error in %s: attempt to create duplicate map node: %s", pathName.c_str(), mapKey->nwStringValue.c_str());
							}
							
							node->mapIndex = topContainer->mapCount ++;
							topContainer->objectMap[mapKey->nwStringValue] = node;
							topContainer->objectArray.push_back(SafeRetain(node)); // duplicate in array
							SafeDelete(mapKey);
						}
					}else if (topContainer->type == YamlNode::TYPE_ARRAY)
					{
						topContainer->objectArray.push_back(node);
					}
				}
				
//				NSLog()
//				wprintf(L"scalar: %s %S\n", event.data.scalar.value, node->stringValue.c_str());
//				Logger::Debug("scalar: %s %d", event.data.scalar.value, length);
//				CFIndex length = CFStringGetLength(s);
//				UniChar *buffer = malloc(length * sizeof(UniChar));
//				CFStringGetCharacters(str, CFRangeMake(0, length), buffer);
//				node->stringValue = (char buffer;
//				free(buffer);
				
				//node->stringValue = event.data.scalar.value;
			}
			break;
		
		case YAML_DOCUMENT_START_EVENT:
			//Logger::Debug("document start:");
			break;
		
		case YAML_DOCUMENT_END_EVENT:
			//Logger::Debug("document end:");
			break;

		case YAML_SEQUENCE_START_EVENT:
			{
//				printf("[");
				YamlNode * node = new YamlNode(YamlNode::TYPE_ARRAY);
				if (objectStack.size() == 0)
					rootObject = node;
				else
				{
					YamlNode * topContainer = objectStack.top();
					if (topContainer->type == YamlNode::TYPE_MAP)
					{
						if (mapKey == 0)
						{
							printf("Something wrong");
						}
						else
						{
//							String s = String(mapKey->stringValue.begin(), mapKey->stringValue.end());
//							printf("put to map: %s\n", s.c_str());
							
							if (topContainer->Get(mapKey->nwStringValue))
							{
								Logger::Error("[YamlParser::Parse] error in %s: attempt to create duplicate map node: %s", pathName.c_str(), mapKey->nwStringValue.c_str());
							}
							
							node->mapIndex = topContainer->mapCount ++;
							topContainer->objectMap[mapKey->nwStringValue] = node;
							topContainer->objectArray.push_back(SafeRetain(node));
							SafeDelete(mapKey);
						}
					}else if (topContainer->type == YamlNode::TYPE_ARRAY)
					{
						topContainer->objectArray.push_back(node);
					}
				}
				objectStack.push(node);
			}break;
				
		case YAML_SEQUENCE_END_EVENT:
			{
//				printf("]");
				objectStack.pop();
			}break;
		
		case YAML_MAPPING_START_EVENT:
			{
//				printf("{");
				YamlNode * node = new YamlNode(YamlNode::TYPE_MAP);
				if (objectStack.size() == 0)
					rootObject = node;
				else
				{
					YamlNode * topContainer = objectStack.top();
					
					if (topContainer->type == YamlNode::TYPE_MAP)
					{
						if (mapKey == 0)
						{
//							printf("Something wrong");
						}
						else
						{
							//String s = String(mapKey->stringValue.begin(), mapKey->stringValue.end());
//							printf("put to map: %s\n", s.c_str());

							if (topContainer->Get(mapKey->nwStringValue))
							{
								Logger::Error("[YamlParser::Parse] error in %s: attempt to create duplicate map node: %s", pathName.c_str(), mapKey->nwStringValue.c_str());
							}
							
							node->mapIndex = topContainer->mapCount ++;
							topContainer->objectMap[mapKey->nwStringValue] = node;
							node->stringValue = mapKey->stringValue;
							node->nwStringValue = mapKey->nwStringValue;
							topContainer->objectArray.push_back(SafeRetain(node));
							SafeDelete(mapKey);
						}
					}else if (topContainer->type == YamlNode::TYPE_ARRAY)
					{
						topContainer->objectArray.push_back(node);
					}
				}
				objectStack.push(node);
			}
			break;
				
		case YAML_MAPPING_END_EVENT:
			{
//				printf("}");
				objectStack.pop();
			}
			break;
		};

		/* Are we finished? */
		done = (event.type == YAML_STREAM_END_EVENT);
		
		/* The application is responsible for destroying the event object. */
		yaml_event_delete(&event);
		
	}
	
	//rootObject->Print(0);
	
	//printf("%s (%d events)\n", (error ? "FAILURE" : "SUCCESS"), count);
	
	/* Destroy the Parser object. */
	yaml_parser_delete(&parser);
//	fclose(input);
	SafeDeleteArray(dataHolder.data);
	
	return true;
}
	

	
YamlParser::YamlParser()
{
	rootObject = 0;
	dataHolder.data = 0;
}

YamlParser::~YamlParser()
{
	SafeRelease(rootObject);
	//SafeDeleteArray(dataHolder.data);
}

std::vector<String> split(const String& s, const String& delim, const bool keep_empty = true) 
{
	std::vector<String> result;
	if (delim.empty()) {
		result.push_back(s);
		return result;
	}
	String::const_iterator substart = s.begin(), subend;
	while (true) {
		subend = std::search(substart, s.end(), delim.begin(), delim.end());
		String temp(substart, subend);
		if (keep_empty || !temp.empty()) {
			result.push_back(temp);
		}
		if (subend == s.end()) {
			break;
		}
		substart = subend + delim.size();
	}
	return result;
}
YamlNode * YamlParser::GetNodeByPath(const String & path)
{
// 	std::vector<String> result = split(path, ".");
// 	if (result.size() > 0)
// 	{
// 		//int pos = 0;
// 		for (int pos = 0; pos < result.size(); ++pos)
// 		{
// 			
// 		}
// 	}
	return 0;
}
	
YamlNode * YamlParser::GetRootNode()
{
	return rootObject;
}


}