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
        * Created by Alexey 'Hottych' Prosin
=====================================================================================*/

#include "BaseTypes.h"
#if defined(__DAVAENGINE_IPHONE__)


#import "Network/WFNetworkConnection.h"
#import <UIKit/UIDevice.h>
//#import "NetworkGameManager.h"

#include "FileSystem/DynamicMemoryFile.h"
#include "FileSystem/File.h"

@implementation WFNetworkConnection

#define GAME_ID @"dava"
#define DOMAIN_NAME @"local"


-(void)startServerWithName:(NSString*)name
{
	sendBufferSize = 0;
	queueEndPosition = 0;
	inReady = false;
	outReady = false;
	
	server = [TCPServer new];
	[server setDelegate:self];
	NSError* error;
	if(!server|| ![server start:&error]) 
	{
		delegate->ConnectionTypeUnavailable((DAVA::NetworkConnection*)currentConnection);
		NSLog(@"WFNetworkSystem Failed creating server: %@", error);
		return;
	}
	
	UIDevice *myDevice = [UIDevice currentDevice];
	NSString *deviceUDID = [myDevice uniqueIdentifier];
	NSString *finalString = [NSString stringWithFormat:@"%@|eos|%@", name, deviceUDID ];
	
	if(![server enableBonjourWithDomain:DOMAIN_NAME applicationProtocol:[TCPServer bonjourTypeFromIdentifier:GAME_ID] name:finalString]) 
	{
		delegate->ConnectionTypeUnavailable((DAVA::NetworkConnection*)currentConnection);
		NSLog(@"WFNetworkSystem Failed advertising server");
		return;
	}
}

-(void)startClient
{
	sendBufferSize = 0;
	queueEndPosition = 0;
	inReady = false;
	outReady = false;
	
	services = [NSMutableArray new];
	
	serviceBrowser = [NSNetServiceBrowser new];
	if(!serviceBrowser) 
	{
		delegate->ConnectionTypeUnavailable((DAVA::NetworkConnection*)currentConnection);
		NSLog(@"WFNetworkSystem NSNetServiceBrowser creation failure");
		return;
	}
	
	serviceBrowser.delegate = self;
	[serviceBrowser searchForServicesOfType:[TCPServer bonjourTypeFromIdentifier:GAME_ID] inDomain:DOMAIN_NAME];
}

- (void)netServiceBrowser:(NSNetServiceBrowser*)netServiceBrowser didFindService:(NSNetService*)service moreComing:(BOOL)moreComing 
{
	[services addObject:service];
	if(!moreComing)
	{
		delegate->ServersListDidChanged((DAVA::NetworkConnection*)currentConnection);
	}
	NSLog(@"found %@", service.type);
}

- (void)netServiceBrowser:(NSNetServiceBrowser*)netServiceBrowser didRemoveService:(NSNetService*)service moreComing:(BOOL)moreComing
{
	NSLog(@"removing service %@", service.type);
	[services removeObject:service];
	if(!moreComing)
	{
		delegate->ServersListDidChanged((DAVA::NetworkConnection*)currentConnection);
	}
}

- (void)netService:(NSNetService *)sender didNotResolve:(NSDictionary *)errorDict 
{
	delegate->ServersListDidChanged((DAVA::NetworkConnection*)currentConnection);
}

