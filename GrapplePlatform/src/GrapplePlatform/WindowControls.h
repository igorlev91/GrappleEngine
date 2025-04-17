#pragma once

#include "GrappleCore/Core.h"

namespace Grapple
{
	class GrapplePLATFORM_API WindowControls
	{
	public:
		virtual bool IsTitleBarHovered() const = 0;
	};
}