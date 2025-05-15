#pragma once

#include "GrappleCore/Core.h"
#include "Grapple/Renderer/RenderData.h"
#include "Grapple/Math/Math.h"

namespace Grapple
{
	class Grapple_API DebugRenderer
	{
	public:
		static void Initialize(uint32_t maxLinesCount = 40000);
		static void Shutdown();

		static void Begin();
		static void End();

		static void DrawLine(const glm::vec3& start, const glm::vec3& end);
		static void DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color);
		static void DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& startColor, const glm::vec4& endColor);

		static void DrawRay(const glm::vec3& origin, const glm::vec3& direction, const glm::vec4& color = glm::vec4(1.0f));
		static void DrawWireQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color = glm::vec4(1.0f));
		static void DrawWireQuad(const glm::vec3* corners, const glm::vec4& color = glm::vec4(1.0f));
		static void DrawFrustum(const glm::mat4& inverseViewProjection, const glm::vec4& color = glm::vec4(1.0f));
		static void DrawWireBox(const glm::vec3 corners[8], const glm::vec4& color = glm::vec4(1.0f));
		static void DrawAABB(const Math::AABB& aabb, const glm::vec4& color = glm::vec4(1.0f));
	private:
		static void FlushLines();
		static void FlushRays();
	};
}