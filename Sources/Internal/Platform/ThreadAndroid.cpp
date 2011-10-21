#include "Platform/Thread.h"

namespace DAVA
{

#if defined(__DAVAENGINE_ANDROID__)

#include <pthread.h>

void * PthreadMain (void * param)
{	
	Logger::Info("[PthreadMain] param = %p", param);
	
	Thread * t = (Thread*)param;

	t->state = Thread::STATE_RUNNING;
	t->msg(t);

	t->state = Thread::STATE_ENDED;
	t->Release();

	pthread_exit(0);
}

void Thread::StartAndroid()
{
	isMainThread = false;

	pthread_t threadId;
	pthread_create(&threadId, 0, PthreadMain, (void*)this);
}

bool Thread::IsMainThread()
{
//	return isMainThread;
	return true;
}

void Thread::MainThread()
{
	isMainThread = true;
}

pthread_mutex_t fakeMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t fakeCond = PTHREAD_COND_INITIALIZER;
void Thread::SleepThread(int32 timems)
{
//	Logger::Info("[Thread::SleepThread] timems = %d", timems);

	struct timespec timeToWait;
	struct timeval now;
	int rt;

	gettimeofday(&now,NULL);

	timeToWait.tv_sec = now.tv_sec;
	timeToWait.tv_nsec = now.tv_usec*1000 + timems*1000000;

//	Logger::Info("[Thread::SleepThread] sec = %ld, nsec = %ld", timeToWait.tv_sec, timeToWait.tv_nsec);


	pthread_mutex_lock(&fakeMutex);
	rt = pthread_cond_timedwait(&fakeCond, &fakeMutex, &timeToWait);
	pthread_mutex_unlock(&fakeMutex);

//	Logger::Info("[Thread::SleepThread] done");
}


#endif //#if defined(__DAVAENGINE_ANDROID__)

};
