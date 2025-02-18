#include "RenderCommand.h"

namespace Grapple
{
	Scope<RendererAPI> RenderCommand::s_API = RendererAPI::Create();
}