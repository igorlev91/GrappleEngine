#include "Scene.h"

#include "GrappleCore/Profiler/Profiler.h"

#include "Grapple/Renderer/Renderer.h"
#include "Grapple/Renderer2D/Renderer2D.h"
#include "Grapple/Renderer/DebugRenderer.h"

#include "Grapple/AssetManager/AssetManager.h"

#include "Grapple/Math/Math.h"
#include "Grapple/Scene/Components.h"
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
		m_PostProcessingManager.ToneMappingPass = CreateRef<ToneMapping>();
		m_PostProcessingManager.VignettePass = CreateRef<Vignette>();
		m_PostProcessingManager.SSAOPass = CreateRef<SSAO>();
		m_PostProcessingManager.Atmosphere = CreateRef<AtmospherePass>();

		m_World.MakeCurrent();
		Initialize();
	}

	Scene::~Scene()
	{
	}

	void Scene::Initialize()
	{
		ScriptingEngine::SetCurrentECSWorld(m_World);
		m_World.Components.RegisterComponents();

		SystemsManager& systemsManager = m_World.GetSystemsManager();
		systemsManager.CreateGroup("Debug Rendering");

		m_2DRenderingGroup = systemsManager.CreateGroup("2D Rendering");
		m_ScriptingUpdateGroup = systemsManager.CreateGroup("Scripting Update");
		m_OnRuntimeStartGroup = systemsManager.CreateGroup("On Runtime Start");
		m_OnRuntimeEndGroup = systemsManager.CreateGroup("On Runtime End");

		m_OnFrameStart = systemsManager.CreateGroup("On Frame End");
		m_OnFrameEnd = systemsManager.CreateGroup("On Frame End");

		m_CameraDataUpdateQuery = m_World.NewQuery().With<TransformComponent, CameraComponent>().Create();
		m_DirectionalLightQuery = m_World.NewQuery().With<TransformComponent, DirectionalLight>().Create();
		m_EnvironmentQuery = m_World.NewQuery().With<Environment>().Create();
		m_PointLightsQuery = m_World.NewQuery().With<TransformComponent>().With<PointLight>().Create();
		m_SpotLightsQuery = m_World.NewQuery().With<TransformComponent>().With<SpotLight>().Create();
		
		systemsManager.RegisterSystem("Sprites Renderer", m_2DRenderingGroup, new SpritesRendererSystem());
		systemsManager.RegisterSystem("Meshes Renderer", m_2DRenderingGroup, new MeshesRendererSystem());
	}

	void Scene::InitializeRuntime()
	{
		ScriptingEngine::RegisterSystems();
		m_World.GetSystemsManager().RebuildExecutionGraphs();
	}

	void Scene::InitializePostProcessing()
	{
		Renderer::AddRenderPass(m_PostProcessingManager.ToneMappingPass);
		Renderer::AddRenderPass(m_PostProcessingManager.VignettePass);
		Renderer::AddRenderPass(m_PostProcessingManager.SSAOPass);
		Renderer::AddRenderPass(m_PostProcessingManager.Atmosphere);
	}

	void Scene::UninitializePostProcessing()
	{
		Renderer::RemoveRenderPass(m_PostProcessingManager.ToneMappingPass);
		Renderer::RemoveRenderPass(m_PostProcessingManager.VignettePass);
		Renderer::RemoveRenderPass(m_PostProcessingManager.SSAOPass);
		Renderer::RemoveRenderPass(m_PostProcessingManager.Atmosphere);
	}

	void Scene::OnRuntimeStart()
	{
		m_World.GetSystemsManager().ExecuteGroup(m_OnRuntimeStartGroup);
	}

	void Scene::OnRuntimeEnd()
	{
		m_World.GetSystemsManager().ExecuteGroup(m_OnRuntimeEndGroup);
	}

	void Scene::OnBeforeRender(Viewport& viewport)
	{
		Grapple_PROFILE_FUNCTION();

		if (!viewport.FrameData.IsEditorCamera)
		{
			float aspectRation = viewport.GetAspectRatio();

			for (EntityView entityView : m_CameraDataUpdateQuery)
			{
				ComponentView<CameraComponent> cameras = entityView.View<CameraComponent>();
				ComponentView<TransformComponent> transforms = entityView.View<TransformComponent>();

				for (EntityViewElement entity : entityView)
				{
					CameraComponent& camera = cameras[entity];
					TransformComponent& transform = transforms[entity];

					float halfSize = camera.Size / 2;

					viewport.FrameData.Camera.ViewDirection = transform.TransformDirection(glm::vec3(0.0f, 0.0f, -1.0f));

					viewport.FrameData.Camera.Near = camera.Near;
					viewport.FrameData.Camera.Far = camera.Far;
					viewport.FrameData.Camera.View = glm::inverse(transforms[entity].GetTransformationMatrix());

					viewport.FrameData.Camera.FOV = cameras[entity].FOV;

					if (camera.Projection == CameraComponent::ProjectionType::Orthographic)
						viewport.FrameData.Camera.Projection = glm::ortho(-halfSize * aspectRation, halfSize * aspectRation, -halfSize, halfSize, camera.Near, camera.Far);
					else
						viewport.FrameData.Camera.Projection = glm::perspective<float>(glm::radians(camera.FOV), aspectRation, camera.Near, camera.Far);

					viewport.FrameData.Camera.Position = transforms[entity].Position;
					viewport.FrameData.Camera.CalculateViewProjection();
				}
			}
		}

		bool hasLight = false;
		for (EntityView entityView : m_DirectionalLightQuery)
		{
			auto transforms = entityView.View<TransformComponent>();
			auto lights = entityView.View<DirectionalLight>();

			for (EntityViewElement entity : entityView)
			{
				TransformComponent& transform = transforms[entity];
				glm::vec3 direction = transform.TransformDirection(glm::vec3(0.0f, 0.0f, -1.0f));
				glm::vec3 right = transform.TransformDirection(glm::vec3(1.0f, 0.0f, 0.0f));

				viewport.FrameData.LightBasis.Right = right;
				viewport.FrameData.LightBasis.Forward = direction;
				viewport.FrameData.LightBasis.Up = glm::cross(right, direction);

				LightData& light = viewport.FrameData.Light;
				light.Color = lights[entity].Color;
				light.Intensity = lights[entity].Intensity;
				light.Direction = direction;
				light.Near = 0.1f;

				for (size_t i = 0; i < 4; i++)
				{
					CameraData& lightView = viewport.FrameData.LightView[i];
					lightView.Position = transform.Position;
				}

				hasLight = true;
				break;
			}

			if (hasLight)
				break;
		}

		bool hasEnviroment = false;
		for (EntityView view : m_EnvironmentQuery)
		{
			auto environments = view.View<const Environment>();

			for (EntityViewElement entity : view)
			{
				viewport.FrameData.Light.EnvironmentLight = glm::vec4(
					environments[entity].EnvironmentColor,
					environments[entity].EnvironmentColorIntensity
				);

				Renderer::GetShadowSettings() = environments[entity].ShadowSettings;

				hasEnviroment = true;
				break;
			}

			if (hasEnviroment)
				break;
		}

		for (EntityView view : m_PointLightsQuery)
		{
			auto transforms = view.View<const TransformComponent>();
			auto lights = view.View<const PointLight>();

			for (EntityViewElement entity : view)
			{
				Renderer::SubmitPointLight(PointLightData{
					transforms[entity].Position,
					glm::vec4(lights[entity].Color, lights[entity].Intensity)
				});
			}
		}

		for (EntityView view : m_SpotLightsQuery)
		{
			auto transforms = view.View<const TransformComponent>();
			auto lights = view.View<const SpotLight>();

			for (EntityViewElement entity : view)
			{
				if (lights[entity].OuterAngle - lights[entity].InnerAngle <= 0.0f)
					continue;

				Renderer::SubmitSpotLight(SpotLightData(
					transforms[entity].Position,
					transforms[entity].TransformDirection(glm::vec3(0.0f, 0.0f, -1.0f)),
					lights[entity].InnerAngle,
					lights[entity].OuterAngle,
					glm::vec4(lights[entity].Color, lights[entity].Intensity)
				));
			}
		}
	}

	void Scene::OnRender(const Viewport& viewport)
	{
		Grapple_PROFILE_FUNCTION();

		Renderer2D::Begin();

		m_World.GetSystemsManager().ExecuteGroup(m_2DRenderingGroup);

		Renderer2D::End();

		Renderer::Flush();
		Renderer::ExecuteRenderPasses();
	}

	void Scene::OnUpdateRuntime()
	{
		m_World.GetSystemsManager().ExecuteGroup(m_OnFrameStart);
		m_World.GetSystemsManager().ExecuteGroup(m_ScriptingUpdateGroup);
		m_World.GetSystemsManager().ExecuteGroup(m_OnFrameEnd);

		m_World.Entities.ClearQueuedForDeletion();
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
}