#include "Grapple/Core/EntryPoint.h"

#include "GrappleEditor/EditorApplication.h"

Grapple::Scope<Grapple::Application> Grapple::CreateGrappleApplication(Grapple::CommandLineArguments arguments)
{
	return Grapple::CreateScope<Grapple::EditorApplication>(arguments);
}
