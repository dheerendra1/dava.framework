#include "FileSystem/XMLParser.h"


#include "libxml/parser.h"

namespace DAVA 
{
	XMLParser::XMLParser()
	{

	}

	XMLParser::~XMLParser()
	{

	}

	bool XMLParser::ParseFile(const String &fileName, XMLParserDelegate *delegateptr)
	{
// 		Logger::Debug("[XMLParser::ParseFile] fileName = %s", fileName.c_str());
// 		Logger::Debug("[XMLParser::ParseFile] delegateptr = %p", delegateptr);

		bool retValue = false;
		File *xmlFile = FileSystem::Instance()->CreateFileForFrameworkPath(fileName, 0);
		if(xmlFile)
		{
			int32 dataSize = xmlFile->GetSize();
//			Logger::Debug("[XMLParser::ParseFile] dataSize = %d", dataSize);

			uint8 *data = new uint8[dataSize];
			if(data)
			{
				int32 readBytes = xmlFile->Read(data, dataSize);
//				Logger::Debug("[XMLParser::ParseFile] readBytes = %d", readBytes);
				if(readBytes == dataSize)
				{
					retValue = this->ParseBytes(data, dataSize, delegateptr);
				}
				else
				{
					Logger::Error("[XMLParser::ParseFile] readBytes != dataSize");
				}

				//TODO: VK: need to delete?
				SafeDeleteArray(data);
			}
			else
			{
				Logger::Error("[XMLParser::ParseFile] can't allocate data");
			}

			//TODO: VK: need to delete?
			SafeRelease(xmlFile);
		}
		else
		{
			Logger::Error("[XMLParser::ParseFile] can't create file");
		}

//		Logger::Debug("[XMLParser::ParseFile] retValue = %d", retValue);
		return retValue;
	}

	bool XMLParser::ParseBytes(const unsigned char *bytes, int length, XMLParserDelegate *delegateptr)
	{
//		Logger::Debug("[XMLParser::ParseBytes] delegateptr = %p", delegateptr);

		bool retValue = false;
		
		xmlSAXHandler saxHandler = {0};
		saxHandler.startDocument = XMLParser::StartDocument;
		saxHandler.endDocument = XMLParser::EndDocument;
		saxHandler.startElement = XMLParser::StartElement;
		saxHandler.endElement = XMLParser::EndElement;
		saxHandler.characters = XMLParser::Characters;

		int32 retCode = xmlSAXUserParseMemory( &saxHandler, (void*)delegateptr, (char *)bytes, length);
//		Logger::Debug("[XMLParser::ParseBytes] retCode = %d", retCode);
		if(0 <= retCode)
		{
			retValue = true;
		}

//		Logger::Debug("[XMLParser::ParseBytes] retValue = %d", retValue);
		return retValue;
	}


	void XMLParser::StartDocument(void *user_data)
	{
//		Logger::Debug("[XMLParser::StartDocument] user_data = %p", user_data);
		XMLParserDelegate *delegateptr = (XMLParserDelegate *)user_data;
		if(delegateptr)
		{

		}
	}
	void XMLParser::EndDocument(void *user_data)
	{
//		Logger::Debug("[XMLParser::EndDocument] user_data = %p", user_data);
		XMLParserDelegate *delegateptr = (XMLParserDelegate *)user_data;
		if(delegateptr)
		{

		}
	}

	void XMLParser::Characters(void *user_data, const xmlChar *ch, int len)
	{
//		Logger::Debug("[XMLParser::Characters] user_data = %p, len = %d", user_data, len);
		XMLParserDelegate *delegateptr = (XMLParserDelegate *)user_data;
		if(delegateptr)
		{
			//char *content = new char[len + 1];
			String s((char *)ch, len);

// 			delegateptr->OnFoundCharacters(content);
			delegateptr->OnFoundCharacters(s);

			//SafeDeleteArray(content);
		}
	}

	void XMLParser::StartElement(void *user_data, const xmlChar *name, const xmlChar **attrs)
	{
//		Logger::Debug("[XMLParser::StartElement] %s, user_data = %p", name, user_data);
		XMLParserDelegate *delegateptr = (XMLParserDelegate *)user_data;
		if(delegateptr)
		{
			Map<String, String> attributes;

			if(attrs)
			{
//				Logger::Debug("[XMLParser::StartElement] attrs in");

				int32 i = 0;
				while(attrs[i])
				{
					char *str = (attrs[i+1]) ? (char *)attrs[i+1] : (char *)"";
					attributes[(char *)attrs[i]] = str;

//					Logger::Debug("[XMLParser::StartElement] %s = %s", attrs[i], str);

					i += 2;
				}

//				Logger::Debug("[XMLParser::StartElement] attrs out");
			}

			delegateptr->OnElementStarted((char *)name, "", "", attributes);
		}
	}

	void XMLParser::EndElement(void *user_data, const xmlChar *name)
	{
//		Logger::Debug("[XMLParser::EndElement] %s", name);
		XMLParserDelegate *delegateptr = (XMLParserDelegate *)user_data;
		if(delegateptr)
		{
			delegateptr->OnElementEnded((char *)name, "", "");
		}
	}

// 	xmlEntityPtr XMLParser::GetEntity(void *user_data, const xmlChar *name)
// 	{
// 		Logger::Debug("[XMLParser::GetEntity] %s", name);
// 		XMLParserDelegate *delegate = (XMLParserDelegate *)user_data;
// 		if(delegate)
// 		{
// 
// 		}
// 	}

};
