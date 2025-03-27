#include "Scene.h"

#include "Grapple/Renderer2D/Renderer2D.h"

#include "Grapple/Scene/Components.h"

#include "GrappleECS/Query/EntityArchetypesView.h"

namespace Grapple
{
	Scene::Scene()
	{
		m_CameraData.Projection = glm::mat4(1.0f);
		m_QuadShader = Shader::Create("QuadShader.glsl");

		m_World.RegisterComponent<TransformComponent>();
		m_World.RegisterComponent<CameraComponent>();
		m_World.RegisterComponent<SpriteComponent>();

		m_CameraDataUpdateQuery = m_World.CreateQuery<CameraComponent>();

		m_World.RegisterSystem(m_World.CreateQuery<TransformComponent, SpriteComponent>(), [this](EntityView view)
		{
			ComponentView<TransformComponent> transforms = view.View<TransformComponent>();
			ComponentView<SpriteComponent> sprites = view.View<SpriteComponent>();

			for (EntityViewElement entity : view)
			{
				TransformComponent& transform = transforms[entity];
				SpriteComponent& sprite = sprites[entity];

				Renderer2D::DrawQuad(transform.GetTransformationMatrix(), sprite.Color);
			}
		});

		Entity cameraEntity = m_World.CreateEntity<CameraComponent>();
		CameraComponent& camera = m_World.GetEntityComponent<CameraComponent>(cameraEntity);
		camera.Size = 10.0f;
		camera.Near = 0.1f;
		camera.Far = 10.0f;

		m_World.CreateEntity<TransformComponent, SpriteComponent>();
	}

	Scene::~Scene()
	{
	}

	void Scene::OnUpdateRuntime()
	{
		// TODO: call between Renderer2D::Begin() and Renderer2D::End()
		// TODO: maually execute system for updating camera data

		if (m_ViewportWidth != 0 && m_ViewportHeight != 0)
		{
			EntityArchetypesView archetypesView = m_World.GetRegistry().ExecuteQuery(m_CameraDataUpdateQuery);
			for (EntityView entityView : archetypesView)
			{
				ComponentView<CameraComponent> cameras = entityView.View<CameraComponent>();
				for (EntityViewElement entity : entityView)
				{
					CameraComponent& camera = cameras[entity];

					float halfSize = camera.Size / 2;
					float aspectRation = (float)m_ViewportWidth / (float)m_ViewportHeight;

					m_CameraData.Projection = glm::ortho(-halfSize * aspectRation, halfSize * aspectRation, -halfSize, halfSize, camera.Near, camera.Far);
				}
			}
		}

		Renderer2D::Begin(m_QuadShader, m_CameraData.Projection);

		m_World.OnUpdate();

		//Renderer2D::DrawQuad(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(2.0f), glm::vec4(0.8f, 0.3f, 0.2f, 1.0f));
		//Renderer2D::DrawQuad(glm::translate(glm::mat4(1.0f), glm::vec3(-0.2f, 0.5f, -1.0f)), nullptr, glm::vec4(0.2f, 0.9f, 0.2f, 1.0f));
		//Renderer2D::DrawQuad(glm::identity<glm::mat4>(), nullptr, glm::vec4(0.2f, 0.9f, 0.2f, 1.0f));
		Renderer2D::End();
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;
	}
}