#include "Renderer2D.h"

#include "GrappleCore/Core.h"
#include "GrappleCore/Profiler/Profiler.h"

#include "Grapple/Math/Math.h"

#include "Grapple/AssetManager/AssetManager.h"

#include "Grapple/Renderer/Viewport.h"
#include "Grapple/Renderer/Renderer.h"
#include "Grapple/Renderer/ShaderLibrary.h"
#include "Grapple/Renderer/Pipeline.h"

#include "Grapple/Renderer2D/Renderer2DFrameData.h"
#include "Grapple/Renderer2D/Geometry2DPass.h"
#include "Grapple/Renderer2D/TextPass.h"

#include "Grapple/Project/Project.h"

#include "Grapple/Platform/Vulkan/VulkanPipeline.h"
#include "Grapple/Platform/Vulkan/VulkanContext.h"
#include "Grapple/Platform/Vulkan/VulkanDescriptorSet.h"

#include <algorithm>
#include <cctype>

namespace Grapple
{
	struct Renderer2DData
	{
		Renderer2DStats Stats;

		Renderer2DFrameData FrameData;
		Renderer2DLimits Limits;

		Ref<IndexBuffer> IndexBuffer = nullptr;

		Ref<Material> DefaultMaterial = nullptr;
		Ref<Material> CurrentMaterial = nullptr;
		Ref<Shader> TextShader = nullptr;

		glm::vec3 QuadVertices[4] = { glm::vec3(0.0f) };
		glm::vec2 QuadUV[4] = { glm::vec2(0.0f) };
	};

	Renderer2DData s_Renderer2DData;

	static void ReloadShaders();

	void Renderer2D::Initialize(size_t maxQuads)
	{
		s_Renderer2DData.Limits.MaxQuadCount = (uint32_t)maxQuads;

		s_Renderer2DData.FrameData.QuadVertices.resize(maxQuads * 4);
		s_Renderer2DData.FrameData.TextVertices.resize(maxQuads * 4);

		std::vector<uint32_t> indices(maxQuads * 6);

		for (size_t quadIndex = 0; quadIndex < maxQuads; quadIndex++)
		{
			indices[quadIndex * 6 + 0] = (uint32_t)(quadIndex * 4 + 0);
			indices[quadIndex * 6 + 1] = (uint32_t)(quadIndex * 4 + 1);
			indices[quadIndex * 6 + 2] = (uint32_t)(quadIndex * 4 + 2);
			indices[quadIndex * 6 + 3] = (uint32_t)(quadIndex * 4 + 0);
			indices[quadIndex * 6 + 4] = (uint32_t)(quadIndex * 4 + 2);
			indices[quadIndex * 6 + 5] = (uint32_t)(quadIndex * 4 + 3);
		}

		s_Renderer2DData.IndexBuffer = IndexBuffer::Create(IndexBuffer::IndexFormat::UInt32, MemorySpan::FromVector(indices));

		s_Renderer2DData.QuadVertices[0] = glm::vec3(-0.5f, -0.5f, 0.0f);
		s_Renderer2DData.QuadVertices[1] = glm::vec3(-0.5f, 0.5f, 0.0f);
		s_Renderer2DData.QuadVertices[2] = glm::vec3(0.5f, 0.5f, 0.0f);
		s_Renderer2DData.QuadVertices[3] = glm::vec3(0.5f, -0.5f, 0.0f);

		s_Renderer2DData.QuadUV[0] = glm::vec2(0.0f, 0.0f);
		s_Renderer2DData.QuadUV[1] = glm::vec2(0.0f, 1.0f);
		s_Renderer2DData.QuadUV[2] = glm::vec2(1.0f, 1.0f);
		s_Renderer2DData.QuadUV[3] = glm::vec2(1.0f, 0.0f);

		s_Renderer2DData.Stats.DrawCalls = 0;
		s_Renderer2DData.Stats.QuadsCount = 0;

		Project::OnProjectOpen.Bind(ReloadShaders);

		if (RendererAPI::GetAPI() == RendererAPI::API::Vulkan)
		{
			{
				VkDescriptorSetLayoutBinding bindings[1] = {};
				bindings[0].binding = 0;
				bindings[0].descriptorCount = Renderer2DLimits::MaxTexturesCount;
				bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				bindings[0].pImmutableSamplers = nullptr;

				s_Renderer2DData.FrameData.QuadDescriptorSetsPool = CreateRef<VulkanDescriptorSetPool>(32, Span(bindings, 1));
			}

			{
				VkDescriptorSetLayoutBinding bindings[1] = {};
				bindings[0].binding = 0;
				bindings[0].descriptorCount = 1;
				bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				bindings[0].pImmutableSamplers = nullptr;

				s_Renderer2DData.FrameData.TextDescriptorSetsPool = CreateRef<VulkanDescriptorSetPool>(32, Span(bindings, 1));
			}
		}
	}

