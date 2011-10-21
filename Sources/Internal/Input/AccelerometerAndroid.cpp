#include "Input/AccelerometerAndroid.h"

#if defined(__DAVAENGINE_ANDROID__)
#include "Platform/Systemtimer.h"

namespace DAVA 
{

	AccelerometerAndroidImpl::AccelerometerAndroidImpl()
		: Accelerometer()
	{
		updRate = 0.01;
		enabled = false;

		lastUpdate = 0;
	}

	AccelerometerAndroidImpl::~AccelerometerAndroidImpl()
	{

	}

	void AccelerometerAndroidImpl::Enable(float32 updateRate)
	{
		lastUpdate = 0;
		updRate = updateRate;
		enabled = true;
	}

	void AccelerometerAndroidImpl::Disable()
	{
		enabled = false;
	}

	void AccelerometerAndroidImpl::SetAccelerationData(float x, float y, float z)
	{
		if(enabled)
		{
//			Logger::Debug("[AccelerometerAndroidImpl::SetAccelerationData] x=%f; y=%f; z=%f", x, y, z);
			uint64 curTime = SystemTimer::Instance()->GetTickCount();
			float32 delta = (curTime - lastUpdate) / 1000.0f;
			if(updRate < delta)
			{
				lastUpdate = curTime;
				accelerationData.x = x;
				accelerationData.y = y;
				accelerationData.z = z;

				eventDispatcher.PerformEvent(DAVA::Accelerometer::EVENT_ACCELLEROMETER_DATA);
			}
		}
	}
};

#endif //#if defined(__DAVAENGINE_ANDROID__)