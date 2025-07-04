#include "Scene.h"

#include "GrappleCore/Profiler/Profiler.h"

#include "Grapple/Renderer/Renderer.h"
#include "Grapple/Renderer2D/Renderer2D.h"
#include "Grapple/DebugRenderer/DebugRenderer.h"

#include "Grapple/Renderer/PostProcessing/ToneMapping.h"
#include "Grapple/Renderer/PostProcessing/Vignette.h"
#include "Grapple/Renderer/PostProcessing/SSAO.h"
#include "Grapple/Renderer/PostProcessing/Atmosphere.h"

#include "Grapple/AssetManager/AssetManager.h"

#include "Grapple/Math/Math.h"
#include "Grapple/Scene/Components.h"
#include "Grapple/Scene/Hierarchy.h"
#include "Grapple/Scene/Transform.h"
#include "Grapple/Input/InputManager.h"

#include "Grapple/Scripting/ScriptingEngine.h"

namespace Grapple
{
	Ref<Scene> s_Active = nullptr;

	Grapple_SERIALIZABLE_IMPL(Scene);
	Grapple_IMPL_ASSET(Scene);

	Scene::Scene(ECSContext& context)
		: Asset(AssetType::Scene), m_World(context)
	{
		m_World.MakeCurrent();
		Initialize();
	}

	Scene::~Scene()
	{
	}

	void Scene::Initialize()
	{
		SystemsManager& systemsManager = m_World.GetSystemsManager();
		systemsManager.CreateGroup("Debug Rendering");

		m_RenderingGroup = systemsManager.CreateGroup("Rendering");
		m_ScriptingUpdateGroup = systemsManager.CreateGroup("Scripting Update");
		m_LateUpdateGroup = systemsManager.CreateGroup("Late Update");
		m_OnRuntimeStartGroup = systemsManager.CreateGroup("On Runtime Start");
		m_OnRuntimeEndGroup = systemsManager.CreateGroup("On Runtime End");

		m_World.GetSystemsManager().SetDefaultSystemsGroup(m_ScriptingUpdateGroup);

		m_OnFrameStart = systemsManager.CreateGroup("On Frame End");
		m_OnFrameEnd = systemsManager.CreateGroup("On Frame End");

		m_CamerasQuery = m_World.NewQuery().All().With<TransformComponent, CameraComponent>().Build();
		m_DirectionalLightQuery = m_World.NewQuery().All().With<TransformComponent, DirectionalLight>().Build();
		m_EnvironmentQuery = m_World.NewQuery().All().With<Environment>().Build();
		m_PointLightsQuery = m_World.NewQuery().All().With<TransformComponent, PointLight>().Build();
		m_SpotLightsQuery = m_World.NewQuery().All().With<TransformComponent, SpotLight>().Build();

		systemsManager.RegisterSystem("Sprite Renderer", new SpriteRendererSystem());
		systemsManager.RegisterSystem("Meshe Renderer", new MeshRendererSystem());
		systemsManager.RegisterSystem("Decal Renderer", new DecalRendererSystem());

		m_PostProcessingManager.AddEffect(CreateRef<SSAO>());
		m_PostProcessingManager.AddEffect(CreateRef<Atmosphere>());
		m_PostProcessingManager.AddEffect(CreateRef<Vignette>());
		m_PostProcessingManager.AddEffect(CreateRef<ToneMapping>());
	}

	void Scene::InitializeRuntime()
	{
		m_World.GetSystemsManager().RegisterSystems();
		m_World.GetSystemsManager().RebuildExecutionGraphs();
	}

	void Scene::OnRuntimeStart()
	{
		m_World.GetSystemsManager().ExecuteGroup(m_OnRuntimeStartGroup);
	}

	void Scene::OnRuntimeEnd()
	{
		m_World.GetSystemsManager().ExecuteGroup(m_OnRuntimeEndGroup);
	}

	void Scene::OnUpdateRuntime()
	{
		m_World.GetSystemsManager().ExecuteGroup(m_OnFrameStart);
		m_World.GetSystemsManager().ExecuteGroup(m_ScriptingUpdateGroup);
		m_World.GetSystemsManager().ExecuteGroup(m_LateUpdateGroup);
		m_World.GetSystemsManager().ExecuteGroup(m_OnFrameEnd);

		// TODO: should probably move out of here, because OnUpdateRuntime is called
		//       only if the game isn't paused, however clearing deleted entites should
		//       be done regardless of the pause state
		m_World.Entities.ClearQueuedForDeletion();
		m_World.Entities.ClearCreatedEntitiesQueryResult();

		UpdateEnvironmentSettings();
	}

	void Scene::OnUpdateEditor()
	{
		m_World.GetSystemsManager().ExecuteSystem<TransformPropagationSystem>();
		m_World.Entities.ClearQueuedForDeletion();
		m_World.Entities.ClearCreatedEntitiesQueryResult();

		UpdateEnvironmentSettings();
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
	}

	World& Scene::GetECSWorld()
	{
		return m_World;
	}

	Ref<Scene> Scene::GetActive()
	{
		return s_Active;
	}

	void Scene::SetActive(const Ref<Scene>& scene)
	{
		s_Active = scene;
	}

	void Scene::UpdateEnvironmentSettings()
	{
		for (EntityView view : m_EnvironmentQuery)
		{
			auto environments = view.View<const Environment>();

			for (EntityViewElement entity : view)
			{
				Renderer::SetShadowSettings(environments[entity].ShadowSettings);
				return;
			}
		}
	}
}