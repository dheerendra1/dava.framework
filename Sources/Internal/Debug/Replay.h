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
#ifndef __DAVAENGINE_REPLAY_H__
#define __DAVAENGINE_REPLAY_H__

#include "Base/Singleton.h"
#include "Base/BaseTypes.h"
#include "UI/UIEvent.h"
#include "FileSystem/File.h"

namespace DAVA
{

class File;
class Replay : public Singleton<Replay>
{
public:
	static inline bool IsRecord();
	static inline bool IsPlayback();

	Replay();
	virtual ~Replay();

	enum	eValueType
	{
		VALUE_FRAMETIME = 0,
		VALUE_EVENTS_COUNT,
		VALUE_SEED
	};

	void	StartRecord(const String & dirName);
	void	StopRecord();
	void	RecordFrame(float32 frameTime);
	void	RecordEventsCount(int32 eventsCount);
	void	RecordEvent(const UIEvent * ev);
	void	RecordSeed(const uint32 seed);
	void	PauseReplay(bool isPause);

	void	StartPlayback(const String & dirName);
	float32	PlayFrameTime();
	int32	PlayEventsCount();
	UIEvent	PlayEvent();
	void	PlaySeed();
	bool	IsEvent();

	bool	ReplayPaused() { return pauseReplay; }

private:
	static bool isRecord;
	static bool isPlayback;

	File * file;

	template<class T>
	void	Write(T value);

	template<class T>
	T		Read();

	bool	skipType;
	bool	pauseReplay;
};

inline bool Replay::IsRecord()
{
	return (isRecord && !Replay::Instance()->ReplayPaused());
}

inline bool Replay::IsPlayback()
{
	return (isPlayback && !Replay::Instance()->ReplayPaused());
}

template<class T>
void Replay::Write(T value)
{
	file->Write(&value, sizeof(T));
}

template<class T>
T Replay::Read()
{
	T value;
	file->Read(&value, sizeof(T));

	if(file->GetPos() == file->GetSize())
	{
		isPlayback = false;
		Logger::Info("replay ended");
	}

	return value;
}

};

#endif // __DAVAENGINE_REPLAY_H__
