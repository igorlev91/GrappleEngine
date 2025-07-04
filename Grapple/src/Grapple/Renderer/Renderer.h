#pragma once

#include "GrappleCore/Core.h"
#include "Grapple/Renderer/RenderData.h"
#include "Grapple/Renderer/Viewport.h"
#include "Grapple/Renderer/RendererSubmitionQueue.h"
#include "Grapple/Renderer/RendererStatistics.h"

#include "Grapple/Renderer/Material.h"
#include "Grapple/Renderer/Mesh.h"

#include <glm/glm.hpp>

namespace Grapple
{
	enum class ShadowQuality
	{
		Low = 0,
		Medium = 1,
		High = 2,
	};

	struct ShadowSettings
	{
		Grapple_TYPE;

		static constexpr uint32_t MaxCascades = 4;

		ShadowSettings()
			: Quality(ShadowQuality::High),
			Enabled(true),
			LightSize(2.0f),
			Cascades(MaxCascades),
			Bias(0.1f),
			NormalBias(0.2f),
			Softness(1.0f),
			FadeDistance(10.0f)
		{
			CascadeSplits[0] = 15.0f;
			CascadeSplits[1] = 30.0f;
			CascadeSplits[2] = 70.0f;
			CascadeSplits[3] = 100.0f;
		}

		bool Enabled;

		float LightSize;
		float Bias;
		float NormalBias;
		ShadowQuality Quality;
		int32_t Cascades;

		float Softness;
		float CascadeSplits[MaxCascades] = { 0.0f };
		float FadeDistance;
	};

	inline uint32_t GetShadowMapResolution(ShadowQuality quality)
	{
		switch (quality)
		{
		case ShadowQuality::Low:
			return 512;
		case ShadowQuality::Medium:
			return 1024;
		case ShadowQuality::High:
			return 2048;
		}

		Grapple_CORE_ASSERT(false);
		return 0;
	}

	template<>
	struct TypeSerializer<ShadowSettings>
	{
		void OnSerialize(ShadowSettings& settings, SerializationStream& stream)
		{
			stream.Serialize("Enabled", SerializationValue(settings.Enabled));
			stream.Serialize("LightSize", SerializationValue(settings.LightSize));
			stream.Serialize("Bias", SerializationValue(settings.Bias));
			stream.Serialize("NormalBias", SerializationValue(settings.NormalBias));

			using Underlying = std::underlying_type_t<ShadowQuality>;
			stream.Serialize("Quality", SerializationValue(reinterpret_cast<Underlying&>(settings.Quality)));

			switch (settings.Quality)
			{
			case ShadowQuality::Low:
			case ShadowQuality::Medium:
			case ShadowQuality::High:
				break;
			default:
				settings.Quality = ShadowQuality::Medium;
			}

			stream.Serialize("Cascades", SerializationValue(settings.Cascades));
			stream.Serialize("Softness", SerializationValue(settings.Softness));
			stream.Serialize("FadeDistance", SerializationValue(settings.FadeDistance));

			stream.Serialize("CascadeSplit0", SerializationValue(settings.CascadeSplits[0]));
			stream.Serialize("CascadeSplit1", SerializationValue(settings.CascadeSplits[1]));
			stream.Serialize("CascadeSplit2", SerializationValue(settings.CascadeSplits[2]));
			stream.Serialize("CascadeSplit3", SerializationValue(settings.CascadeSplits[3]));
		}
	};

	struct PointLightData
	{
		PointLightData(glm::vec3 position, glm::vec4 color)
			: Position(position), Color(color) {}

		glm::vec3 Position;
		float Padding0 = 0.0f;
		glm::vec4 Color;
	};

	struct SpotLightData
	{
		SpotLightData(glm::vec3 position, glm::vec3 direction, float innerAngle, float outerAngle, glm::vec4 color)
			: Position(position),
			InnerAngleCos(glm::cos(glm::radians(innerAngle))),
			Direction(glm::normalize(direction)),
			OuterAngleCos(glm::cos(glm::radians(outerAngle))),
			Color(color) {}

		glm::vec3 Position;
		float InnerAngleCos;
		glm::vec3 Direction;
		float OuterAngleCos;
		glm::vec4 Color;
	};

	struct SceneSubmition;
	class DescriptorSet;
	class DescriptorSetLayout;
	class DescriptorSetPool;
	class Grapple_API Renderer
	{
	public:
		static void Initialize();
		static void Shutdown();

		static const RendererStatistics& GetStatistics();
		static void ClearStatistics();

		static void SetMainViewport(Viewport& viewport);
		static void SetCurrentViewport(Viewport& viewport);

		static void BeginFrame();
		static void EndFrame();

		static SceneSubmition& GetCurrentSceneSubmition();
		static void BeginScene(SceneSubmition& sceneSubmition);
		static void EndScene();

		static void BeginScene(Viewport& viewport);
		static void Flush();

		static void SubmitPointLight(const PointLightData& light);
		static void SubmitSpotLight(const SpotLightData& light);

		static void DrawMesh(const Ref<Mesh>& mesh,
			uint32_t subMesh,
			const Ref<Material>& material,
			const glm::mat4& transform,
			MeshRenderFlags flags = MeshRenderFlags::None);

		static void SubmitDecal(const Ref<const Material>& material, const glm::mat4& transform);

		static RendererSubmitionQueue& GetOpaqueSubmitionQueue();

		static Viewport& GetMainViewport();
		static Viewport& GetCurrentViewport();

		static Ref<Texture> GetWhiteTexture();
		static Ref<Texture> GetDefaultNormalMap();
		static Ref<Material> GetErrorMaterial();
		static Ref<Material> GetDepthOnlyMaterial();

		static Ref<DescriptorSetPool> GetGlobalDescriptorSetPool();
		static Ref<DescriptorSetPool> GetCameraDescriptorSetPool();
		static Ref<DescriptorSetPool> GetInstanceDataDescriptorSetPool();

		static const ShadowSettings& GetShadowSettings();
		static void SetShadowSettings(const ShadowSettings& settings);

		static bool RequiresRenderGraphRebuild();

		static Ref<const DescriptorSetLayout> GetDecalsDescriptorSetLayout();

		static void ConfigurePasses(Viewport& viewport);
	private:
		static void ReloadShaders();
	};
}