#pragma once

#include "GrappleCore/Core.h"
#include "Grapple/Renderer/RenderData.h"

namespace Grapple
{
	class Grapple_API DebugRenderer
	{
	public:
		static void Initialize(uint32_t maxLinesCount = 40000);
		static void Shutdown();

		static void Begin(const RenderData& renderData);
		static void End();

		static void DrawLine(const glm::vec3& start, const glm::vec3& end);
		static void DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color);
		static void DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& startColor, const glm::vec4& endColor);

		static void DrawRay(const glm::vec3& origin, const glm::vec3& direction, const glm::vec4& color = glm::vec4(1.0f));
	private:
		static void FlushLines();
		static void FlushRays();
	};
}