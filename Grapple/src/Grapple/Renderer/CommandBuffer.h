#pragma once

#include "GrappleCore/Core.h"
#include "Grapple/Math/Math.h"

#include "Grapple/Renderer/Texture.h"

namespace Grapple
{
	class FrameBuffer;
	class Material;
	class Mesh;
	class GPUTimer;
	class ComputePipeline;
	class ShaderConstantBuffer;

	class CommandBuffer
	{
	public:
		virtual void BeginRenderTarget(const Ref<FrameBuffer> frameBuffer) = 0;
		virtual void EndRenderTarget() = 0;

		virtual void ClearColorAttachment(Ref<FrameBuffer> frameBuffer, uint32_t index, const glm::vec4& clearColor) = 0;

		// Depth is in range [0.0, 1.0]
		// 0.0 - is near plane
		// 1.0 - is far plane
		virtual void ClearDepthAttachment(Ref<FrameBuffer> frameBuffer, float depth) = 0;

		// Depth is in range [0.0, 1.0]
		// 0.0 - is near plane
		// 1.0 - is far plane
		virtual void ClearColor(const Ref<Texture>& texture, const glm::vec4& clearColor) = 0;
		virtual void ClearDepth(const Ref<Texture>& texture, float depth) = 0;

		virtual void ApplyMaterial(const Ref<const Material>& material) = 0;
		virtual void PushConstants(const ShaderConstantBuffer& constantBuffer) = 0;

		virtual void SetViewportAndScisors(Math::Rect viewportRect) = 0;

		virtual void DrawIndexed(const Ref<const Mesh>& mesh,
			uint32_t subMeshIndex,
			uint32_t baseInstance,
			uint32_t instanceCount) = 0;

		virtual void Blit(Ref<FrameBuffer> source, uint32_t sourceAttachment, Ref<FrameBuffer> destination, uint32_t destinationAttachment, TextureFiltering filter) = 0;
		virtual void Blit(Ref<Texture> source, Ref<Texture> destination, TextureFiltering filter) = 0;

		virtual void DispatchCompute(Ref<ComputePipeline> pipeline, const glm::uvec3& groupCount) = 0;

		virtual void StartTimer(Ref<GPUTimer> timer) = 0;
		virtual void StopTimer(Ref<GPUTimer> timer) = 0;
	};
}