	static void ReloadShaders()
	{
		std::optional<AssetHandle> quadShaderHandle = ShaderLibrary::FindShader("QuadShader");

		if (!quadShaderHandle || !AssetManager::IsAssetHandleValid(quadShaderHandle.value()))
			Grapple_CORE_ERROR("Renderer 2D: Failed to find Quad shader");
		else
		{
			Ref<Shader> quadShader = AssetManager::GetAsset<Shader>(quadShaderHandle.value());
			s_Renderer2DData.DefaultMaterial = Material::Create(quadShader);
			s_Renderer2DData.CurrentMaterial = s_Renderer2DData.DefaultMaterial;
		}

		std::optional<AssetHandle> textShaderHandle = ShaderLibrary::FindShader("Text");
		if (!textShaderHandle || !AssetManager::IsAssetHandleValid(textShaderHandle.value()))
			Grapple_CORE_ERROR("Renderer 2D: Failed to find Text shader");
		else
		{
			s_Renderer2DData.TextShader = AssetManager::GetAsset<Shader>(textShaderHandle.value());
		}
	}

	static void FillRemainingTextureSlots(QuadsBatch& batch)
	{
		Ref<Texture> whiteTexture = Renderer::GetWhiteTexture();
		for (uint32_t i = batch.TexturesCount; i < Renderer2DLimits::MaxTexturesCount; i++)
		{
			batch.Textures[i] = whiteTexture;
		}
	}

	static QuadsBatch& BeginQuadBatch()
	{
		// Fill remaining texture slots of the previous batch with white textures
		if (s_Renderer2DData.FrameData.QuadBatches.size() > 0)
		{
			FillRemainingTextureSlots(s_Renderer2DData.FrameData.QuadBatches.back());
		}

		return s_Renderer2DData.FrameData.QuadBatches.emplace_back();
	}

	void Renderer2D::Shutdown()
	{
		s_Renderer2DData = {};
	}

	void Renderer2D::BeginFrame()
	{
		Grapple_PROFILE_FUNCTION();

		s_Renderer2DData.FrameData.QuadBatches.clear();
	}

	void Renderer2D::EndFrame()
	{

	}

	void Renderer2D::Begin(const Ref<Material>& material)
	{
		s_Renderer2DData.FrameData.Reset();

		s_Renderer2DData.CurrentMaterial = material == nullptr ? s_Renderer2DData.DefaultMaterial : material;
	}

	void Renderer2D::End()
	{
		// Make sure that all empty texture slots of the last batch are filled with white textures
		if (s_Renderer2DData.FrameData.QuadBatches.size() > 0)
		{
			FillRemainingTextureSlots(s_Renderer2DData.FrameData.QuadBatches.back());
		}
	}

	void Renderer2D::ConfigurePasses(Viewport& viewport)
	{
		RenderGraphPassSpecifications geometryPass{};
		geometryPass.SetDebugName("2DGeometryPass");
		geometryPass.SetType(RenderGraphPassType::Graphics);
		geometryPass.AddOutput(viewport.ColorTexture, 0);
		
		viewport.Graph.AddPass(geometryPass, CreateRef<Geometry2DPass>(
			s_Renderer2DData.FrameData,
			s_Renderer2DData.Limits,
			s_Renderer2DData.IndexBuffer,
			s_Renderer2DData.DefaultMaterial));

		RenderGraphPassSpecifications textPass{};
		textPass.SetDebugName("TextPass");
		textPass.SetType(RenderGraphPassType::Graphics);
		textPass.AddOutput(viewport.ColorTexture, 0);

		viewport.Graph.AddPass(textPass, CreateRef<TextPass>(
			s_Renderer2DData.FrameData,
			s_Renderer2DData.Limits,
			s_Renderer2DData.IndexBuffer,
			s_Renderer2DData.TextShader));
	}

	void Renderer2D::ResetStats()
	{
		s_Renderer2DData.Stats.DrawCalls = 0;
		s_Renderer2DData.Stats.QuadsCount = 0;
	}

