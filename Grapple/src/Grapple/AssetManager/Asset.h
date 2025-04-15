#pragma once

#include "GrappleCore/UUID.h"
#include "GrappleCore/Assert.h"

#include "GrappleCore/Serialization/TypeInitializer.h"

#include <filesystem>
#include <string_view>

namespace Grapple
{
	struct Grapple_API AssetHandle
	{
	public:
		Grapple_TYPE;

		AssetHandle() = default;
		constexpr AssetHandle(UUID uuid)
			: m_UUID(uuid) {}

		constexpr operator UUID() const { return m_UUID; }
		constexpr operator uint64_t() const { return (uint64_t)m_UUID; }

		constexpr bool operator==(AssetHandle other) const { return m_UUID == other.m_UUID; }
		constexpr bool operator!=(AssetHandle other) const { return m_UUID != other.m_UUID; }
		constexpr AssetHandle& operator=(AssetHandle other) { m_UUID = other.m_UUID; return *this; }
		constexpr AssetHandle& operator=(UUID uuid) { m_UUID = uuid; return *this; }
	private:
		UUID m_UUID;
	};

	constexpr AssetHandle NULL_ASSET_HANDLE = 0;

	enum class AssetType
	{
		None,
		Scene,
		Texture,
	};

	Grapple_API std::string_view AssetTypeToString(AssetType type);
	Grapple_API AssetType AssetTypeFromString(std::string_view string);

	struct AssetMetadata
	{
		AssetType Type;
		AssetHandle Handle;
		std::filesystem::path Path;
	};

	class Grapple_API Asset
	{
	public:
		Asset(AssetType type)
			: m_Type(type) {}
	public:
		inline AssetType GetType() const { return m_Type; }
	private:
		AssetType m_Type;
	public:
		AssetHandle Handle = NULL_ASSET_HANDLE;
	};
}

template<>
struct std::hash<Grapple::AssetHandle>
{
	size_t operator()(Grapple::AssetHandle handle) const
	{
		return std::hash<Grapple::UUID>()((Grapple::UUID)handle);
	}
};