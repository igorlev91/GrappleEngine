#pragma once

#include "GrappleCore/Core.h"
#include "Grapple/Renderer/RenderData.h"

#include "Grapple/Renderer/RenderGraph/RenderGraph.h"

#include <vector>
#include <glm/glm.hpp>

namespace Grapple
{
	class Texture;
	class UniformBuffer;
	class DescriptorSet;
	class FrameBuffer;
	class ShaderStorageBuffer;

	struct ViewportGlobalResources
	{
		Ref<UniformBuffer> CameraBuffer = nullptr;
		Ref<UniformBuffer> LightBuffer = nullptr;
		Ref<ShaderStorageBuffer> PointLightsBuffer = nullptr;
		Ref<ShaderStorageBuffer> SpotLightsBuffer = nullptr;
		Ref<UniformBuffer> ShadowDataBuffer = nullptr;

		Ref<DescriptorSet> CameraDescriptorSet = nullptr; // Set 0
		Ref<DescriptorSet> GlobalDescriptorSet = nullptr; // Set 1
		Ref<DescriptorSet> GlobalDescriptorSetWithoutShadows = nullptr; // Set 1, but with all shadow cascades set to white textures
	};

	class Grapple_API Viewport
	{
	public:
		Viewport();
		~Viewport();

		inline glm::ivec2 GetPosition() const { return m_Position; }
		inline glm::ivec2 GetSize() const { return m_Size; }

		inline float GetAspectRatio() const { return (float)m_Size.x / (float)m_Size.y; }

		void Resize(glm::ivec2 position, glm::ivec2 size);
		void UpdateGlobalDescriptorSets();
	public:
		inline bool IsPostProcessingEnabled() const { return m_PostProcessingEnabled; }
		void SetPostProcessingEnabled(bool enabled);

		inline bool IsShadowMappingEnabled() const { return m_ShadowMappingEnabled; }
		void SetShadowMappingEnabled(bool enabled);

		inline bool IsDebugRenderingEnabled() const { return m_DebugRenderingEnabled; }
		void SetDebugRenderingEnabled(bool enabled);
	public:
		RenderData FrameData;
		Ref<FrameBuffer> RenderTarget = nullptr;

		RenderGraph Graph;

		RenderGraphTextureId ColorTextureId;
		RenderGraphTextureId NormalsTextureId;
		RenderGraphTextureId DepthTextureId;

		Ref<Texture> ColorTexture = nullptr;
		Ref<Texture> NormalsTexture = nullptr;
		Ref<Texture> DepthTexture = nullptr;

		ViewportGlobalResources GlobalResources;
	private:
		void SetupGlobalDescriptorSet(Ref<DescriptorSet> set);
	private:
		bool m_PostProcessingEnabled = true;
		bool m_ShadowMappingEnabled = true;
		bool m_DebugRenderingEnabled = false;
	private:
		glm::ivec2 m_Position = glm::ivec2(0, 0);
		glm::ivec2 m_Size = glm::ivec2(0, 0);
	};
}