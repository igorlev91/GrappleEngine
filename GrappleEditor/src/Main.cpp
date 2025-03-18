#include "Grapple.h"

#include "Grapple/Core/EntryPoint.h"
#include "Grapple/Renderer2D/Renderer2D.h"

#include "SandboxLayer.h"

namespace Grapple
{
	class SandboxApplication : public Application
	{
	public:
		SandboxApplication()
		{
			Renderer2D::Initialize();

			PushLayer(CreateRef<SandboxLayer>());
		}

		~SandboxApplication()
		{
			Renderer2D::Shutdown();
		}
	};
}

Grapple::Scope<Grapple::Application> Grapple::CreateGrappleApplication(Grapple::CommandLineArguments arguments)
{
	return Grapple::CreateScope<Grapple::SandboxApplication>();
}