-(void)openStreams
{
	inStream.delegate = self;
	[inStream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
	[inStream open];
	outStream.delegate = self;
	[outStream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
	[outStream open];
}

- (void)netServiceDidResolveAddress:(NSNetService *)service 
{
	[service getInputStream:&inStream outputStream:&outStream];
	[self openStreams];
}

-(NSArray*)listServers
{
	NSMutableArray * arr = [[NSMutableArray new] autorelease];
	for(NSNetService * s in services)
	{
		NSRange r = [s.name rangeOfString:@"|eos|"];
		[arr addObject: [s.name substringWithRange:NSMakeRange(0, r.location)]];
	}
	return  arr;
}

-(void)connectToServer:(int)index
{
	inReady = outReady = false;
	if((index+1) > (int)[[self listServers] count])
	{
		delegate->ServersListDidChanged((DAVA::NetworkConnection*)currentConnection);
		return;
	}
	NSNetService * s = [services objectAtIndex:index];
	s.delegate = self;
	[s resolveWithTimeout:5.0f];
}

- (void) didAcceptConnectionForServer:(TCPServer*)_server inputStream:(NSInputStream *)istr outputStream:(NSOutputStream *)ostr
{
	inReady = outReady = false;
	
	OBJC_SAFE_RELEASE(server);
	
	inStream = istr;
	[inStream retain];
	outStream = ostr;
	[outStream retain];
	
	[self openStreams];
}

-(void)stop
{
	OBJC_SAFE_RELEASE(services);
	OBJC_SAFE_RELEASE(serviceBrowser);
	
	[inStream removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
	OBJC_SAFE_RELEASE(inStream);
	
	[outStream removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
	OBJC_SAFE_RELEASE(outStream);
	
	OBJC_SAFE_RELEASE(server);
}

- (void) writeToBuffer: (uint8_t *)data size: (int)size
{
	
	if (sendBufferSize + size >= SENDBUFFER_SIZE)
	{
		// buffer overflow skip packet	
		NSLog(@"Send buffer overflow - skip packet");
		return; 
	}
	memcpy(&sendBuffer[sendBufferSize], data, size);
	sendBufferSize += size;
	
	[self tryToWriteSendBuffer];
}

- (void) tryToWriteSendBuffer
{
	if (![outStream hasSpaceAvailable])return;
	
	if (sendBufferSize > 0)
	{
		int size = [outStream write:sendBuffer maxLength:sendBufferSize];
		if (size == sendBufferSize)
		{
			sendBufferSize = 0;
		}else if (size > 0)
		{
			memmove(sendBuffer, &sendBuffer[size], sendBufferSize - size);
			sendBufferSize -= size;
			NSLog(@"Send only %d bytes", size);
		}else if (size == -1)
		{
			NSLog(@"Error sending to stream bufferSize: %d", sendBufferSize);
		}
		if(!sendBufferSize)
		{
			delegate->SendComplete((DAVA::NetworkConnection*)currentConnection);
		}
	}
}

- (void) parseInputStream
{
	while(queueEndPosition != 0)
	{
		if (queueEndPosition < (int)sizeof(int))break;
		
		int size = *(int *)&queue[0];
		//NSLog(@"net size = %d", *size);
		if (size <= queueEndPosition)
		{
			//NSLog(@"will parse");
			//NSLog(@"<<<<<<<  size %d received %@", size, [data description]);
			DAVA::NetworkPacket * p = new DAVA::NetworkPacket(queue+sizeof(int), size-sizeof(int));
			delegate->PacketReceived(p, (DAVA::NetworkConnection*)currentConnection);
			DAVA::SafeRelease(p);
			
			memmove(queue, &queue[size], queueEndPosition - size);
			queueEndPosition -= size;
			if (queueEndPosition < 0)
			{
				NSLog(@"WFNetworkSystem Queue parsing error");
				delegate->NetworkError((DAVA::NetworkConnection*)currentConnection);
			}
		}
		else
		{
//			NSLog(@"will NOT parse");
			break;
		}
	}
}

-(void)sendPacket:(DAVA::NetworkPacket*)packet
{

	DAVA::DynamicMemoryFile *fl = DAVA::DynamicMemoryFile::Create(DAVA::File::WRITE);
	packet->GetArchive()->Save(fl);
	int len = fl->GetSize()+sizeof(int);
	
	//NSLog(@"********  size %d sent %@", len, [data description]);
	
	uint8_t * tempBuf = new uint8_t[len];
	*(int*)tempBuf = len;
	memcpy(tempBuf+sizeof(int), fl->GetData(), fl->GetSize());
	DAVA::SafeRelease(fl);
	[self writeToBuffer:tempBuf size:len];
	delete[] tempBuf;
}

- (void) stream:(NSStream*)stream handleEvent:(NSStreamEvent)eventCode
{
	switch(eventCode) 
	{
		case NSStreamEventOpenCompleted:
		{
			OBJC_SAFE_RELEASE(server);
			
			if (stream == inStream)
				inReady = true;
			else
				outReady = true;
			
			if (inReady && outReady) 
			{
				queueEndPosition = 0;
				delegate->ClientAndServerConnected((DAVA::NetworkConnection*)currentConnection);
			}
			
			break;
		}
		case NSStreamEventHasSpaceAvailable:
		{
			if (stream == outStream)
			{
				[self tryToWriteSendBuffer];
			}
			break;
		}
		case NSStreamEventHasBytesAvailable:
		{
			if (stream == inStream) 
			{
				const int TEMP_BUFFER_LEN = 256;
				uint8_t tempBuffer[TEMP_BUFFER_LEN + 1]; 
				unsigned int len = 0;
				len = [inStream read:tempBuffer maxLength:TEMP_BUFFER_LEN];
				if(!len) 
				{
					if ([stream streamStatus] != NSStreamStatusAtEnd)
						NSLog(@"WFNetworkSystem Failed reading data from peer");
						//[self _showAlert:@"Failed reading data from peer"];
				}else
				{
					if (queueEndPosition + len < (int)QUEUE_SIZE)
					{
						memcpy(&queue[queueEndPosition], tempBuffer, len);
						queueEndPosition += len;
					}
					else
					{
						NSLog(@"WFNetworkSystem Queue overflow!");
					}
					[self parseInputStream];
				}
			}
			break;
		}
		case NSStreamEventErrorOccurred:
		{
			NSLog(@"WFNetworkSystem NSStreamEventErrorOccurred");
			delegate->NetworkError((DAVA::NetworkConnection*)currentConnection);
			break;
		}
		case NSStreamEventEndEncountered:
		{
			NSLog(@"WFNetworkSystem opponent disconnected");
			delegate->OpponentDisconnected((DAVA::NetworkConnection*)currentConnection);
			break;
		}
	}
}

@end
#endif // #if defined(__DAVAENGINE_IPHONE__)
