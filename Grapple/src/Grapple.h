#pragma once

#include "GrappleCore/Core.h"
#include "GrappleCore/Log.h"
#include "GrappleCore/Assert.h"

// Rendering

#include "Grapple/Renderer/Buffer.h"
#include "Grapple/Renderer/GraphicsContext.h"
#include "Grapple/Renderer/Shader.h"
#include "Grapple/Renderer/Texture.h"
#include "Grapple/Renderer/FrameBuffer.h"

#include "Grapple/Renderer/Sprite.h"

#include <stdint.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
