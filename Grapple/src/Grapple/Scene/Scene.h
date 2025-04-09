#pragma once

#include "Grapple/Core/Core.h"
#include "Grapple/AssetManager/Asset.h"

#include "Grapple/Renderer/Shader.h"
#include "Grapple/Renderer/RenderData.h"

#include "Grapple/Scene/SceneRenderer.h"

#include "GrappleECS.h"

namespace Grapple
{
	class SceneSerializer;
	class Scene : public Asset
	{
	public:
		Scene(bool registerComponents = true);
		~Scene();

		void CopyFrom(const Ref<Scene>& scene);

		void Initialize();
		void InitializeRuntime();
	public:
		void OnRuntimeStart();
		void OnRuntimeEnd();

		void OnBeforeRender(RenderData& renderData);
		void OnRender(const RenderData& renderData);

		void OnUpdateRuntime();
		void OnViewportResize(uint32_t width, uint32_t height);

		inline World& GetECSWorld() { return m_World; }

		inline static Ref<Scene> GetActive() { return s_Active; }
		inline static void SetActive(const Ref<Scene>& scene) { s_Active = scene; }
	private:
		World m_World;
		Ref<Shader> m_QuadShader;

		SystemGroupId m_2DRenderingGroup;
		SystemGroupId m_ScriptingUpdateGroup;
		SystemGroupId m_OnRuntimeStartGroup;
		SystemGroupId m_OnRuntimeEndGroup;

		Query m_CameraDataUpdateQuery;
	private:
		static Ref<Scene> s_Active;
		friend SceneSerializer;
	};
}