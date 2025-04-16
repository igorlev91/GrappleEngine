#include "Time.h"

#include "GrapplePlatform/Platform.h"

namespace Grapple
{
	struct TimeData
	{
		float DeltaTime;
		float PreviousFrameTime;
	};

	TimeData s_TimeData;

	float Time::GetDeltaTime()
	{
		return s_TimeData.DeltaTime;
	}

	void Time::UpdateDeltaTime()
	{
		float currentTime = Platform::GetTime();
		s_TimeData.DeltaTime = currentTime - s_TimeData.PreviousFrameTime;
		s_TimeData.PreviousFrameTime = currentTime;
	}
}
