#pragma once

#include "Grapple.h"

#include <vector>

namespace Grapple
{
	struct Renderer2DStats
	{
		uint32_t QuadsCount;
		uint32_t DrawCalls;

		uint32_t GetTotalVertexCount() const { return QuadsCount * 4; }
	};

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 UV;
		float TextuteIndex;
		int32_t EntityIndex;
	};

	constexpr size_t MaxTexturesCount = 32;

	struct Renderer2DData
	{
		Renderer2DStats Stats;

		size_t QuadIndex;
		size_t MaxQuadCount;

		std::vector<QuadVertex> Vertices;

		Ref<VertexArray> VertexArray;
		Ref<VertexBuffer> VertexBuffer;
		Ref<IndexBuffer> IndexBuffer;

		Ref<Texture> WhiteTexture;
		Ref<Texture> Textures[MaxTexturesCount];
		uint32_t TextureIndex;

		Ref<Shader> CurrentShader;

		glm::vec3 QuadVertices[4];
		glm::vec2 QuadUV[4];

		glm::mat4 CameraProjectionMatrix;
	};

	enum class SpriteRenderFlags : uint8_t
	{
		None = 0,
		FlipX = 1,
		FlipY = 2,
	};

	constexpr SpriteRenderFlags operator|(SpriteRenderFlags a, SpriteRenderFlags b)
	{
		return (SpriteRenderFlags)((uint8_t)a | (uint8_t)b);
	}

	constexpr SpriteRenderFlags operator~(SpriteRenderFlags a)
	{
		return (SpriteRenderFlags)(~(uint8_t)a);
	}

	constexpr void operator|=(SpriteRenderFlags& a, SpriteRenderFlags b)
	{
		a = (SpriteRenderFlags)((uint8_t)a | (uint8_t)b);
	}

	constexpr SpriteRenderFlags operator&(SpriteRenderFlags a, SpriteRenderFlags b)
	{
		return (SpriteRenderFlags)((uint8_t)a & (uint8_t)b);
	}

	constexpr bool operator!=(SpriteRenderFlags a, int b)
	{
		return (int)a != b;
	}

	class Grapple_API Renderer2D
	{
	public:
		static void Initialize(size_t maxQuads = 10000);
		static void Shutdown();

		static void Begin(const Ref<Shader>& shader, const glm::mat4& projectionMatrix);
		static void Flush();
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);

		static void DrawQuad(const glm::mat4& transform,
			const glm::vec4& tint,
			const Ref<Texture>& texture,
			glm::vec2 tiling,
			int32_t entityIndex,
			SpriteRenderFlags flags = SpriteRenderFlags::None);

		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, 
			const Ref<Texture>& texture, 
			glm::vec4 tint = glm::vec4(1), 
			glm::vec2 tilling = glm::vec2(1));

		static void DrawSprite(const Sprite& sprite, const glm::vec3& position, const glm::vec2& size, const glm::vec4& color = glm::vec4(1.0f));

		static void End();

		static void ResetStats();
		static const Renderer2DStats& GetStats();
	private:
		static void DrawQuad(const glm::vec3& position, 
			const glm::vec2& size, 
			const Ref<Texture>& texture, 
			const glm::vec4& tint,
			const glm::vec2& tiling, 
			const glm::vec2* uv);
	};
}
