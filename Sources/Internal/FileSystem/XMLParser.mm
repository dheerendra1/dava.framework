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
#include "FileSystem//XMLParser.h"
#include "Utils/Utils.h"
#include "FileSystem/Logger.h"
#include "FileSystem/File.h"

#import <Foundation/Foundation.h>


@interface XMLParserObject : NSObject
{
@public
	DAVA::XMLParserDelegate *delegate;
	
	DAVA::Map<DAVA::String, DAVA::String> attributes;

	DAVA::String uri;
	DAVA::String qn;

}


@end


@implementation XMLParserObject

- (void)parser:(NSXMLParser *)parser didStartElement:(NSString *)elementName 
  namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName attributes:(NSDictionary *)attributeDict 
{
	
	NSArray *objs;
	NSArray *keys;
	
	keys = [attributeDict allKeys];
	objs = [attributeDict allValues];
	
	for (unsigned int i = 0; i < [keys count]; i++) 
	{
		DAVA::String o = [[objs objectAtIndex: i] cStringUsingEncoding:NSUTF8StringEncoding];
		DAVA::String k = [[keys objectAtIndex: i] cStringUsingEncoding:NSUTF8StringEncoding];
		attributes[k] = o;
	}
	

	if(namespaceURI)
	{
		uri = [namespaceURI cStringUsingEncoding:NSUTF8StringEncoding];
	}
	else
	{
		uri = "";
	}
	if(qName)
	{
		qn = [qName cStringUsingEncoding:NSUTF8StringEncoding];
	}
	else
	{
		qn = "";
	}
	delegate->OnElementStarted([elementName cStringUsingEncoding:NSUTF8StringEncoding ]
							   , uri
							   , qn, attributes);
	
	attributes.clear();
}

- (void)parser:(NSXMLParser *)parser didEndElement:(NSString *)elementName 
  namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName 
{
	
	if(namespaceURI)
	{
		uri = [namespaceURI cStringUsingEncoding:NSUTF8StringEncoding];
	}
	else
	{
		uri = "";
	}
	if(qName)
	{
		qn = [qName cStringUsingEncoding:NSUTF8StringEncoding];
	}
	else
	{
		qn = "";
	}
	delegate->OnElementEnded([elementName cStringUsingEncoding:NSUTF8StringEncoding ]
							 , uri
							 , qn);
}	

- (void)parser:(NSXMLParser *)parser parseErrorOccurred:(NSError *)parseError
{
	
}

- (void)parser:(NSXMLParser *)parser validationErrorOccurred:(NSError *)validError
{
	
}

- (void)parser:(NSXMLParser *)parser foundCharacters:(NSString *)string
{
	delegate->OnFoundCharacters([string cStringUsingEncoding:NSUTF8StringEncoding]);
}

@end

namespace DAVA 
{
XMLParser::XMLParser()
{
}

XMLParser::~XMLParser()
{
}
	
	bool ParseData(NSData *data, DAVA::XMLParserDelegate *delegate)
	{
		NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
		
		XMLParserObject *parserObject	= [[XMLParserObject alloc] init];
		parserObject->delegate			= delegate;
		NSXMLParser *parser				= [[NSXMLParser alloc] initWithData: data];
		[parser setDelegate: parserObject];
		[parser setShouldResolveExternalEntities: YES];
		BOOL success					= [parser parse];
		if (!success)
		{
			NSLog(@"*** XML Parser Error: %@", [[parser parserError] localizedDescription]);
		}
		
		[parser release];
		[parserObject release];
		
		[pool release];
		
		return success;
	}	

bool XMLParser::ParseBytes(const unsigned char *bytes, int length, XMLParserDelegate *delegate)
{
	NSData *data = [NSData dataWithBytes: (void*)bytes length: length];
	if (!ParseData(data, delegate))
	{
		Logger::Warning("PARSING PROBLEMS!!!");
		return false;
	}
	return true;
}

bool XMLParser::ParseFile(const String &fileName, XMLParserDelegate *delegate)
{
	File *fp = File::Create(fileName, File::READ|File::OPEN);
	if(!fp)
	{
		Logger::Error("Can't open file %s", fileName.c_str());
		return false;
	}
	
	int32 len = fp->GetSize();
	uint8 *bytes = new uint8[len];
	fp->Read(bytes, len);
	SafeRelease(fp);
	bool res = ParseBytes(bytes, len, delegate);
	SafeDeleteArray(bytes);

	if (!res)
	{
		Logger::Warning("%s PARSING PROBLEMS!!!", fileName.c_str());
		return false;
	}
	return true;
}
	
	
};
