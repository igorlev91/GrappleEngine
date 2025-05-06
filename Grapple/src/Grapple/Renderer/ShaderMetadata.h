#pragma once

#include "GrappleCore/Core.h"
#include "GrappleCore/Assert.h"

#include <optional>
#include <stdint.h>

namespace Grapple
{
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

		Matrix4x4,
	};

	constexpr uint32_t ShaderDataTypeSize(ShaderDataType dataType)
	{
		switch (dataType)
		{
		case ShaderDataType::Int:
		case ShaderDataType::Float:
			return 4;
		case ShaderDataType::Float2:
			return 4 * 2;
		case ShaderDataType::Float3:
			return 4 * 3;
		case ShaderDataType::Float4:
			return 4 * 4;
		case ShaderDataType::Matrix4x4:
			return 4 * 4 * 4;
		case ShaderDataType::Sampler:
			return 4;
		}

		return 0;
	}

	constexpr uint32_t ShaderDataTypeComponentCount(ShaderDataType dataType)
	{
		switch (dataType)
		{
		case ShaderDataType::Int:
		case ShaderDataType::Float:
			return 1;
		case ShaderDataType::Float2:
			return 2;
		case ShaderDataType::Float3:
			return 3;
		case ShaderDataType::Float4:
			return 4;
		case ShaderDataType::Matrix4x4:
			return 16;
		}

		return 0;
	}

	struct ShaderProperty
	{
		ShaderProperty(std::string_view name, ShaderDataType type, size_t offset)
			: Name(name),
			Type(type),
			Offset(offset),
			Size(ShaderDataTypeSize(type)) {}

		ShaderProperty(std::string_view name, ShaderDataType type, size_t size, size_t offset)
			: Name(name),
			Type(type),
			Size(size),
			Offset(offset) {}

		std::string Name;
		ShaderDataType Type;
		size_t Offset;
		size_t Size;
	};

	enum class ShaderStageType
	{
		Vertex,
		Pixel
	};

	struct ShaderFeatures
	{
		ShaderFeatures()
			: Blending(BlendMode::Opaque),
			Culling(CullingMode::Back),
			DepthFunction(DepthComparisonFunction::Less),
			DepthTesting(true) {}

		BlendMode Blending;
		CullingMode Culling;
		DepthComparisonFunction DepthFunction;
		bool DepthTesting;
	};
}