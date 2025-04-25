#pragma once

#include "Grapple/Renderer/PostProcessing/ToneMapping.h"
#include "Grapple/Renderer/PostProcessing/Vignette.h"

namespace Grapple
{
	struct PostProcessingManager
	{
		Ref<ToneMapping> ToneMappingPass;
		Ref<Vignette> VignettePass;
	};
}