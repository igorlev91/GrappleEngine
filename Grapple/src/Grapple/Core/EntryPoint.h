#include "Grapple/Core/Application.h"
#include "Grapple/Core/CommandLineArguments.h"
#include "GrappleCore/Log.h"

#include <stdint.h>

namespace Grapple
{
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