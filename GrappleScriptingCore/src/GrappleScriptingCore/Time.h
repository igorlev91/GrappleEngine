#pragma once

namespace Grapple::Scripting
{
	struct TimeData
	{
		float DeltaTime;

		static TimeData Data;
	};

	class Time
	{
	public:
		static constexpr float GetDeltaTime() { return TimeData::Data.DeltaTime; }
	};
}