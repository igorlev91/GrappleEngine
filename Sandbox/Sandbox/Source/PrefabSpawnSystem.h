#pragma once

#include <GrappleCore/Serialization/TypeSerializer.h>
#include <GrappleCore/Serialization/SerializationStream.h>

#include <GrappleECS/System/SystemInitializer.h>
#include <GrappleECS/Entity/ComponentInitializer.h>

#include <Grapple/AssetManager/Asset.h>
#include <Grapple/AssetManager/AssetManager.h>

struct PrefabSpawner
{
	Grapple_COMPONENT;

	PrefabSpawner()
		: Enabled(false), PrefabHandle(12561055223819313244), Period(1.0f), TimeLeft(0.0f) {}

	bool Enabled;
	Grapple::AssetHandle PrefabHandle;

	float TimeLeft;
	float Period;
};

template<>
struct Grapple::TypeSerializer<PrefabSpawner>
{
	void OnSerialize(PrefabSpawner& prefabSpawner, Grapple::SerializationStream& stream)
	{
		stream.Serialize("Enabled", Grapple::SerializationValue(prefabSpawner.Enabled));
		stream.Serialize("PrefabHandle", Grapple::SerializationValue(prefabSpawner.PrefabHandle));
		stream.Serialize("TimeLeft", Grapple::SerializationValue(prefabSpawner.TimeLeft));
		stream.Serialize("Period", Grapple::SerializationValue(prefabSpawner.Period));
	}
};

struct PrefabSpawnSystem : Grapple::System
{
	Grapple_SYSTEM;

	virtual void OnConfig(Grapple::SystemConfig& config) override;
	virtual void OnUpdate(Grapple::SystemExecutionContext& context) override;
private:
	Grapple::Query m_Query;
};
