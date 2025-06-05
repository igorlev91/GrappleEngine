#include "Viewport.h"

namespace Grapple
{
	void Viewport::Resize(glm::ivec2 position, glm::ivec2 size)
	{
		m_Position = position;
		m_Size = size;
	}
}
