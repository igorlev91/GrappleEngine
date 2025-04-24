#include "Components.h"

#include "Grapple/Renderer/Renderer.h"

namespace Grapple
{
	Grapple_IMPL_COMPONENT(TransformComponent,
		Grapple_FIELD(TransformComponent, Position),
		Grapple_FIELD(TransformComponent, Rotation),
		Grapple_FIELD(TransformComponent, Scale),
	);

	TransformComponent::TransformComponent()
		: Position(glm::vec3(0.0f)),
		  Rotation(glm::vec3(0.0f)),
		  Scale(glm::vec3(1.0f)) {}

	TransformComponent::TransformComponent(const glm::vec3& position)
		: Position(position), Rotation(glm::vec3(0.0f)), Scale(glm::vec3(1.0f)) {}

	glm::mat4 TransformComponent::GetTransformationMatrix() const
	{
		return glm::translate(glm::mat4(1.0f), Position) * glm::toMat4(glm::quat(glm::radians(Rotation))) *
			glm::scale(glm::mat4(1.0f), Scale);
	}

	glm::vec3 TransformComponent::TransformDirection(const glm::vec3& direction) const
	{
		return glm::rotate(glm::quat(glm::radians(Rotation)), direction);
	}

	Grapple_IMPL_COMPONENT(CameraComponent,
		Grapple_ENUM_FIELD(CameraComponent, Projection),
		Grapple_FIELD(CameraComponent, Size),
		Grapple_FIELD(CameraComponent, FOV),
		Grapple_FIELD(CameraComponent, Near),
		Grapple_FIELD(CameraComponent, Far),
	);

	CameraComponent::CameraComponent()
		: Projection(ProjectionType::Perspective),
		  Size(10.0f),
		  FOV(60.0f),
		  Near(0.1f),
		  Far(1000.0f) {}

	glm::mat4 CameraComponent::GetProjection() const
	{
		glm::uvec2 viewportSize = Renderer::GetMainViewport().GetSize();

		float halfSize = Size / 2;
		float aspectRation = (float)viewportSize.x / (float)viewportSize.y;

		if (Projection == CameraComponent::ProjectionType::Orthographic)
			return glm::ortho(-halfSize * aspectRation, halfSize * aspectRation, -halfSize, halfSize, Near, Far);
		else
			return glm::perspective<float>(glm::radians(FOV), aspectRation, Near, Far);
	}

	glm::vec3 CameraComponent::ScreenToWorld(glm::vec2 point) const
	{
		glm::mat4 projection = GetProjection();
		glm::mat4 inverseProjection = glm::inverse(projection);
		
		point = point / (glm::vec2)Renderer::GetMainViewport().GetSize() * 2.0f - glm::vec2(1.0f);
		return inverseProjection * glm::vec4(point, 0.0f, 1.0f);
	}

	glm::vec3 CameraComponent::ViewportToWorld(glm::vec2 point) const
	{
		glm::mat4 projection = GetProjection();
		glm::mat4 inverseProjection = glm::inverse(projection);
		return inverseProjection * glm::vec4(point, 0.0f, 1.0f);
	}

	Grapple_IMPL_COMPONENT(SpriteComponent,
		Grapple_FIELD(SpriteComponent, Color),
		Grapple_FIELD(SpriteComponent, TextureTiling),
		Grapple_FIELD(SpriteComponent, Texture),
		Grapple_ENUM_FIELD(SpriteComponent, Flags),
	);

	SpriteComponent::SpriteComponent()
		: Color(glm::vec4(1.0f)),
		  TextureTiling(glm::vec2(1.0f)),
		  Texture(0),
		  Flags(SpriteRenderFlags::None) {}

	SpriteComponent::SpriteComponent(AssetHandle texture)
		: Color(glm::vec4(1.0f)),
		TextureTiling(glm::vec2(1.0f)),
		Texture(texture),
		Flags(SpriteRenderFlags::None) {}

	Grapple_IMPL_COMPONENT(SpriteLayer, Grapple_FIELD(SpriteLayer, Layer));
	SpriteLayer::SpriteLayer()
		: Layer(0) {}

	SpriteLayer::SpriteLayer(int32_t layer)
		: Layer(layer) {}

	Grapple_IMPL_COMPONENT(MaterialComponent, Grapple_FIELD(MaterialComponent, Material));

	MaterialComponent::MaterialComponent()
		: Material(NULL_ASSET_HANDLE) {}

	MaterialComponent::MaterialComponent(AssetHandle handle)
		: Material(handle) {}


	Grapple_IMPL_COMPONENT(TextComponent,
		Grapple_FIELD(TextComponent, Text),
		Grapple_FIELD(TextComponent, Font),
	);
	TextComponent::TextComponent()
		: Color(1.0f), Font(NULL_ASSET_HANDLE) {}

	TextComponent::TextComponent(std::string_view text, const glm::vec4& color, AssetHandle font)
		: Text(text), Color(color), Font(font) {}

	Grapple_IMPL_COMPONENT(MeshComponent,
		Grapple_FIELD(MeshComponent, Mesh),
		Grapple_FIELD(MeshComponent, Material)
	);
	MeshComponent::MeshComponent()
		: Mesh(NULL_ASSET_HANDLE), Material(NULL_ASSET_HANDLE) {}

	MeshComponent::MeshComponent(AssetHandle mesh, AssetHandle material)
		: Mesh(mesh), Material(material) {}

	Grapple_IMPL_COMPONENT(DirectionalLight,
		Grapple_FIELD(DirectionalLight, Color),
		Grapple_FIELD(DirectionalLight, Intensity)
	);
	DirectionalLight::DirectionalLight()
		: Color(1.0f), Intensity(1.0f) {}

	DirectionalLight::DirectionalLight(const glm::vec3& color, float intensity)
		: Color(color), Intensity(intensity) {}
}