#pragma once

#include "Grapple.h"
#include "Grapple/Renderer2D/Renderer2D.h"
#include "Grapple/Renderer/Renderer.h"

#include "Grapple/Renderer/Mesh.h"
#include "Grapple/Renderer/Material.h"

#include "GrappleECS/World.h"
#include "GrappleECS/Entity/ComponentInitializer.h"

#include "Grapple/AssetManager/Asset.h"

namespace Grapple
{
	struct Grapple_API NameComponent
	{
		Grapple_COMPONENT;

		NameComponent();
		NameComponent(std::string_view name);
		~NameComponent();

		std::string Value;
	};

	struct Grapple_API TransformComponent
	{
		Grapple_COMPONENT;

		TransformComponent();
		TransformComponent(const glm::vec3& position);
		
		glm::mat4 GetTransformationMatrix() const;
		glm::vec3 TransformDirection(const glm::vec3& direction) const;

		glm::vec3 Position;
		glm::vec3 Rotation;
		glm::vec3 Scale;
	};

	struct Grapple_API CameraComponent
	{
		Grapple_COMPONENT;

		enum class ProjectionType : uint8_t
		{
			Orthographic,
			Perspective,
		};

		CameraComponent();

		glm::mat4 GetProjection() const;
		glm::vec3 ScreenToWorld(glm::vec2 point) const;
		glm::vec3 ViewportToWorld(glm::vec2 point) const;

		ProjectionType Projection;

		float Size;
		float FOV;
		float Near;
		float Far;
	};

	struct Grapple_API SpriteComponent
	{
		Grapple_COMPONENT;

		SpriteComponent();
		SpriteComponent(AssetHandle texture);

		glm::vec4 Color;
		glm::vec2 TextureTiling;
		AssetHandle Texture;
		SpriteRenderFlags Flags;
	};

	struct Grapple_API SpriteLayer
	{
		Grapple_COMPONENT;

		SpriteLayer();
		SpriteLayer(int32_t layer);

		int32_t Layer;
	};

	struct Grapple_API MaterialComponent
	{
		Grapple_COMPONENT;

		MaterialComponent();
		MaterialComponent(AssetHandle handle);

		AssetHandle Material;
	};

	struct Grapple_API TextComponent
	{
		Grapple_COMPONENT;

		TextComponent();
		TextComponent(std::string_view text, const glm::vec4& color = glm::vec4(1.0f), AssetHandle font = NULL_ASSET_HANDLE);

		std::string Text;
		glm::vec4 Color;
		AssetHandle Font;
	};

	struct Grapple_API MeshComponent
	{
		Grapple_COMPONENT;

		MeshComponent(MeshRenderFlags flags = MeshRenderFlags::None);
		MeshComponent(AssetHandle mesh, AssetHandle material, MeshRenderFlags flags = MeshRenderFlags::None);

		AssetHandle Mesh;
		AssetHandle Material;
		MeshRenderFlags Flags;
	};

	struct Grapple_API DirectionalLight
	{
		Grapple_COMPONENT;

		DirectionalLight();
		DirectionalLight(const glm::vec3& color, float intensity);

		glm::vec3 Color;
		float Intensity;
	};
}