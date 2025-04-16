#include "Viewport.h"

namespace Grapple
{
	Viewport::Viewport() {}

	void Viewport::Resize(const ViewportRect& newRect)
	{
		m_IsDirty = true;
		m_Rect = newRect;
	}
}
