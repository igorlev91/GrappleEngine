#pragma once

#include "GrappleCore/Core.h"
#include "GrappleCore/Assert.h"

#include "GrappleCore/Serialization/SerializationStream.h"

#include <optional>
#include <stdint.h>

namespace Grapple
{
	enum class ShaderType
	{
		Unknown,
		_2D,
		Surface,
		FullscreenQuad,
		Debug,
		Decal,
	};

	Grapple_API uint32_t GetMaterialDescriptorSetIndex(ShaderType type);

	enum class BlendMode : uint8_t
	{
		Opaque,
		Transparent,
	};

	enum class CullingMode : uint8_t
	{
		None,
		Back,
		Front,
	};

	enum class DepthComparisonFunction : uint8_t
	{
		Less,
		Greater,

		LessOrEqual,
		GreaterOrEqual,

		Equal,
		NotEqual,

		Never,
		Always,
	};

	Grapple_API const char* CullingModeToString(CullingMode mode);
	Grapple_API std::optional<CullingMode> CullingModeFromString(std::string_view mode);

	Grapple_API const char* DepthComparisonFunctionToString(DepthComparisonFunction function);
	Grapple_API std::optional<DepthComparisonFunction> DepthComparisonFunctionFromString(std::string_view function);

	Grapple_API const char* BlendModeToString(BlendMode blendMode);
	Grapple_API std::optional<BlendMode> BlendModeFromString(std::string_view string);

	enum class ShaderTargetEnvironment
	{
		OpenGL,
		Vulkan,
	};

	enum class ShaderDataType
	{
		Int,
		Int2,
		Int3,
		Int4,
		Float,
		Float2,
		Float3,
		Float4,

		Sampler,
		SamplerArray,

		StorageImage,

		Matrix4x4,
	};

	Grapple_API uint32_t ShaderDataTypeSize(ShaderDataType dataType);
	Grapple_API uint32_t ShaderDataTypeComponentCount(ShaderDataType dataType);

	struct ShaderProperty
	{
		ShaderProperty() = default;
		ShaderProperty(std::string_view name, ShaderDataType type, size_t offset)
			: Name(name),
			Type(type),
			Offset(offset),
			Binding(UINT32_MAX),
			SamplerIndex(UINT32_MAX),
			Size(ShaderDataTypeSize(type)) {}

		ShaderProperty(std::string_view name, ShaderDataType type, size_t size, size_t offset)
			: Name(name),
			Type(type),
			Size(size),
			Binding(UINT32_MAX),
			SamplerIndex(UINT32_MAX),
			Offset(offset) {}

		std::string Name;
		std::string DisplayName;
		ShaderDataType Type;
		uint32_t Binding;
		uint32_t SamplerIndex;
		size_t Offset;
		size_t Size;

		bool Hidden = true;
		SerializationValueFlags Flags = SerializationValueFlags::None;
	};

	using ShaderProperties = std::vector<ShaderProperty>;

	// Indices of frame buffer attachments to which the shader writes
	using ShaderOutputs = std::vector<uint32_t>;

	enum class ShaderStageType
	{
		Vertex,
		Pixel,
		Compute,
	};

	Grapple_API const char* ShaderStageTypeToString(ShaderStageType stage);

	struct ShaderFeatures
	{
		ShaderFeatures()
			: Blending(BlendMode::Opaque),
			Culling(CullingMode::Back),
			DepthFunction(DepthComparisonFunction::Less),
			DepthTesting(true),
			DepthWrite(true),
			DepthBiasEnabled(false) {}

		BlendMode Blending;
		CullingMode Culling;
		DepthComparisonFunction DepthFunction;
		bool DepthTesting;
		bool DepthWrite;
		bool DepthBiasEnabled;
	};

	struct ShaderPushConstantsRange
	{
		ShaderStageType Stage = ShaderStageType::Vertex;
		size_t Offset = 0;
		size_t Size = 0;
	};

	struct VertexShaderInput
	{
		uint32_t Location = 0;
		ShaderDataType Type = ShaderDataType::Float;
	};

	struct ShaderMetadata
	{
		std::string Name;
		ShaderType Type = ShaderType::Unknown;
		ShaderFeatures Features;
		ShaderOutputs Outputs;
		std::vector<ShaderProperty> Properties;
		std::vector<ShaderStageType> Stages;
		std::vector<ShaderPushConstantsRange> PushConstantsRanges;

		std::vector<VertexShaderInput> VertexShaderInputs;
	};
}