	const Renderer2DStats& Renderer2D::GetStats()
	{
		return s_Renderer2DData.Stats;
	}

	void Renderer2D::SetMaterial(const Ref<Material>& material)
	{
		Grapple_CORE_ASSERT(s_Renderer2DData.FrameData.QuadBatches.size() > 0);
		Grapple_CORE_ASSERT(material);

		size_t lastBatchIndex = s_Renderer2DData.FrameData.QuadBatches.size() - 1;
		QuadsBatch& batch = BeginQuadBatch();
		batch.Material = material;
		batch.Count = 0;

		if (s_Renderer2DData.FrameData.QuadBatches.size() > 0)
			batch.Start = s_Renderer2DData.FrameData.QuadBatches[lastBatchIndex].GetEnd();

		s_Renderer2DData.CurrentMaterial = material;
	}

	Ref<Material> Renderer2D::GetMaterial()
	{
		return s_Renderer2DData.CurrentMaterial;
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		glm::vec3 vertices[4];
		for (size_t i = 0; i < 4; i++)
			vertices[i] = s_Renderer2DData.QuadVertices[i] * glm::vec3(size, 1.0f) + position;

		DrawQuad(vertices, nullptr, color, glm::vec2(1.0f), s_Renderer2DData.QuadUV, INT32_MAX);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position,
		const glm::vec2& size,
		const glm::vec4& color,
		const Ref<Texture> texture,
		glm::vec2 uvMin, glm::vec2 uvMax)
	{
		glm::vec2 uvs[] =
		{
			uvMin,
			glm::vec2(uvMin.x, uvMax.y),
			uvMax,
			glm::vec2(uvMax.x, uvMin.y)
		};

		glm::vec3 vertices[4];
		for (size_t i = 0; i < 4; i++)
			vertices[i] = s_Renderer2DData.QuadVertices[i] * glm::vec3(size, 1.0f) + position;

		DrawQuad(vertices, texture, color, glm::vec2(1.0f), uvs, INT32_MAX);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& tint, const Ref<Texture>& texture,
		glm::vec2 tiling, int32_t entityIndex, SpriteRenderFlags flags)
	{
		glm::vec3 vertices[4];
		for (size_t i = 0; i < 4; i++)
			vertices[i] = transform * glm::vec4(s_Renderer2DData.QuadVertices[i], 1.0f);

		glm::vec2 uvs[4];
		for (size_t i = 0; i < 4; i++)
			uvs[i] = s_Renderer2DData.QuadUV[i];

		if (HAS_BIT(flags, SpriteRenderFlags::FlipX))
		{
			for (size_t i = 0; i < 4; i++)
				uvs[i].x = 1.0f - uvs[i].x;
		}

		if (HAS_BIT(flags, SpriteRenderFlags::FlipY))
		{
			for (size_t i = 0; i < 4; i++)
				uvs[i].y = 1.0f - uvs[i].y;
		}

		DrawQuad(vertices, texture, tint, tiling, uvs, entityIndex);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture>& texture, glm::vec4 tint, glm::vec2 tiling, int32_t entityIndex)
	{
		glm::vec3 vertices[4];
		for (size_t i = 0; i < 4; i++)
			vertices[i] = s_Renderer2DData.QuadVertices[i] * glm::vec3(size, 1.0f) + position;

		DrawQuad(vertices, texture, tint, tiling, s_Renderer2DData.QuadUV, entityIndex);
	}

	void Renderer2D::DrawSprite(const Sprite& sprite, const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int32_t entityIndex)
	{
		glm::vec2 uv[4] =
		{
			sprite.UVMin,
			glm::vec2(sprite.UVMin.x, sprite.UVMax.y),
			sprite.UVMax,
			glm::vec2(sprite.UVMax.x, sprite.UVMin.y),
		};

		glm::vec3 vertices[4];
		for (size_t i = 0; i < 4; i++)
			vertices[i] = s_Renderer2DData.QuadVertices[i] * glm::vec3(size, 1.0f) + position;

		DrawQuad(vertices, sprite.GetTexture(), color, glm::vec2(1.0f), uv, entityIndex);
	}

