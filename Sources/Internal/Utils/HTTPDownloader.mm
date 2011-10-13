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

#include "Utils/HTTPDownloader.h"
#include "Utils/Utils.h"
#include "FileSystem/FileSystem.h"


@interface DownloaderObj : NSObject
{
	NSURLConnection					*connection;
	NSMutableData					*receivedData;
	
	DAVA::HTTPDownloaderDelegate	*delegate;
}

- (void)downloadFileFromURLAddress: (NSString*)address delegate: (DAVA::HTTPDownloaderDelegate*)_delegate;

@end

@implementation DownloaderObj

#pragma mark memory management

- (void)dealloc
{
    [connection release];
    [receivedData release];
	
    [super dealloc];
}

#pragma mark public methods

- (void)downloadFileFromURLAddress: (NSString*)address delegate: (DAVA::HTTPDownloaderDelegate*)_delegate
{
	delegate					= _delegate;
	
	NSURL			*url		= [NSURL URLWithString: [address stringByAddingPercentEscapesUsingEncoding: NSUTF8StringEncoding]];
	NSURLRequest	*request	= [NSURLRequest requestWithURL: url cachePolicy: NSURLRequestUseProtocolCachePolicy timeoutInterval: 5.0];
	
	receivedData				= [NSMutableData new];
	connection					= [[NSURLConnection alloc] initWithRequest: request delegate: self startImmediately: YES];
}

#pragma mark NSURLConnection delegate methods

- (void)connection: (NSURLConnection*)theConnection didFailWithError: (NSError*)error
{
	DAVA::String errorMessage = [[error localizedDescription] UTF8String];
	delegate->HTTPDownloaderDelegateDidFailWithErrorMessage(errorMessage);
}

- (void)connection: (NSURLConnection*)theConnection didReceiveData: (NSData*)data
{
    [receivedData appendData: data];
}

- (void)connectionDidFinishLoading: (NSURLConnection*)theConnection
{
	delegate->HTTPDownloaderDelegateDidFinish((const unsigned char*)[receivedData bytes], receivedData.length);
}

@end

//Downloader *downloader;

namespace DAVA 
{
	
HTTPDownloader::HTTPDownloader()
{
	downloader = [DownloaderObj new];
}

HTTPDownloader::~HTTPDownloader()
{
	[(DownloaderObj*)downloader release];
}

void HTTPDownloader::DownloadFile(const String &address, HTTPDownloaderDelegate *delegate)
{
	[(DownloaderObj*)downloader downloadFileFromURLAddress: [NSString stringWithCString: address.c_str() encoding:NSASCIIStringEncoding] delegate: delegate];
}

bool DownloadFileFromURLToDocuments(const String & url, const String & documentsPathname)
{
	NSString * localizedPath = [NSString stringWithUTF8String:url.c_str()];
//	NSError *error = nil;
//TODO: На маке эта хрень возвращает страницу с информацией о том, что такой файл не найден. Никаких ошибок при этом не генерится. Нужно найти решение.
    NSData * fileContents = [NSData dataWithContentsOfURL:[NSURL URLWithString:localizedPath]];
    if (fileContents) 
    {
        [fileContents writeToFile:[NSString stringWithCString: FileSystem::Instance()->SystemPathForFrameworkPath(documentsPathname).c_str() encoding:NSUTF8StringEncoding] atomically:true];
        return true;
    }
    return false;
}
};
