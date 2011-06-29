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

#import "Network/BTNetworkConnection.h"

#include "FileSystem/DynamicMemoryFile.h"
#include "Network/NetworkPacket.h"
#include <sys/types.h>
#include <sys/sysctl.h>


#if 0
#define kGKSessionErrorDomain GKSessionErrorDomain
#else
#define kGKSessionErrorDomain @"com.apple.gamekit.GKSessionErrorDomain"
#endif

@implementation BTNetworkConnection

- (NSString *) platform
{
    size_t size;
    sysctlbyname("hw.machine", NULL, &size, NULL, 0);
    char *machine = (char *)malloc(size);
    sysctlbyname("hw.machine", machine, &size, NULL, 0);
	
    NSString *platform = [NSString stringWithCString:machine encoding: NSASCIIStringEncoding];
    free(machine);
    return platform;
}

- (BOOL)supportsBluetoothNetworking 
{
    NSString *platform = [self platform];
    return !([platform isEqualToString:@"iPod1,1"]);
//    return !([platform isEqualToString:@"iPhone1,1"] || [platform isEqualToString:@"iPod1,1"]);
	//"iPhone1,1" - iPhone 2G bluetooth is supported
}

-(void)startServerWithName:(NSString*)name
{
	btSession = nil;
	if (![self supportsBluetoothNetworking]) 
	{
		delegate->ConnectionTypeUnavailable((DAVA::NetworkConnection*)currentConnection);
		return;
	}
	btSession = [[GKSession alloc] initWithSessionID:nil displayName:name sessionMode:GKSessionModeServer];
	if(!btSession)
	{
		delegate->ConnectionTypeUnavailable((DAVA::NetworkConnection*)currentConnection);
	}
	btSession.delegate = self;
	btSession.available = true;
	btSession.disconnectTimeout = 0;
	[btSession setDataReceiveHandler:self withContext:nil];
	clientConnected = false;
	
	peers = nil;
}

-(void)startClient
{
	btSession = nil;
	if (![self supportsBluetoothNetworking]) 
	{
		delegate->ConnectionTypeUnavailable((DAVA::NetworkConnection*)currentConnection);
		return;
	}
	btSession = [[GKSession alloc] initWithSessionID:nil displayName:nil sessionMode:GKSessionModeClient];
	if(!btSession)
	{
		delegate->ConnectionTypeUnavailable((DAVA::NetworkConnection*)currentConnection);
		return;
	}
	btSession.delegate = self;
	btSession.available = true;
	[btSession setDataReceiveHandler:self withContext:nil];
	
	peers = [NSMutableArray new];
}

-(void)stop
{
	OBJC_SAFE_RELEASE(peers);
	if (btSession) 
	{
		[btSession disconnectFromAllPeers];
		btSession.available = NO;
		[btSession setDataReceiveHandler: nil withContext: nil];
		btSession.delegate = nil;
		[btSession release];
	}
	
}

-(void)session:(GKSession *)session peer:(NSString *)peerID didChangeState:(GKPeerConnectionState)state
{
	switch(state)
	{
		case GKPeerStateAvailable:
		{
			for(NSString * oldPeer in peers)
			{
				if([oldPeer isEqualToString:peerID])
				{
					return;
				}
			}
			[peers addObject:peerID];
			delegate->ServersListDidChanged((DAVA::NetworkConnection*)currentConnection);
		}
			break;
		case GKPeerStateUnavailable:
		{
			[peers removeObject:peerID];
			delegate->ServersListDidChanged((DAVA::NetworkConnection*)currentConnection);
		}
			break;	
		case GKPeerStateConnected:
		{
			delegate->ClientAndServerConnected((DAVA::NetworkConnection*)currentConnection);
		}
			break;
		case GKPeerStateDisconnected:
		{
			delegate->OpponentDisconnected((DAVA::NetworkConnection*)currentConnection);
		}
			break;
	}
}

-(NSArray*)listServers
{
	NSMutableArray * arr = [[NSMutableArray new] autorelease];
	for(NSString * p in peers)
	{
		[arr addObject:[btSession displayNameForPeer:p]];
	}
	return  arr;
}

-(void)connectToServer:(int)index
{
	[btSession connectToPeer:[peers objectAtIndex:index] withTimeout:5.f];
	btSession.available = true;
}

-(void)session:(GKSession *)session connectionWithPeerFailed:(NSString *)peerID withError:(NSError *)error
{
	delegate->ServersListDidChanged((DAVA::NetworkConnection*)currentConnection);
}

- (void)session:(GKSession *)_session didReceiveConnectionRequestFromPeer:(NSString *)peerID
{
	if(clientConnected)
	{
		[btSession denyConnectionFromPeer:peerID];
	}
	else
	{
		clientConnected = true;
		[btSession acceptConnectionFromPeer:peerID error:nil];
		btSession.available = false;
	}
}

-(void)sendPacket:(DAVA::NetworkPacket*)packet
{
	DAVA::DynamicMemoryFile *fl = DAVA::DynamicMemoryFile::Create(DAVA::File::WRITE);
	packet->GetArchive()->Save(fl);
	NSData *dt = [NSData dataWithBytes:fl->GetData() length:fl->GetSize()];
	[btSession sendDataToAllPeers:dt withDataMode:GKSendDataReliable error:nil];
	delegate->SendComplete((DAVA::NetworkConnection*)currentConnection);
	DAVA::SafeRelease(fl);
}

- (void) receiveData:(NSData *)data fromPeer:(NSString *)peer inSession: (GKSession *)session context:(void *)context
{
	DAVA::NetworkPacket * p = new DAVA::NetworkPacket((void *)[data bytes], [data length]);
	delegate->PacketReceived(p, (DAVA::NetworkConnection*)currentConnection);
	SafeRelease(p);
}

//- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
//{
//	GETAPP
//	[appDelegate switchView:appDelegate.mainMenuController.view];
//	[[BaseNetworkSystem instance] stop];
//	[BaseNetworkSystem destroy];
//}

- (void)session:(GKSession *)session didFailWithError:(NSError *)error
{
	if ([[error domain] isEqual:kGKSessionErrorDomain] &&
		([error code] == GKSessionCannotEnableError))
	{
//		AlertView * alert = [AlertView createAlert:NSLocalizedString(@"Error\n Bluetooth not available.", nil)];
//		alert.delegate = self;
//		[alert show];
		delegate->ConnectionTypeUnavailable((DAVA::NetworkConnection*)currentConnection);
	}
	else
	{
		delegate->OpponentDisconnected((DAVA::NetworkConnection*)currentConnection);
	}
}



@end

#endif // #if defined(__DAVAENGINE_IPHONE__)

