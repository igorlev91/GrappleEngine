#include <Grapple/Core/Application.h>
#include <Grapple/Core/Log.h>

#include <stdint.h>

namespace Grapple
{
	struct CommandLineArguments
	{
		const char** Arguments = nullptr;
		uint32_t ArgumentsCount = 0;

		const char* operator[](uint32_t index)
		{
			return Arguments[index];
		}
	};

	extern Scope<Application> CreateGrappleApplication(CommandLineArguments arguments);
}

int main(int argc, const char* argv[])
{
	Grapple::CommandLineArguments arguments;
	arguments.ArgumentsCount = argc;
	arguments.Arguments = argv;

	Grapple::Log::Initialize();

	Grapple::Scope<Grapple::Application> application = CreateGrappleApplication(arguments);
	application->Run();
	return 0;
}