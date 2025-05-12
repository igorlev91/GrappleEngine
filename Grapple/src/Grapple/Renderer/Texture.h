#pragma once

#include "GrappleCore/Core.h"

#include "Grapple/AssetManager/Asset.h"

#include <glm/glm.hpp>

#include <filesystem>
#include <optional>

namespace Grapple
{
	enum class TextureWrap
	{
		Clamp,
		Repeat,
	};

	enum class TextureFormat
	{
		RGB8,
		RGBA8,

		RG8,
		RG16,

		RF32,
	};

	enum class TextureFiltering
	{
		Closest,
		Linear,
	};

	struct TextureSpecifications
	{
		uint32_t Width;
		uint32_t Height;
		TextureFormat Format;
		TextureFiltering Filtering;
		TextureWrap Wrap;
	};

	Grapple_API const char* TextureWrapToString(TextureWrap wrap);
	Grapple_API const char* TextureFilteringToString(TextureFiltering filtering);

	Grapple_API std::optional<TextureWrap> TextureWrapFromString(std::string_view string);
	Grapple_API std::optional<TextureFiltering> TextureFilteringFromString(std::string_view string);

	class Grapple_API Texture : public Asset
	{
	public:
		Texture()
			: Asset(AssetType::Texture) {}

		virtual void Bind(uint32_t slot = 0) = 0;
		virtual void SetData(const void* data, size_t size) = 0;

		virtual const TextureSpecifications& GetSpecifications() const = 0;
		virtual void* GetRendererId() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual TextureFormat GetFormat() const = 0;
		virtual TextureFiltering GetFiltering() const = 0;
	public:
		static Ref<Texture> Create(const std::filesystem::path& path, const TextureSpecifications& specifications);
		static Ref<Texture> Create(uint32_t width, uint32_t height, const void* data, TextureFormat format, TextureFiltering filtering = TextureFiltering::Linear);
	};



	struct Texture3DSpecifications
	{
		glm::uvec3 Size;
		TextureFormat Format;
		TextureFiltering Filtering;
		TextureWrap Wrap;
	};

	class Grapple_API Texture3D
	{
	public:
		virtual void Bind(uint32_t slot = 0) = 0;
		virtual void* GetRendererId() const = 0;

		virtual void SetData(const void* data, glm::uvec3 dataSize) = 0;
		virtual const Texture3DSpecifications& GetSpecifications() const = 0;
	public:
		static Ref<Texture3D> Create(const Texture3DSpecifications& specifications);
		static Ref<Texture3D> Create(const Texture3DSpecifications& specifications, const void* data, glm::uvec3 dataSize);
	};
}
