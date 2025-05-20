#pragma once

#include "Grapple/Renderer/PostProcessing/ToneMapping.h"
#include "Grapple/Renderer/PostProcessing/Vignette.h"
#include "Grapple/Renderer/PostProcessing/SSAO.h"

namespace Grapple
{
	struct PostProcessingManager
	{
		Ref<ToneMapping> ToneMappingPass;
		Ref<Vignette> VignettePass;
		Ref<SSAO> SSAOPass;
	};
}