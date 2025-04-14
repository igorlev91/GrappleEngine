#pragma once

#include "Grapple/Core/Core.h"

namespace Grapple
{
	class GrapplePLATFORM_API WindowControls
	{
	public:
		virtual bool IsTitleBarHovered() const = 0;
	};
}