#include <Grapple/Core/Application.h>

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

	extern Scope<Application> CreateFlareApplication(CommandLineArguments arguments);
}

int main(int argc, const char* argv[])
{
	Grapple::CommandLineArguments arguments;
	arguments.ArgumentsCount = argc;
	arguments.Arguments = argv;

	Grapple::Scope<Grapple::Application> application = CreateFlareApplication(arguments);
	application->Run();
	return 0;
}