#pragma once

#include "GrappleCore/Core.h"
#include "Grapple/AssetManager/Asset.h"

#include "Grapple/Renderer/Shader.h"
#include "Grapple/Renderer/RenderData.h"

#include "Grapple/Scene/SceneRenderer.h"

#include "GrappleECS.h"

namespace Grapple
{
	class SceneSerializer;
	class Grapple_API Scene : public Asset
	{
	public:
		Scene(ECSContext& context);
		~Scene();

		void Initialize();
		void InitializeRuntime();
	public:
		void OnRuntimeStart();
		void OnRuntimeEnd();

		void OnBeforeRender(RenderData& renderData);
		void OnRender(const RenderData& renderData);

		void OnUpdateRuntime();
		void OnViewportResize(uint32_t width, uint32_t height);

		World& GetECSWorld();

		static Ref<Scene> GetActive();
		static void SetActive(const Ref<Scene>& scene);
	private:
		World m_World;
		Ref<Shader> m_QuadShader;

		SystemGroupId m_2DRenderingGroup;
		SystemGroupId m_ScriptingUpdateGroup;
		SystemGroupId m_OnRuntimeStartGroup;
		SystemGroupId m_OnRuntimeEndGroup;

		Query m_CameraDataUpdateQuery;
	private:
		friend SceneSerializer;
	};
}