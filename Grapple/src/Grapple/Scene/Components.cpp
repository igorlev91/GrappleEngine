#include "Components.h"

namespace Grapple
{
	Grapple_IMPL_COMPONENT(NameComponent);
	NameComponent::NameComponent() {}

	NameComponent::NameComponent(std::string_view name)
		: Value(name) {}

	NameComponent::~NameComponent()
	{
	}

	Grapple_IMPL_COMPONENT(TransformComponent);
	TransformComponent::TransformComponent()
		: Position(glm::vec3(0.0f)),
		  Rotation(glm::vec3(0.0f)),
		  Scale(glm::vec3(1.0f)) {}

	TransformComponent::TransformComponent(const glm::vec3& position)
		: Position(position), Rotation(glm::vec3(0.0f)), Scale(glm::vec3(1.0f)) {}

	TransformComponent::TransformComponent(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
		: Position(position), Rotation(rotation), Scale(scale) {}

	glm::mat4 TransformComponent::GetTransformationMatrix() const
	{
		return glm::translate(glm::identity<glm::mat4>(), Position) * glm::toMat4(glm::quat(glm::radians(Rotation))) *
			glm::scale(glm::identity<glm::mat4>(), Scale);
	}

	glm::vec3 TransformComponent::TransformDirection(const glm::vec3& direction) const
	{
		return glm::rotate(glm::quat(glm::radians(Rotation)), direction);
	}

	Grapple_IMPL_COMPONENT(CameraComponent);
	CameraComponent::CameraComponent()
		: Projection(ProjectionType::Perspective),
		  Size(10.0f),
		  FOV(60.0f),
		  Near(0.1f),
		  Far(1000.0f) {}

	CameraComponent::CameraComponent(ProjectionType projection)
		: Projection(projection),
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

		point.y = Renderer::GetMainViewport().GetSize().y - point.y;

		point = (point / (glm::vec2)Renderer::GetMainViewport().GetSize()) * 2.0f - glm::vec2(1.0f);
		return inverseProjection * glm::vec4(point, 0.0f, 1.0f);
	}

	glm::vec3 CameraComponent::ViewportToWorld(glm::vec2 point) const
	{
		glm::mat4 projection = GetProjection();
		glm::mat4 inverseProjection = glm::inverse(projection);
		return inverseProjection * glm::vec4(point, 0.0f, 1.0f);
	}

	Grapple_IMPL_COMPONENT(SpriteComponent);
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

	Grapple_IMPL_COMPONENT(SpriteLayer);
	SpriteLayer::SpriteLayer()
		: Layer(0) {}

	SpriteLayer::SpriteLayer(int32_t layer)
		: Layer(layer) {}

	Grapple_IMPL_COMPONENT(MaterialComponent);
	MaterialComponent::MaterialComponent()
		: Material(NULL_ASSET_HANDLE) {}

	MaterialComponent::MaterialComponent(AssetHandle handle)
		: Material(handle) {}



	Grapple_IMPL_COMPONENT(TextComponent);
	TextComponent::TextComponent()
		: Color(1.0f), Font(nullptr) {}

	TextComponent::TextComponent(std::string_view text, const glm::vec4& color, const Ref<Grapple::Font>& font)
		: Text(text), Color(color), Font(font) {}



	Grapple_IMPL_COMPONENT(MeshComponent);
	MeshComponent::MeshComponent(MeshRenderFlags flags)
		: Mesh(NULL_ASSET_HANDLE), Material(NULL_ASSET_HANDLE), Flags(flags) {}

	MeshComponent::MeshComponent(AssetHandle mesh, AssetHandle material, MeshRenderFlags flags)
		: Mesh(mesh), Material(material), Flags(flags) {}



	Grapple_IMPL_COMPONENT(Decal);



	Grapple_IMPL_COMPONENT(DirectionalLight);
	DirectionalLight::DirectionalLight()
		: Color(1.0f), Intensity(1.0f) {}

	DirectionalLight::DirectionalLight(const glm::vec3& color, float intensity)
		: Color(color), Intensity(intensity) {}



	Grapple_IMPL_COMPONENT(PointLight);
	Grapple_IMPL_COMPONENT(SpotLight);


	Grapple_IMPL_COMPONENT(Environment);
	Environment::Environment()
		: EnvironmentColor(0.0f), EnvironmentColorIntensity(0.0f) {}

	Environment::Environment(glm::vec3 color, float intensity)
		: EnvironmentColor(color), EnvironmentColorIntensity(intensity) {}
}