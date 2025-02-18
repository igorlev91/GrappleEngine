#include <Grapple/Core/Application.h>
#include <Grapple/Renderer/RenderCommand.h>

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

int main()
{
	SandboxApplication application;
	application.Run();
	return 0;
}