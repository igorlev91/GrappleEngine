#pragma once

#include "GrappleCore/Core.h"
#include "Grapple/Renderer/Mesh.h"

namespace Grapple
{
	class Grapple_API RendererPrimitives
	{
	public:
		static Ref<const Mesh> GetCube();
		static Ref<const VertexArray> GetFullscreenQuad();
	};
}
