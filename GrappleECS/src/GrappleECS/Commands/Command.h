#pragma once

#include "GrappleCore/Assert.h"
#include "GrappleECS/Entity/Entity.h"

#include <stdint.h>

namespace Grapple
{
	struct FutureEntity
	{
	public:
		constexpr FutureEntity()
			: Location(SIZE_MAX) {}
		constexpr FutureEntity(size_t location)
			: Location(location) {}

		size_t Location;
	};

	struct CommandMetadata
	{
		size_t CommandSize;
	};

	class GrappleECS_API CommandsStorage;
	class GrappleECS_API CommandContext
	{
	public:
		constexpr CommandContext(CommandMetadata& meta, CommandsStorage& storage)
			: m_Meta(meta), m_Storage(storage) {}

		Entity GetEntity(FutureEntity futureEntity);
		void SetEntity(FutureEntity futureEntity, Entity entity);
	private:
		const CommandMetadata& m_Meta;
		CommandsStorage& m_Storage;
	};

	class GrappleECS_API World;
	class GrappleECS_API Command
	{
	public:
		virtual ~Command() = default;
		virtual void Apply(CommandContext& context, World& world) = 0;
	};

	class GrappleECS_API EntityCreationCommand : public Command
	{
	public:
		virtual void Initialize(FutureEntity entity) = 0;
	};

	using ApplyCommandFunction = void(*)(Command*, World&);
}
