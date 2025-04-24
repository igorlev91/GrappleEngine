#pragma once

#include "GrappleECS/System/System.h"
#include "GrappleECS/System/SystemInitializer.h"

namespace Grapple
{
	class LightVisualizer : public System
	{
	public:
		Grapple_SYSTEM;

		void OnConfig(SystemConfig& config) override;
		void OnUpdate(SystemExecutionContext& context) override;
	private:
		Query m_Query;
	};
}