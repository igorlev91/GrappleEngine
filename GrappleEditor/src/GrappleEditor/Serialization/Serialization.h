#pragma once

#include "GrappleCore/Serialization/Serialization.h"
#include "GrappleCore/Serialization/TypeInitializer.h"

#include <yaml-cpp/yaml.h>

namespace Grapple
{
	void SerializeObject(YAML::Emitter& emitter, const SerializableObject& object);
	void DeserializeObject(YAML::Node& node, SerializableObject& object);
}