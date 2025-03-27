#pragma once

#include "Grapple/Core/Core.h"

#include "GrappleECS/World.h"

#include "Grapple/Renderer/Shader.h"
#include "Grapple/Renderer/CameraData.h"

namespace Grapple
{
	class SceneSerializer;
	class Scene
	{
	public:
		Scene();
		~Scene();
	public:
		void OnUpdateRuntime();
		void OnViewportResize(uint32_t width, uint32_t height);

		inline World& GetECSWorld() { return m_World; }
	private:
		uint32_t m_ViewportWidth = 0;
		uint32_t m_ViewportHeight = 0;

		World m_World;
		Ref<Shader> m_QuadShader;

		Query m_CameraDataUpdateQuery;
		CameraData m_CameraData;
	private:
		static Ref<Scene> m_Active;

		friend SceneSerializer;
	};
}