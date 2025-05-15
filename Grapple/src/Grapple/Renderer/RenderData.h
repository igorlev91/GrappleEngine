#pragma once

#include "Grapple/Math/Math.h"
#include "Grapple/Renderer/UniformBuffer.h"

#include "GrappleCore/Core.h"
#include "GrappleCore/Log.h"

#include <glm/glm.hpp>

namespace Grapple
{
	struct FrustumPlanes
	{
		static constexpr size_t PlanesCount = 6;
		static constexpr size_t NearPlaneIndex = 0;
		static constexpr size_t FarPlaneIndex = 1;
		static constexpr size_t LeftPlaneIndex = 2;
		static constexpr size_t RightPlaneIndex = 3;
		static constexpr size_t TopPlaneIndex = 4;
		static constexpr size_t BottomPlaneIndex = 5;

		inline bool ContainsPoint(const glm::vec3& point) const
		{
			for (size_t i = 0; i < 6; i++)
			{
				if (Planes[i].SignedDistance(point) < 0.0f)
					return false;
			}

			return true;
		}

		Math::Plane Planes[PlanesCount];
	};

	struct CameraData
	{
		CameraData()
			: Position(0.0f),
			Unused(0.0f),
			Projection(0.0f),
			View(0.0f),
			ViewProjection(0.0f),
			InverseProjection(0.0f),
			InverseView(0.0f),
			InverseViewProjection(0.0f) {}

		void CalculateViewProjection()
		{
			ViewProjection = Projection * View;
			InverseView = glm::inverse(View);
			InverseViewProjection = glm::inverse(ViewProjection);
			InverseProjection = glm::inverse(Projection);
		}

		glm::vec3 Position;

		float Unused; // Is needed to align the struct to 16 byte boundary

		glm::mat4 Projection;
		glm::mat4 View;
		glm::mat4 ViewProjection;

		glm::mat4 InverseProjection;
		glm::mat4 InverseView;
		glm::mat4 InverseViewProjection;

		float Near;
		float Far;

		glm::ivec2 ViewportSize;

		glm::vec3 ViewDirection;
		float FOV;
	};

	struct LightData
	{
		glm::vec3 Color;
		float Intensity;

		glm::vec3 Direction;
		float Unused;
		
		glm::vec4 EnvironmentLight;

		float Near;
		uint32_t PointLightsCount;
		uint32_t SpotLightsCount;
	};

	struct RenderData
	{
		CameraData Camera;
		FrustumPlanes CameraFrustumPlanes;
		LightData Light;
		Math::Basis LightBasis;
		CameraData LightView[4];
		bool IsEditorCamera = false;
	};
}