#include "RenderGraphCommon.h"

namespace Grapple
{
	void CompiledRenderGraph::Reset()
	{
		LayoutTransitions.clear();
		ExternalResourceFinalTransitions = LayoutTransitionsRange(0);
	}
}
