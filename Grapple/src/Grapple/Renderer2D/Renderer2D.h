#pragma once

#include "Grapple.h"

#include <vector>

namespace Grapple
{
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 UV;
		float TextuteIndex;
		glm::vec2 TextureTiling;
	};

	constexpr size_t MaxTexturesCount = 32;

	struct Renderer2DData
	{
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

	class Renderer2D
	{
	public:
		static void Initialize(size_t maxQuads = 10000);
		static void Shutdown();

		static void Begin(const Ref<Shader>& shader, const glm::mat4& projectionMatrix);
		static void Flush();
		static void DrawQuad(glm::vec3 position, glm::vec2 size, glm::vec4 color);

		static void DrawQuad(glm::vec3 position, 
			glm::vec2 size, 
			const Ref<Texture>& texture, 
			glm::vec4 tint = glm::vec4(1), 
			glm::vec2 tilling = glm::vec2(1));

		static void End();
	private:
		static Renderer2DData* s_Data;
	};
}
