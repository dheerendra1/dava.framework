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
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTR ACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    Revision History:
        * Created by Ivan "Dizz" Petrochenko
=====================================================================================*/
#ifndef __TESTTEMPLATE_H__
#define __TESTTEMPLATE_H__

#include "DAVAEngine.h"
using namespace DAVA;
#include "GameCore.h"

template <class T>
class TestTemplate : public UIScreen
{
public:

	struct PerfFuncData
	{
		void (T::*func)(PerfFuncData * data);
		T				* screen;
		String			name;
		uint64			totalTime;
		uint64			minTime;
		uint64			maxTime;
		uint64			startTime;
		uint64			endTime;
		int32			runCount;
		void			* userData;
	};

	TestTemplate(const String & screenName);

	virtual void Update(float32 timeElapsed);
	virtual void Draw(const UIGeometricData &geometricData);
	virtual void DidAppear();


	void RegisterFunction(T * screen, void (T::*func)(PerfFuncData * data), const String & name, int32 runCount, void * userData);

	void SubmitTime(PerfFuncData * data, uint64 time);
	void WriteLog(PerfFuncData * data);

protected:
	Vector<PerfFuncData> perfFuncs;
	int32 funcIndex;
	int32 runIndex;
	String screenName;

private:
	TestTemplate();
};

template <class T>
void TestTemplate<T>::RegisterFunction(T * screen, void (T::*func)(PerfFuncData * data), const String & name, int32 runCount, void * userData)
{
	PerfFuncData data;
	data.name = name;
	data.func = func;
	data.screen = screen;
	data.runCount = runCount;
	data.userData = userData;
	data.totalTime = 0;
	data.minTime = 0;
	data.maxTime = 0;

	perfFuncs.push_back(data);
}

template <class T>
void TestTemplate<T>::WriteLog(PerfFuncData * data)
{
	File * log = GameCore::Instance()->logFile;
	log->WriteLine(Format("%s", data->name.c_str()));
	log->WriteLine(Format("%lld %lld %lld %lld", data->endTime-data->startTime, data->totalTime, data->minTime, data->maxTime));
}

template <class T>
void TestTemplate<T>::SubmitTime(PerfFuncData * data, uint64 time)
{
	data->totalTime += time;
	if (runIndex == 0)
	{
		data->minTime = time;
		data->maxTime = time;
	}
	if(time < data->minTime)
	{
		data->minTime = time;
	}
	if(time > data->maxTime)
	{
		data->maxTime = time;
	}
}

template <class T>
TestTemplate<T>::TestTemplate(const String & _screenName)
{
	screenName = _screenName;
	funcIndex = -1;
	runIndex = -1;
}

template <class T>
void TestTemplate<T>::DidAppear()
{
	funcIndex = 0;
	runIndex = 0;

	File * log = GameCore::Instance()->logFile;
	log->WriteLine(Format("$%s", screenName.c_str()));
}

template <class T>
void TestTemplate<T>::Update(float32 timeElapsed)
{

}

template <class T>
void TestTemplate<T>::Draw(const UIGeometricData &geometricData)
{
	int32 funcsCount = perfFuncs.size();
	if(funcIndex >= 0 && funcIndex < funcsCount)
	{
		PerfFuncData * data = &(perfFuncs[funcIndex]);
		if(runIndex < data->runCount)
		{
			uint64 startTime = SystemTimer::Instance()->AbsoluteMS();
			if(0 == runIndex)
			{
				data->startTime = startTime;
			}
			(data->screen->*data->func)(data);
			uint64 endTime = SystemTimer::Instance()->AbsoluteMS();
			SubmitTime(data, endTime-startTime);

			runIndex++;
			if(data->runCount == runIndex)
			{
				data->endTime = endTime;
			}
		}
		else
		{
			WriteLog(data);

			Logger::Debug("%s %s", screenName.c_str(), data->name.c_str());

			runIndex = 0;
			funcIndex++;
		}
	}
	else
	{
		Core::Instance()->Quit();
	}
}

#endif // __TESTTEMPLATE_H__
