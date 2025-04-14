#pragma once

#include "Grapple/Core/Core.h"

#include <glm/glm.hpp>

namespace Grapple::Math
{
	Grapple_API bool DecomposeTransform(const glm::mat4& transform, glm::vec3& outTranslation, glm::vec3& outRotation, glm::vec3& outScale);
}