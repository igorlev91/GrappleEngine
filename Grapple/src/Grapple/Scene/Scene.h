#pragma once

#include "GrappleCore/Core.h"
#include "Grapple/AssetManager/Asset.h"

#include "Grapple/Renderer/RenderData.h"
#include "Grapple/Renderer/Viewport.h"
#include "Grapple/Renderer/Font.h"
#include "Grapple/Renderer/PostProcessing/PostProcessingManager.h"

#include "Grapple/Scene/SceneRenderer.h"

#include "GrappleECS.h"

namespace Grapple
{
	class SceneSerializer;
	class Grapple_API Scene : public Asset
	{
	public:
		Grapple_ASSET;
		Grapple_SERIALIZABLE;

		Scene(ECSContext& context);
		~Scene();

		void Initialize();
		void InitializeRuntime();
	public:
		void OnRuntimeStart();
		void OnRuntimeEnd();

		void OnUpdateRuntime();
		void OnUpdateEditor();
		void OnViewportResize(uint32_t width, uint32_t height);

		World& GetECSWorld();
		inline PostProcessingManager& GetPostProcessingManager() { return m_PostProcessingManager; }
		inline const PostProcessingManager& GetPostProcessingManager() const { return m_PostProcessingManager; }

		static Ref<Scene> GetActive();
		static void SetActive(const Ref<Scene>& scene);
	private:
		void UpdateEnvironmentSettings();
	private:
		World m_World;

		SystemGroupId m_RenderingGroup;
		SystemGroupId m_ScriptingUpdateGroup;
		SystemGroupId m_LateUpdateGroup;
		SystemGroupId m_OnRuntimeStartGroup;
		SystemGroupId m_OnRuntimeEndGroup;

		SystemGroupId m_OnFrameStart;
		SystemGroupId m_OnFrameEnd;

		Query m_CamerasQuery;
		Query m_DirectionalLightQuery;
		Query m_EnvironmentQuery;
		Query m_PointLightsQuery;
		Query m_SpotLightsQuery;

		PostProcessingManager m_PostProcessingManager;
	private:
		friend SceneSerializer;
	};
}