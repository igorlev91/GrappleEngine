#pragma once

#include "GrappleCore/Serialization/TypeSerializer.h"
#include "GrappleCore/Serialization/SerializationStream.h"

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

    template<>
    struct TypeSerializer<NameComponent>
    {
        void OnSerialize(NameComponent& name, SerializationStream& stream)
        {
            stream.Serialize("Value", SerializationValue(name.Value));
        }
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
        CameraComponent(ProjectionType projection);

        glm::mat4 GetProjection() const;
        glm::vec3 ScreenToWorld(glm::vec2 point) const;
        glm::vec3 ViewportToWorld(glm::vec2 point) const;

        ProjectionType Projection;

        float Size;
        float FOV;
        float Near;
        float Far;
    };

    template<>
    struct TypeSerializer<CameraComponent>
    {
        void OnSerialize(CameraComponent& camera, SerializationStream& stream)
        {
            stream.Serialize("Size", SerializationValue(camera.Size));
            stream.Serialize("FOV", SerializationValue(camera.FOV));
            stream.Serialize("Near", SerializationValue(camera.Near));
            stream.Serialize("Far", SerializationValue(camera.Far));
        }
    };



    struct Grapple_API SpriteComponent
    {
        Grapple_COMPONENT;

        SpriteComponent();
        SpriteComponent(AssetHandle sprite);

        glm::vec4 Color;
        glm::vec2 Tilling;
        Ref<Sprite> Sprite;
        SpriteRenderFlags Flags;
    };

    template<>
    struct TypeSerializer<SpriteComponent>
    {
        void OnSerialize(SpriteComponent& sprite, SerializationStream& stream)
        {
            stream.Serialize("Color", SerializationValue(sprite.Color, SerializationValueFlags::Color));
            stream.Serialize("Tiling", SerializationValue(sprite.Tilling));
            stream.Serialize("Sprite", SerializationValue(sprite.Sprite));
            
            using FlagsUnderlyingType = std::underlying_type_t<decltype(sprite.Flags)>;
            stream.Serialize("Flags", SerializationValue(reinterpret_cast<FlagsUnderlyingType&>(sprite.Flags)));
        }
    };



    struct Grapple_API SpriteLayer
    {
        Grapple_COMPONENT;

        SpriteLayer();
        SpriteLayer(int32_t layer);

        int32_t Layer;
    };

    template<>
    struct TypeSerializer<SpriteLayer>
    {
        void OnSerialize(SpriteLayer& value, SerializationStream& stream)
        {
            stream.Serialize("Layer", SerializationValue(value.Layer));
        }
    };



    struct Grapple_API MaterialComponent
    {
        Grapple_COMPONENT;

        MaterialComponent();
        MaterialComponent(AssetHandle handle);

        AssetHandle Material;
    };

    template<>
    struct TypeSerializer<MaterialComponent>
    {
        void OnSerialize(MaterialComponent& material, SerializationStream& stream)
        {
            stream.Serialize("Material", SerializationValue(material.Material));
        }
    };



    struct Grapple_API TextComponent
    {
        Grapple_COMPONENT;

        TextComponent();
        TextComponent(std::string_view text, const glm::vec4& color = glm::vec4(1.0f), const Ref<Font>& font = nullptr);

        std::string Text;
        glm::vec4 Color;
        Ref<Font> Font;
    };

    template<>
    struct TypeSerializer<TextComponent>
    {
        void OnSerialize(TextComponent& text, SerializationStream& stream)
        {
            stream.Serialize("Text", SerializationValue(text.Text));
            stream.Serialize("Color", SerializationValue(text.Color, SerializationValueFlags::Color));
            stream.Serialize("Font", SerializationValue(text.Font));
        }
    };



    struct Grapple_API MeshComponent
    {
        Grapple_COMPONENT;

        MeshComponent(MeshRenderFlags flags = MeshRenderFlags::None);
        MeshComponent(const Ref<Mesh>& mesh, AssetHandle material, MeshRenderFlags flags = MeshRenderFlags::None);

        Ref<Mesh> Mesh;
        AssetHandle Material;
        MeshRenderFlags Flags;
    };

    template<>
    struct TypeSerializer<MeshComponent>
    {
        void OnSerialize(MeshComponent& mesh, SerializationStream& stream)
        {
            stream.Serialize("Mesh", SerializationValue(mesh.Mesh));
            stream.Serialize("Material", SerializationValue(mesh.Material));

            using FlagsUnderlyingType = std::underlying_type_t<decltype(mesh.Flags)>;
            stream.Serialize("Flags", SerializationValue(reinterpret_cast<FlagsUnderlyingType&>(mesh.Flags)));
        }
    };



    struct Grapple_API Decal
    {
        Grapple_COMPONENT;

        Ref<Material> Material;
    };

    template<>
    struct TypeSerializer<Decal>
    {
        static void OnSerialize(Decal& decal, SerializationStream& stream)
        {
            stream.Serialize("Material", SerializationValue(decal.Material));
        }
    };



    struct Grapple_API DirectionalLight
    {
        Grapple_COMPONENT;

        DirectionalLight();
        DirectionalLight(const glm::vec3& color, float intensity);

        glm::vec3 Color;
        float Intensity;
    };

    template<>
    struct TypeSerializer<DirectionalLight>
    {
        void OnSerialize(DirectionalLight& light, SerializationStream& stream)
        {
            stream.Serialize("Color", SerializationValue(light.Color, SerializationValueFlags::Color));
            stream.Serialize("Intensity", SerializationValue(light.Intensity));
        }
    };



    struct Grapple_API PointLight
    {
        Grapple_COMPONENT;

        PointLight()
            : Color(glm::vec3(1.0f)), Intensity(1.0f) {}
        PointLight(const glm::vec3& color, float intensity)
            : Color(color), Intensity(intensity) {}

        glm::vec3 Color;
        float Intensity;
    };

    template<>
    struct TypeSerializer<PointLight>
    {
        void OnSerialize(PointLight& light, SerializationStream& stream)
        {
            stream.Serialize("Color", SerializationValue(light.Color, SerializationValueFlags::Color));
            stream.Serialize("Intensity", SerializationValue(light.Intensity));
        }
    };



    struct Grapple_API SpotLight
    {
        Grapple_COMPONENT;

        SpotLight()
            : Color(glm::vec3(1.0f)), Intensity(1.0f), InnerAngle(30.0f), OuterAngle(45.0f) {}
        SpotLight(const glm::vec3& color, float intensity, float innerAngle, float outerAngle)
            : Color(color), Intensity(intensity), InnerAngle(innerAngle), OuterAngle(outerAngle) {}

        glm::vec3 Color;
        float Intensity;

        float InnerAngle;
        float OuterAngle;
    };

    template<>
    struct TypeSerializer<SpotLight>
    {
        void OnSerialize(SpotLight& light, SerializationStream& stream)
        {
            stream.Serialize("Color", SerializationValue(light.Color, SerializationValueFlags::Color));
            stream.Serialize("Intensity", SerializationValue(light.Intensity));
            stream.Serialize("InnerAngle", SerializationValue(light.InnerAngle));
            stream.Serialize("OuterAngle", SerializationValue(light.OuterAngle));
        }
    };



    struct Grapple_API Environment
    {
        Grapple_COMPONENT;

        Environment();
        Environment(glm::vec3 color, float intensity);

        glm::vec3 EnvironmentColor;
        float EnvironmentColorIntensity;

        ShadowSettings ShadowSettings;
    };

    template<>
    struct TypeSerializer<Environment>
    {
        void OnSerialize(Environment& environment, SerializationStream& stream)
        {
            stream.Serialize("EnvironmentColor", SerializationValue(environment.EnvironmentColor, SerializationValueFlags::Color));
            stream.Serialize("EnvironmentColorIntensity", SerializationValue(environment.EnvironmentColorIntensity));

            stream.Serialize("ShadowSettings", SerializationValue(environment.ShadowSettings));
        }
    };
}