	void Renderer2D::DrawSprite(const Ref<Sprite>& sprite, const glm::mat4& transform, const glm::vec4& color, glm::vec2 tilling, SpriteRenderFlags flags, int32_t entityIndex)
	{
		glm::vec2 uvMin = glm::vec2(0.0f);
		glm::vec2 uvMax = glm::vec2(1.0f);

		if (sprite)
		{
			uvMin = sprite->UVMin;
			uvMax = sprite->UVMax;
		}

		if (HAS_BIT(flags, SpriteRenderFlags::FlipX))
			std::swap(uvMin.x, uvMax.x);
		if (HAS_BIT(flags, SpriteRenderFlags::FlipY))
			std::swap(uvMin.y, uvMax.y);

		glm::vec2 uv[4] =
		{
			uvMin,
			glm::vec2(uvMin.x, uvMax.y),
			uvMax,
			glm::vec2(uvMax.x, uvMin.y),
		};

		for (size_t i = 0; i < 4; i++)
			uv[i] *= tilling;

		glm::vec3 vertices[4];

		for (size_t i = 0; i < 4; i++)
			vertices[i] = transform * glm::vec4(s_Renderer2DData.QuadVertices[i], 1.0f);

		DrawQuad(vertices, sprite == nullptr ? nullptr : sprite->GetTexture(), color, tilling, uv, entityIndex);
	}

	void Renderer2D::DrawQuad(const glm::vec3* vertices, const Ref<Texture>& texture, const glm::vec4& tint, const glm::vec2& tiling, const glm::vec2* uv, int32_t entityIndex)
	{
		if (s_Renderer2DData.FrameData.QuadBatches.size() == 0)
		{
			QuadsBatch& batch = BeginQuadBatch();
			batch.Material = s_Renderer2DData.DefaultMaterial;
		}

		if (s_Renderer2DData.FrameData.QuadCount >= s_Renderer2DData.Limits.MaxQuadCount)
		{
			Grapple_CORE_ASSERT(false);
		}

		if (s_Renderer2DData.FrameData.QuadBatches.back().TexturesCount == Renderer2DLimits::MaxTexturesCount)
		{
			BeginQuadBatch();
		}

		QuadsBatch& currentBatch = s_Renderer2DData.FrameData.QuadBatches.back();
		size_t vertexIndex = s_Renderer2DData.FrameData.QuadCount * 4;
		uint32_t textureIndex = currentBatch.GetTextureIndex(texture == nullptr ? Renderer::GetWhiteTexture() : texture);

		for (uint32_t i = 0; i < 4; i++)
		{
			QuadVertex& vertex = s_Renderer2DData.FrameData.QuadVertices[vertexIndex + i];
			vertex.Position = vertices[i];
			vertex.Color = tint;
			vertex.UV = uv[i] * tiling;
			vertex.TextureIndex = textureIndex;
			vertex.EntityIndex = entityIndex;
		}

		currentBatch.Count++;
		s_Renderer2DData.FrameData.QuadCount++;
		s_Renderer2DData.Stats.QuadsCount++;
	}

