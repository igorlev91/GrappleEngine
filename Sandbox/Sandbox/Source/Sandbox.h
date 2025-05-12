#pragma once

#include <GrappleCore/Serialization/TypeInitializer.h>
#include <GrappleCore/Serialization/TypeSerializer.h>
#include <GrappleCore/Serialization/SerializationStream.h>

#include <Grapple/AssetManager/AssetManager.h>

#include <GrappleECS/System/SystemInitializer.h>
#include <GrappleECS/Entity/ComponentInitializer.h>

namespace Sandbox
{
	using namespace Grapple;
	struct RotatingQuadData
	{
		Grapple_COMPONENT;
		float RotationSpeed;
		AssetHandle PrefabHandle;
	};

	struct SomeComponent
	{
		Grapple_COMPONENT;

		int a, b;

		SomeComponent()
			: a(100), b(-234) {}
	};

	struct TestComponent
	{
		Grapple_COMPONENT;
		int a;

		TestComponent()
			: a(1000) {}
	};
}

template<>
struct Grapple::TypeSerializer<Sandbox::RotatingQuadData>
{
	void OnSerialize(Sandbox::RotatingQuadData& data, Grapple::SerializationStream& stream)
	{
		stream.Serialize("RotationSpeed", Grapple::SerializationValue(data.RotationSpeed));
		stream.Serialize("PrefabHandle", Grapple::SerializationValue(data.PrefabHandle));
	}
};

template<>
struct Grapple::TypeSerializer<Sandbox::SomeComponent>
{
	void OnSerialize(Sandbox::SomeComponent& data, Grapple::SerializationStream& stream)
	{
		stream.Serialize("a", Grapple::SerializationValue(data.a));
		stream.Serialize("b", Grapple::SerializationValue(data.b));
	}
};

template<>
struct Grapple::TypeSerializer<Sandbox::TestComponent>
{
	void OnSerialize(Sandbox::TestComponent& data, Grapple::SerializationStream& stream)
	{
		stream.Serialize("a", Grapple::SerializationValue(data.a));
	}
};
