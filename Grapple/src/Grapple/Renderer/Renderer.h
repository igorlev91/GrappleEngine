#pragma once

#include "GrappleCore/Core.h"
#include "Grapple/Renderer/RenderData.h"
#include "Grapple/Renderer/Viewport.h"
#include "Grapple/Renderer/RenderPass.h"

#include "Grapple/Renderer/VertexArray.h"
#include "Grapple/Renderer/Material.h"
#include "Grapple/Renderer/Mesh.h"

#include <glm/glm.hpp>

namespace Grapple
{
	enum class MeshRenderFlags : uint8_t
	{
		None = 0,
		DontCastShadows = 1,
	};

	Grapple_IMPL_ENUM_BITFIELD(MeshRenderFlags);

	struct RendererStatistics
	{
		uint32_t DrawCallsCount;
		uint32_t DrawCallsSavedByInstancing;

		uint32_t ObjectsSubmitted;
		uint32_t ObjectsCulled;
	};

	struct ShadowSettings
	{
		Grapple_TYPE;

		static constexpr uint32_t MaxCascades = 4;

		enum class ShadowResolution
		{
			_512 = 512,
			_1024 = 1024,
			_2048 = 2048,
			_4096 = 4096,
		};

		static std::optional<ShadowResolution> ShadowResolutionFromSize(uint32_t size)
		{
			switch (size)
			{
			case 512:
				return ShadowResolution::_512;
			case 1024:
				return ShadowResolution::_1024;
			case 2048:
				return ShadowResolution::_2048;
			case 4096:
				return ShadowResolution::_4096;
			}

			return {};
		}

		ShadowSettings()
			: Resolution(ShadowResolution::_1024),
			LightSize(0.009f),
			Cascades(MaxCascades),
			Bias(0.001f)
		{
			CascadeSplits[0] = 25.0f;
			CascadeSplits[1] = 50.0f;
			CascadeSplits[2] = 150.0f;
			CascadeSplits[3] = 300.0f;
		}

		float LightSize;
		float Bias;
		ShadowResolution Resolution;
		int32_t Cascades;

		float CascadeSplits[4] = { 0.0f };
	};

	template<>
	struct TypeSerializer<ShadowSettings>
	{
		void OnSerialize(ShadowSettings& settings, SerializationStream& stream)
		{
			stream.Serialize("LightSize", SerializationValue(settings.LightSize));
			stream.Serialize("Bias", SerializationValue(settings.Bias));

			using Underlying = std::underlying_type_t<ShadowSettings::ShadowResolution>;
			stream.Serialize("Resolution", SerializationValue(reinterpret_cast<Underlying&>(settings.Resolution)));

			stream.Serialize("Cascades", SerializationValue(settings.Cascades));

			stream.Serialize("CascadeSplit0", SerializationValue(settings.CascadeSplits[0]));
			stream.Serialize("CascadeSplit1", SerializationValue(settings.CascadeSplits[1]));
			stream.Serialize("CascadeSplit2", SerializationValue(settings.CascadeSplits[2]));
			stream.Serialize("CascadeSplit3", SerializationValue(settings.CascadeSplits[3]));
		}
	};

	class Grapple_API Renderer
	{
	public:
		static void Initialize();
		static void Shutdown();

		static const RendererStatistics& GetStatistics();
		static void ClearStatistics();

		static void SetMainViewport(Viewport& viewport);

		static void BeginScene(Viewport& viewport);
		static void Flush();
		static void EndScene();

		static void DrawFullscreenQuad(const Ref<Material>& material);
		static void DrawMesh(const Ref<VertexArray>& mesh, const Ref<Material>& material, size_t indicesCount = SIZE_MAX);
		static void DrawMesh(const Ref<Mesh>& mesh,
			const Ref<Material>& material,
			const glm::mat4& transform,
			MeshRenderFlags flags = MeshRenderFlags::None,
			int32_t entityIndex = INT32_MAX);

		static void AddRenderPass(Ref<RenderPass> pass);
		static void ExecuteRenderPasses();

		static Viewport& GetMainViewport();
		static Viewport& GetCurrentViewport();

		static Ref<const VertexArray> GetFullscreenQuad();
		static Ref<Texture> GetWhiteTexture();

		static Ref<FrameBuffer> GetShadowsRenderTarget(size_t index);
		static ShadowSettings& GetShadowSettings();
	private:
		static void DrawQueued(bool shadowPass);
		static void FlushInstances();
	};
}