	void Renderer2D::DrawString(std::string_view text, const glm::mat4& transform, const Ref<Font>& font, const glm::vec4& color, int32_t entityIndex)
	{
		Grapple_PROFILE_FUNCTION();

		if (s_Renderer2DData.FrameData.TextBatches.empty() || s_Renderer2DData.FrameData.TextBatches.back().Font.get() != font.get())
		{
			TextBatch& batch = s_Renderer2DData.FrameData.TextBatches.emplace_back();
			batch.Start = (uint32_t)s_Renderer2DData.FrameData.TextQuadCount;
			batch.Count = 0;
			batch.Font = font;
		}

		TextBatch& batch = s_Renderer2DData.FrameData.TextBatches.back();

		const auto& msdfData = font->GetData();
		const auto& geometry = msdfData.Geometry;
		const auto& metrics = msdfData.Geometry.getMetrics();

		float kerningOffset = 0.0f;
		float lineHeightOffset = 0.0f;

		const Ref<Texture>& fontAtlas = font->GetAtlas();
		glm::vec2 texelSize = glm::vec2(1.0f / fontAtlas->GetWidth(), 1.0f / fontAtlas->GetHeight());
		glm::vec2 position = glm::vec2(0.0f);

		float fontScale = 1.0f / (float)(metrics.ascenderY - metrics.descenderY);
		position.y = -fontScale * (float)metrics.ascenderY;

		const msdf_atlas::GlyphGeometry* errorGlyph = geometry.getGlyph('?');
		const msdf_atlas::GlyphGeometry* spaceGlyph = geometry.getGlyph(' ');
		
		struct Rect
		{
			double Top;
			double Right;
			double Bottom;
			double Left;
		};

		for (size_t charIndex = 0; charIndex < text.size(); charIndex++)
		{
			if (text[charIndex] == 0)
				break;

			const msdf_atlas::GlyphGeometry* glyph = geometry.getGlyph(text[charIndex]);

			if (!glyph)
				glyph = errorGlyph;
			if (!glyph)
				return;

			if (text[charIndex] == '\r')
				continue;
			else if (text[charIndex] == '\t')
				glyph = spaceGlyph;
			else if (text[charIndex] == '\n')
			{
				position.x = 0;
				position.y -= fontScale * (float)metrics.lineHeight + lineHeightOffset;
				continue;
			}
			else if (!std::isspace(text[charIndex]))
			{
				Rect atlasBounds;
				Rect planeBounds;
				glyph->getQuadAtlasBounds(atlasBounds.Left, atlasBounds.Bottom, atlasBounds.Right, atlasBounds.Top);
				glyph->getQuadPlaneBounds(planeBounds.Left, planeBounds.Bottom, planeBounds.Right, planeBounds.Top);

				glm::vec2 min = glm::vec2(planeBounds.Left, planeBounds.Bottom);
				glm::vec2 max = glm::vec2(planeBounds.Right, planeBounds.Top);

				min = min * fontScale + position;
				max = max * fontScale + position;

				atlasBounds.Left *= texelSize.x;
				atlasBounds.Right *= texelSize.x;
				atlasBounds.Top *= texelSize.y;
				atlasBounds.Bottom *= texelSize.y;

				size_t textVertexIndex = s_Renderer2DData.FrameData.TextQuadCount * 4;

				{
					TextVertex& vertex = s_Renderer2DData.FrameData.TextVertices[textVertexIndex + 0];
					vertex.Position = transform * glm::vec4(min, 0.0f, 1.0f);
					vertex.Color = color;
					vertex.UV = glm::vec2((float)atlasBounds.Left, (float)atlasBounds.Bottom);
					vertex.EntityIndex = entityIndex;
				}

				{
					TextVertex& vertex = s_Renderer2DData.FrameData.TextVertices[textVertexIndex + 1];
					vertex.Position = transform * glm::vec4(min.x, max.y, 0.0f, 1.0f);
					vertex.Color = color;
					vertex.UV = glm::vec2((float)atlasBounds.Left, (float)atlasBounds.Top);
					vertex.EntityIndex = entityIndex;
				}

				{
					TextVertex& vertex = s_Renderer2DData.FrameData.TextVertices[textVertexIndex + 2];
					vertex.Position = transform * glm::vec4(max, 0.0f, 1.0f);
					vertex.Color = color;
					vertex.UV = glm::vec2((float)atlasBounds.Right, (float)atlasBounds.Top);
					vertex.EntityIndex = entityIndex;
				}

				{
					TextVertex& vertex = s_Renderer2DData.FrameData.TextVertices[textVertexIndex + 3];
					vertex.Position = transform * glm::vec4(max.x, min.y, 0.0f, 1.0f);
					vertex.Color = color;
					vertex.UV = glm::vec2((float)atlasBounds.Right, (float)atlasBounds.Bottom);
					vertex.EntityIndex = entityIndex;
				}

				batch.Count++;
				s_Renderer2DData.FrameData.TextQuadCount++;
				s_Renderer2DData.Stats.QuadsCount++;

				Grapple_CORE_ASSERT(s_Renderer2DData.FrameData.TextQuadCount < s_Renderer2DData.Limits.MaxQuadCount);
			}

			if (charIndex + 1 < text.size())
			{
				double advance = 0.0;

				// TODO: properly handle tabs
				char currentChar = text[charIndex];
				if (currentChar == '\t')
					currentChar = ' ';

				geometry.getAdvance(advance, currentChar, text[charIndex + 1]);

				if (text[charIndex] == '\t')
					advance *= 4.0;

				position.x += fontScale * (float)advance + kerningOffset;
			}
		}
	}

	Ref<const DescriptorSetLayout> Renderer2D::GetDescriptorSetLayout()
	{
		return s_Renderer2DData.FrameData.QuadDescriptorSetsPool->GetLayout();
	}

	const Renderer2DLimits& Renderer2D::GetLimits()
	{
		return s_Renderer2DData.Limits;
	}
}
