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
#include "FileSystem/LocalizationSystem.h"
#include "Utils/Utils.h"
#include "FileSystem/Logger.h"
#include "yaml/yaml.h"
#include "Utils/UTF8Utils.h"
#include "Debug/DVAssert.h"
#include "FileSystem/FileSystem.h"
#ifdef __DAVAENGINE_IPHONE__
#include "FileSystem/LocalizationIPhone.h"
#endif


namespace DAVA 
{

LocalizationSystem::LocalizationSystem()
{
	langId = "en";

	dataHolder = new YamlParser::YamlDataHolder();
	dataHolder->data = 0;
}

LocalizationSystem::~LocalizationSystem()
{
	// release all memory allocated by strings
	for (List<StringFile*>::reverse_iterator it = stringsList.rbegin(); it != stringsList.rend(); ++it)
	{
		StringFile * file = *it;
		SafeDelete(file);
	}		
	stringsList.clear();

	SafeDeleteArray(dataHolder->data);
	SafeDelete(dataHolder);

}
	
void LocalizationSystem::InitWithDirectory(const String &directoryPath)
{
#ifdef __DAVAENGINE_IPHONE__
	LocalizationIPhone::SelecePreferedLocalizationForPath(directoryPath);
#endif
	LoadStringFile(directoryPath + "/" + langId.c_str() + ".yaml");
}
	
const String &LocalizationSystem::GetCurrentLocale()
{
	return langId;
}
	
void LocalizationSystem::SetCurrentLocale(const String &newLangId)
{//TODO: add reloading strings data on langId changing
	langId = newLangId;
}
	
LocalizationSystem::StringFile * LocalizationSystem::LoadFromYamlFile(const String & pathName)
{
	yaml_parser_t parser;
	yaml_event_t event;
	
	int done = 0;
	
	/* Create the Parser object. */
	yaml_parser_initialize(&parser);
	
	yaml_parser_set_encoding(&parser, YAML_UTF8_ENCODING);
	
	File * yamlFile = File::Create(pathName, File::OPEN | File::READ);
	dataHolder->fileSize = yamlFile->GetSize();
	dataHolder->data = new uint8[dataHolder->fileSize];
	dataHolder->dataOffset = 0;
	yamlFile->Read(dataHolder->data, dataHolder->fileSize);
	yamlFile->Release();
	
	yaml_parser_set_input(&parser, read_handler, dataHolder);
	
	WideString key;
	WideString value;
	bool isKey = true;
	StringFile * strFile = new StringFile();
	
	/* Read the event sequence. */
	while (!done) 
	{
		
		/* Get the next event. */
		if (!yaml_parser_parse(&parser, &event))
		{
			Logger::Debug("parsing error: type: %d %s line: %d pos: %d", parser.error, parser.problem, parser.problem_mark.line, parser.problem_mark.column);
			SafeDelete(strFile);
			break;
		}
				
		switch(event.type)
		{
			case YAML_ALIAS_EVENT:
				//Logger::Debug("alias: %s", event.data.alias.anchor);
				break;
				
			case YAML_SCALAR_EVENT:
			{
				
				if (isKey)
				{
					UTF8Utils::EncodeToWideString((uint8*)event.data.scalar.value, (int32)event.data.scalar.length, key);
				}else 
				{
					UTF8Utils::EncodeToWideString((uint8*)event.data.scalar.value, (int32)event.data.scalar.length, value);
					strFile->strings[key] = value;
				}
				
				isKey = !isKey;
			}
			break;
				
			case YAML_DOCUMENT_START_EVENT:
			{
				//Logger::Debug("document start:");
			}break;
				
			case YAML_DOCUMENT_END_EVENT:
			{	//Logger::Debug("document end:");
			}
				break;
				
			case YAML_SEQUENCE_START_EVENT:
			{
			}break;
				
			case YAML_SEQUENCE_END_EVENT:
			{
			}break;
				
			case YAML_MAPPING_START_EVENT:
			{
			}
			break;
				
			case YAML_MAPPING_END_EVENT:
			{
			}
			break;
                default:
                break;
		};
		
		/* Are we finished? */
		done = (event.type == YAML_STREAM_END_EVENT);
		
		/* The application is responsible for destroying the event object. */
		yaml_event_delete(&event);
		
	}
	
	yaml_parser_delete(&parser);	
	if (strFile)
	{
		strFile->pathName = pathName;
	}

	SafeDeleteArray(dataHolder->data);
	return strFile;
}
	
void LocalizationSystem::LoadStringFile(const String & fileName)
{
	StringFile * file = LoadFromYamlFile(fileName);
	if (file)
	{
		stringsList.push_back(file);
	}	
}
	
void LocalizationSystem::UnloadStringFile(const String & fileName)
{
	DVASSERT(0 && "Method do not implemented");
}

const WideString & LocalizationSystem::GetLocalizedString(const WideString & key)
{
	//List<StringFile*>::const_reverse_iterator rEnd = stringsList.rend();
	for (List<StringFile*>::reverse_iterator it = stringsList.rbegin(); it != stringsList.rend(); ++it)
	{
		StringFile * file = *it;
		
		Map<WideString, WideString>::iterator res = file->strings.find(key);
		if (res != file->strings.end())
		{
			return res->second;
		}
	}
	return key;
}


};