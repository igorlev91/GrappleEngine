#include "Renderer2DFrameData.h"

namespace Grapple
{
	void Renderer2DFrameData::Reset()
	{
		QuadCount = 0;
		QuadBatches.clear();

		TextQuadCount = 0;
		TextBatches.clear();
	}
}
