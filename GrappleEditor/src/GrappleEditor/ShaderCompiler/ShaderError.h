#pragma once

#include "GrappleEditor/ShaderCompiler/ShaderSyntax.h"

#include <string>

namespace Grapple
{
	struct ShaderError
	{
		ShaderError(SourcePosition position, const std::string& message)
			: Position(position), Message(message) {}

		SourcePosition Position;
		std::string Message;
	};
}