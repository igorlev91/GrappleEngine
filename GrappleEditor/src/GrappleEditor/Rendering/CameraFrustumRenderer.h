#pragma once

#include "GrappleECS/System/System.h"
#include "GrappleECS/System/SystemInitializer.h"

namespace Grapple
{
	class CameraFrustumRenderer : public System
	{
	public:
		Grapple_SYSTEM;

		virtual void OnConfig(SystemConfig& config) override;
		virtual void OnUpdate(SystemExecutionContext& context) override;
	private:
		Query m_Query;
	};
}