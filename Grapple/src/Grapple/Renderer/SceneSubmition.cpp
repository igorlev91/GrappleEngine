#include "SceneSubmition.h"

#include "GrappleCore/Profiler/Profiler.h"

namespace Grapple
{
	void SceneSubmition::Clear()
	{
		Grapple_PROFILE_FUNCTION();

		OpaqueGeometrySubmitions.Clear();
		PointLights.clear();
		SpotLights.clear();
	}
}
