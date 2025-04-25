#pragma once

#include "Grapple/Renderer/PostProcessing/ToneMapping.h"

namespace Grapple
{
	struct PostProcessingManager
	{
		Ref<ToneMapping> ToneMappingPass;
	};
}