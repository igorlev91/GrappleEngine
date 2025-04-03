#pragma once

#include "Grapple/Core/Core.h"
#include "Grapple/AssetManager/Asset.h"

#include "Grapple/Renderer/Shader.h"
#include "Grapple/Renderer/RenderData.h"

#include "GrappleECS.h"

namespace Grapple
{
	class SceneSerializer;
	class Scene : public Asset
	{
	public:
		Scene(bool reigsyerComponents = true);
		~Scene();

		void CopyFrom(const Ref<Scene>& scene);

		void Initialize();
		void InitializeRuntime();
	public:
		void OnBeforeRender(RenderData& renderData);
		void OnRender(const RenderData& renderData);

		void OnUpdateRuntime();
		void OnViewportResize(uint32_t width, uint32_t height);

		inline World& GetECSWorld() { return m_World; }
	private:
		World m_World;
		Ref<Shader> m_QuadShader;

		Query m_CameraDataUpdateQuery;
		Query m_SpritesQuery;
	private:
		friend SceneSerializer;
	};
}