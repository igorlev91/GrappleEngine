#pragma once

#include "GrappleCore/Core.h"
#include "GrappleCore/Collections/Span.h"
#include "GrappleCore/Serialization/TypeInitializer.h"
#include "GrappleCore/Serialization/Metadata.h"

#include "Grapple/AssetManager/Asset.h"

#include <glm/glm.hpp>

#include <filesystem>
#include <optional>
#include <GrappleCore/KeyCode.h>

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

		R11G11B10,

		R32G32B32,
		R32G32B32A32,

		RG8,
		RG16,

		RF32,

		R8,

		Depth24Stencil8,
		Depth32,

		// Compressed formats at the end
		BC1_RGB,
		BC1_RGBA,

		BC2_RGB,
		BC3_RGB,
		BC4_RGB,
		BC5_RGB,
	};

	Grapple_API bool IsCompressedTextureFormat(TextureFormat format);
	Grapple_API const char* TextureFormatToString(TextureFormat format);
	Grapple_API bool IsDepthTextureFormat(TextureFormat format);
	Grapple_API bool HasStencilComponent(TextureFormat format);

	enum class TextureFiltering
	{
		Closest,
		Linear,
	};

	enum class TextureUsage : uint8_t
	{
		None = 0,
		Sampling = 1,
		RenderTarget = 2,
	};

	Grapple_IMPL_ENUM_BITFIELD(TextureUsage);

	enum class ImageLayout
	{
		Undefined,
		General,
		ReadOnly,
		AttachmentOutput,
		TransferSource,
		TransferDestination,
	};

	struct TextureSpecifications
	{
		static constexpr uint32_t DefaultMipLevelsCount = 4;

		uint32_t Width = 0;
		uint32_t Height = 0;
		TextureFormat Format = TextureFormat::RGB8;
		TextureFiltering Filtering = TextureFiltering::Linear;
		TextureWrap Wrap = TextureWrap::Clamp;
		TextureUsage Usage = TextureUsage::Sampling;

		bool GenerateMipMaps = false;
	};

	Grapple_API const char* TextureWrapToString(TextureWrap wrap);
	Grapple_API const char* TextureFilteringToString(TextureFiltering filtering);

	Grapple_API std::optional<TextureWrap> TextureWrapFromString(std::string_view string);
	Grapple_API std::optional<TextureFiltering> TextureFilteringFromString(std::string_view string);

	struct Grapple_API TextureData
	{
		struct Mip
		{
			const void* Data = nullptr;
			size_t SizeInBytes = 0;
		};

		~TextureData();

		std::vector<Mip> Mips;
		size_t Size = 0;
		void* Data = nullptr;
	};

	class Grapple_API Texture : public Asset
	{
	public:
		Grapple_ASSET;
		Grapple_SERIALIZABLE;

		Texture()
			: Asset(AssetType::Texture) {}

		virtual void SetData(const void* data, size_t size) = 0;

		virtual const TextureSpecifications& GetSpecifications() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual TextureFormat GetFormat() const = 0;
		virtual TextureFiltering GetFiltering() const = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
	public:
		static Ref<Texture> Create(const TextureSpecifications& specifications);
		static Ref<Texture> Create(const std::filesystem::path& path, const TextureSpecifications& specifications);
		static Ref<Texture> Create(uint32_t width, uint32_t height, const void* data, TextureFormat format, TextureFiltering filtering = TextureFiltering::Linear);
		static Ref<Texture> Create(const TextureSpecifications& specifications, const TextureData& textureData);

		static bool ReadDataFromFile(const std::filesystem::path& path, TextureSpecifications& specifications, TextureData& data);
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
