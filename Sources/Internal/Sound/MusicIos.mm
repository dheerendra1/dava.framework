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
 * Created by Ivan Petrochenko
 =====================================================================================*/

#include "MusicIos.h"
#include "FileSystem/FileSystem.h"
#include "Sound/SoundSystem.h"
#include "Sound/SoundInstance.h"
#include "Sound/SoundChannel.h"
#import <AVFoundation/AVFoundation.h>

@interface AvSound : NSObject <AVAudioPlayerDelegate>
{
@public
	AVAudioPlayer		*audioPlayer;
	
	BOOL				playing;
	BOOL				interruptedOnPlayback;
}

@property(nonatomic, readonly) AVAudioPlayer * audioPlayer;

- (void)play;
- (void)stop;
 
@end

@implementation AvSound

@synthesize audioPlayer;

- (id)initWithFileName: (NSString*)name
{  
    if(self == [super init])
	{  
        NSError * error = 0;
        NSURL * url = [NSURL fileURLWithPath: name];
		audioPlayer					= [[AVAudioPlayer alloc] initWithContentsOfURL: url error: &error];
		if(error)
        {
            printf("AvSound::initWithFileName error %s", [[error localizedDescription] cStringUsingEncoding:NSASCIIStringEncoding]);
        }
        
		audioPlayer.delegate = self;
		[audioPlayer prepareToPlay];
	}  
	
    return self;  
}  

- (void)dealloc  
{  
	[audioPlayer release];

    [super dealloc];  
} 

- (void)play
{
	//DAVA::SoundSystemiPhone *soundSystemIPhone = (DAVA::SoundSystemiPhone*)DAVA::SoundSystem::Instance();
	//if (!soundSystemIPhone->CanUseBgm())					return;
	//if (!DAVA::SoundSystem::Instance()->IsMusicOn())		return;
	
	[audioPlayer play];
	playing = YES;
}

- (void)stop
{
	//DAVA::SoundSystemiPhone *soundSystemIPhone = (DAVA::SoundSystemiPhone*)DAVA::SoundSystem::Instance();
	//if (!soundSystemIPhone->CanUseBgm())					return;
	
	playing = NO;
	[audioPlayer stop];
}

- (void)audioPlayerBeginInterruption: (AVAudioPlayer*)player
{
    if (playing)
	{
        playing					= NO;
        interruptedOnPlayback	= YES;
    }
}

- (void)audioPlayerEndInterruption: (AVAudioPlayer*)player
{
    if (interruptedOnPlayback)
	{
        [player prepareToPlay];
        [player play];

        playing					= YES;
        interruptedOnPlayback	= NO; 
    }
}

@end



namespace DAVA
{

MusicIos::MusicIos(const String & fileName)
:   Sound(fileName, Sound::TYPE_STREAMED),  
    avSound(0)
{
    avSound = [[AvSound alloc] initWithFileName:[NSString stringWithCString:FileSystem::Instance()->SystemPathForFrameworkPath(fileName).c_str() 
                encoding:NSASCIIStringEncoding]]; 
}

MusicIos::~MusicIos()
{
    [(AvSound*)avSound release];
    avSound = 0;
}

SoundInstance * MusicIos::Play()
{
    if(soundInstances.size())
	{
		return soundInstances.front();
	}

	SoundInstance * inst = new SoundInstance();
    inst->buddyMusic = this;
	AddSoundInstance(inst);
    [(AvSound*)avSound play];
	return inst;
}

void MusicIos::Stop()
{
    [(AvSound*)avSound stop];
}

void MusicIos::SetVolume(float32 _volume)
{
    volume = Clamp(_volume, 0.f, 1.f);
	((AvSound*)avSound).audioPlayer.volume = volume;
}


void MusicIos::SetLooping(bool looping)
{
    ((AvSound*)avSound).audioPlayer.numberOfLoops = looping ? -1 : 0;
}

bool MusicIos::IsPlaying()
{
    return [((AvSound*)avSound).audioPlayer isPlaying];
}

};
