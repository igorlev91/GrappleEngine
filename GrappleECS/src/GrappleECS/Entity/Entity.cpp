#include "Entity.h"

namespace Grapple
{
	bool operator==(const Entity& a, const Entity& b)
	{
		return a.m_Index == b.m_Index && a.m_Generation == b.m_Generation;
	}

	bool operator!=(const Entity& a, const Entity& b)
	{
		return a.m_Index != b.m_Index || a.m_Generation != b.m_Generation;
	}
}