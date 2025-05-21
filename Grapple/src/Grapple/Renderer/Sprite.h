#pragma once

#include "GrappleCore/Serialization/SerializationStream.h"

#include "Grapple/AssetManager/Asset.h"
#include "Grapple/Renderer/Texture.h"

#include <glm/glm.hpp>

namespace Grapple
{
	class Grapple_API Sprite : public Asset
	{
	public:
		Grapple_ASSET;
		Grapple_SERIALIZABLE;

		Sprite();
		Sprite(const Ref<Texture>& texture, glm::i32vec2 pixelsMin, glm::i32vec2 pixelsMax);

		inline const Ref<Texture>& GetTexture() const { return m_Texture; }
		inline void SetTexture(const Ref<Texture>& texture) { m_Texture = texture; }
	public:
		glm::vec2 UVMin;
		glm::vec2 UVMax;
	private:
		Ref<Texture> m_Texture;

		friend struct TypeSerializer<Sprite>;
	};

	template<>
	struct TypeSerializer<Sprite>
	{
		static void OnSerialize(Sprite& sprite, SerializationStream& stream)
		{
			stream.Serialize("Texture", SerializationValue(sprite.m_Texture));
			stream.Serialize("UVMin", SerializationValue(sprite.UVMin));
			stream.Serialize("UVMax", SerializationValue(sprite.UVMax));
		}
	};
}