#pragma once

#include "GrappleCore/Core.h"
#include "GrappleECS/Commands/Command.h"

#include <stdint.h>

namespace Grapple
{
	class GrappleECS_API CommandsStorage
	{
	public:
		CommandsStorage(size_t capacity);
		~CommandsStorage();

		void Push(const CommandMetadata& meta, const void* commandData);
		std::pair<const CommandMetadata&, void*> Pop();
		bool CanRead();
		void Clear();
	private:
		void Reallocate();
	private:
		uint8_t* m_Buffer;
		size_t m_Size;
		size_t m_Capacity;

		size_t m_ReadPosition;
	};
}