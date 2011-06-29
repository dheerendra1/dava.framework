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
#include "Base/BaseTypes.h"
#include "Platform/SystemTimer.h"
#include "FileSystem/Logger.h"
#include "DAVAConfig.h"
#include "Debug/Replay.h"

namespace DAVA 
{

#if defined(__DAVAENGINE_WIN32__)

#else
	static mach_timebase_info_data_t timebase;
#endif 

float SystemTimer::delta = 0;
uint64 SystemTimer::stampTime = 0;

#ifdef SHOW_FRAME_TIME
static uint64 startTime;
static uint64 curTime;
static int frameCount;
#endif

SystemTimer::SystemTimer()
{
#if defined(__DAVAENGINE_WIN32__)
	bHighTimerSupport = QueryPerformanceFrequency(&liFrequency);
	if (bHighTimerSupport)
	{
		Logger::Debug("[SystemTimer] High frequency timer support enabled\n");
	}

#else
	(void) mach_timebase_info(&timebase);
	t0 = mach_absolute_time();
#endif

#ifdef SHOW_FRAME_TIME
	curTime = startTime = AbsoluteMS();
	frameCount = 0;
#endif
}

void SystemTimer::Start()
{
#if defined(__DAVAENGINE_WIN32__)

	delta = ElapsedSec();

	if(delta < 0.001f)
	{
		delta = 0.001f;
	}
	else if(delta > 0.1f)
	{
		delta = 0.1f;
	}

	if (bHighTimerSupport)
	{
		LARGE_INTEGER liCounter;
		QueryPerformanceCounter(&liCounter);

		tLi = liCounter;
	}else
	{
		t0 = (float32)(GetTickCount() / 1000.0f);
	}

#else
	delta = ElapsedSec();
#ifdef SHOW_FRAME_TIME
	curTime = AbsoluteMS();
	frameCount++;
	if(frameCount > 60)
	{
		Logger::Debug("frame time = %dms", (curTime - startTime) / frameCount);
		startTime = curTime;
		frameCount = 0;
	}
#endif
	if(delta < 0.001f)
	{
		delta = 0.001f;
	}
	else if(delta > 0.1f)
	{
		delta = 0.1f;
	}
	t0 = mach_absolute_time();	
#endif
	
	stampTime = AbsoluteMS();
}

float32 SystemTimer::ElapsedSec()
{
#if defined(__DAVAENGINE_WIN32__)
	if (bHighTimerSupport)
	{
		LARGE_INTEGER liCounter;
		QueryPerformanceCounter(&liCounter);

		return (float32)(((float64)(liCounter.QuadPart - tLi.QuadPart))/(float64)liFrequency.QuadPart);
	}
	else
	{
		float32 currentTime = (float32)(GetTickCount() / 1000.0f);
		Logger::Info("delta %f", currentTime - t0);
		return currentTime - t0;
	}

#else
	uint64_t current = mach_absolute_time();
	uint64_t elapsed = (current - t0) * timebase.numer / timebase.denom;
	int highestBitSet = 31;
	for (int k = 32; k < 64; ++k)
		if ((elapsed >> k) & 1)
		{
			highestBitSet = k;
		}
	uint32_t elapsed32 = (uint32_t)(elapsed >> (highestBitSet - 31));
	
	float mult = 1.0f;
	for (int c = 0; c < highestBitSet - 31; ++c)
	{
		mult *= 2.0f;
	}
	
	float t2 = (float)elapsed32 * mult / 1000000000.0f ;
	//float t = ((float)(mach_absolute_time() - t0)) * ((float)timebase.numer) / ((float)timebase.denom) / 1000000000.0f;
	return t2;
#endif
}

uint64 SystemTimer::AbsoluteMS()
{
#if defined(__DAVAENGINE_WIN32__)
	if (bHighTimerSupport)
	{
		LARGE_INTEGER liCounter;
		QueryPerformanceCounter(&liCounter);
		return (uint64)(((float64)(liCounter.QuadPart))/(float64)liFrequency.QuadPart * 1000.);
	}
	else
	{
		return 0;
	}
#else
	uint64_t numer = timebase.numer;
	uint64_t denom = timebase.denom;

	while(((numer % 10) == 0) && ((denom % 10) == 0))
	{
		numer /= 10;
		denom /= 10;
	}
	uint64_t elapsed = mach_absolute_time();
	elapsed *= numer;
	elapsed /= denom;
	return elapsed / 1000000;
#endif
}

void SystemTimer::SetFrameDelta(float32 _delta)
{
	DVASSERT(Replay::IsPlayback());
	delta = _delta;
}


//float SystemTimer::FrameDelta()
//{
//}

};