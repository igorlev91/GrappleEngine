#pragma once

#include "GrappleECS/World.h"
#include "GrappleECS/System/System.h"
#include "GrappleECS/System/SystemInitializer.h"

namespace Grapple
{
	class CameraFrustumRenderer : public System
	{
	public:
		Grapple_SYSTEM;

		virtual void OnConfig(World& world, SystemConfig& config) override;
		virtual void OnUpdate(World& world, SystemExecutionContext& context) override;
	private:
		Query m_Query;
	};
}