#pragma once

#include "Grapple/AssetManager/Asset.h"

#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>

namespace YAML
{
	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& vector)
		{
			Node node;
			node.push_back(vector.x);
			node.push_back(vector.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& out)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			out.x = node[0].as<float>();
			out.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& vector)
		{
			Node node;
			node.push_back(vector.x);
			node.push_back(vector.y);
			node.push_back(vector.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& out)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			out.x = node[0].as<float>();
			out.y = node[1].as<float>();
			out.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& vector)
		{
			Node node;
			node.push_back(vector.x);
			node.push_back(vector.y);
			node.push_back(vector.z);
			node.push_back(vector.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& out)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			out.x = node[0].as<float>();
			out.y = node[1].as<float>();
			out.z = node[2].as<float>();
			out.w = node[3].as<float>();
			return true;
		}
	};


	template<>
	struct convert<glm::ivec2>
	{
		static Node encode(const glm::ivec2& vector)
		{
			Node node;
			node.push_back(vector.x);
			node.push_back(vector.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::ivec2& out)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			out.x = node[0].as<int32_t>();
			out.y = node[1].as<int32_t>();
			return true;
		}
	};

	template<>
	struct convert<glm::ivec3>
	{
		static Node encode(const glm::ivec3& vector)
		{
			Node node;
			node.push_back(vector.x);
			node.push_back(vector.y);
			node.push_back(vector.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::ivec3& out)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			out.x = node[0].as<int32_t>();
			out.y = node[1].as<int32_t>();
			out.z = node[2].as<int32_t>();
			return true;
		}
	};

	template<>
	struct convert<glm::ivec4>
	{
		static Node encode(const glm::ivec4& vector)
		{
			Node node;
			node.push_back(vector.x);
			node.push_back(vector.y);
			node.push_back(vector.z);
			node.push_back(vector.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::ivec4& out)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			out.x = node[0].as<int32_t>();
			out.y = node[1].as<int32_t>();
			out.z = node[2].as<int32_t>();
			out.w = node[3].as<int32_t>();
			return true;
		}
	};

	template<>
	struct convert<Grapple::AssetHandle>
	{
		static Node encode(const Grapple::AssetHandle& handle)
		{
			return Node((uint64_t)(Grapple::UUID)handle);
		}

		static bool decode(const Node& node, Grapple::AssetHandle& out)
		{
			if (!node.IsScalar())
				return false;

			out = (Grapple::UUID)node.as<uint64_t>();
			return true;
		}
	};

	template<>
	struct convert<Grapple::UUID>
	{
		static Node encode(const Grapple::UUID& id)
		{
			return Node((uint64_t)id);
		}

		static bool decode(const Node& node, Grapple::UUID& out)
		{
			if (!node.IsScalar())
				return false;

			out = node.as<uint64_t>();
			return true;
		}
	};
}

inline YAML::Emitter& operator<<(YAML::Emitter& emitter, const glm::vec2& vector)
{
	emitter << YAML::Flow;
	emitter << YAML::BeginSeq << vector.x << vector.y << YAML::EndSeq;
	return emitter;
}

inline YAML::Emitter& operator<<(YAML::Emitter& emitter, const glm::vec3& vector)
{
	emitter << YAML::Flow;
	emitter << YAML::BeginSeq << vector.x << vector.y << vector.z << YAML::EndSeq;
	return emitter;
}

inline YAML::Emitter& operator<<(YAML::Emitter& emitter, const glm::vec4& vector)
{
	emitter << YAML::Flow;
	emitter << YAML::BeginSeq << vector.x << vector.y << vector.z << vector.w << YAML::EndSeq;
	return emitter;
}


inline YAML::Emitter& operator<<(YAML::Emitter& emitter, const glm::ivec2& vector)
{
	emitter << YAML::Flow;
	emitter << YAML::BeginSeq << vector.x << vector.y << YAML::EndSeq;
	return emitter;
}

inline YAML::Emitter& operator<<(YAML::Emitter& emitter, const glm::ivec3& vector)
{
	emitter << YAML::Flow;
	emitter << YAML::BeginSeq << vector.x << vector.y << vector.z << YAML::EndSeq;
	return emitter;
}

inline YAML::Emitter& operator<<(YAML::Emitter& emitter, const glm::ivec4& vector)
{
	emitter << YAML::Flow;
	emitter << YAML::BeginSeq << vector.x << vector.y << vector.z << vector.w << YAML::EndSeq;
	return emitter;
}


inline YAML::Emitter& operator<<(YAML::Emitter& emitter, Grapple::AssetHandle handle)
{
	emitter << (uint64_t)handle;
	return emitter;
}

inline YAML::Emitter& operator<<(YAML::Emitter& emitter, Grapple::UUID id)
{
	emitter << (uint64_t)id;
	return emitter;
}