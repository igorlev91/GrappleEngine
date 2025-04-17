#pragma once

#include "GrappleCore/Core.h"
#include "GrappleECS/Commands/Command.h"

#include <stdint.h>
#include <optional>

namespace Grapple
{
	class GrappleECS_API CommandsStorage
	{
	public:
		CommandsStorage(size_t capacity);
		~CommandsStorage();

		template<typename T>
		std::optional<const T*> Read(size_t location) const
		{
			if (location + sizeof(T) <= m_Capacity)
				return (const T*)(m_Buffer + location);
			return {};
		}

		template<typename T>
		bool Write(size_t location, const T& value)
		{
			if (location + sizeof(T) <= m_Capacity)
			{
				new(m_Buffer + location) T;
				*(T*)(m_Buffer + location) = value;
				return true;
			}
			return false;
		}

		std::optional<size_t> Allocate(size_t size);

		std::pair<CommandMetadata*, void*> AllocateCommand(size_t commandSize);
		std::pair<CommandMetadata&, Command*> Pop();
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