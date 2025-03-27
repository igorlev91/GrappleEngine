#include "Scene.h"

#include "Grapple/Renderer2D/Renderer2D.h"
#include "Grapple/AssetManager/AssetManager.h"

#include "Grapple/Scene/Components.h"

namespace Grapple
{
	Scene::Scene()
	{
		m_CameraData.Projection = glm::mat4(1.0f);
		m_QuadShader = Shader::Create("QuadShader.glsl");

		m_World.RegisterComponent<TransformComponent>();
		m_World.RegisterComponent<CameraComponent>();
		m_World.RegisterComponent<SpriteComponent>();

		m_CameraDataUpdateQuery = m_World.CreateQuery<TransformComponent, CameraComponent>();

		m_World.RegisterSystem(m_World.CreateQuery<TransformComponent, SpriteComponent>(), [this](EntityView view)
		{
			ComponentView<TransformComponent> transforms = view.View<TransformComponent>();
			ComponentView<SpriteComponent> sprites = view.View<SpriteComponent>();

			for (EntityViewElement entity : view)
			{
				TransformComponent& transform = transforms[entity];
				SpriteComponent& sprite = sprites[entity];

				Renderer2D::DrawQuad(transform.GetTransformationMatrix(), sprite.Color, sprite.Texture == NULL_ASSET_HANDLE 
					? nullptr 
					: AssetManager::GetAsset<Texture>(sprite.Texture));
			}
		});
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
			for (EntityView entityView : m_CameraDataUpdateQuery)
			{
				ComponentView<CameraComponent> cameras = entityView.View<CameraComponent>();
				ComponentView<TransformComponent> transforms = entityView.View<TransformComponent>();

				for (EntityViewElement entity : entityView)
				{
					CameraComponent& camera = cameras[entity];

					glm::mat4 inverseTransform = glm::inverse(transforms[entity].GetTransformationMatrix());

					float halfSize = camera.Size / 2;
					float aspectRation = (float)m_ViewportWidth / (float)m_ViewportHeight;

					m_CameraData.Projection = glm::ortho(-halfSize * aspectRation, halfSize * aspectRation, -halfSize, halfSize, camera.Near, camera.Far) * inverseTransform;
				}
			}
		}

		Renderer2D::Begin(m_QuadShader, m_CameraData.Projection);

		m_World.OnUpdate();

		Renderer2D::End();
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;
	}
}