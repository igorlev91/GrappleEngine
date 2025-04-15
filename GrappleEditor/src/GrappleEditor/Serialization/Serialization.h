#pragma once

#include "GrappleCore/Serialization/TypeInitializer.h"

#include <yaml-cpp/yaml.h>

namespace Grapple
{
	void SerializeType(YAML::Emitter& emitter, const TypeInitializer& type, const uint8_t* data);
	void DeserializeType(YAML::Node& node, const TypeInitializer& type, uint8_t* data);
}