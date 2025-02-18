#include <Grapple/Core/Application.h>
#include <Grapple/Renderer/RenderCommand.h>
#include <Grapple/Core/EntryPoint.h>

using namespace Grapple;

class SandboxApplication : public Application
{
public:
	SandboxApplication()
	{
		RenderCommand::SetClearColor(0.1f, 0.2f, 0.3f, 1);
	}

public:
	virtual void OnUpdate() override
	{
		RenderCommand::Clear();
	}
};

Scope<Application> Grapple::CreateGrappleApplication(Grapple::CommandLineArguments arguments)
{
	return CreateScope<SandboxApplication>();
}