#include "Renderer2D.h"

#include "GrappleCore/Core.h"
#include "GrappleCore/Profiler/Profiler.h"

#include "Grapple/Math/Math.h"

#include "Grapple/AssetManager/AssetManager.h"

#include "Grapple/Renderer/Viewport.h"
#include "Grapple/Renderer/Renderer.h"
#include "Grapple/Renderer/ShaderLibrary.h"
#include "Grapple/Renderer/Pipeline.h"

#include "Grapple/Project/Project.h"

#include "Grapple/Platform/Vulkan/VulkanPipeline.h"
#include "Grapple/Platform/Vulkan/VulkanContext.h"
#include "Grapple/Platform/Vulkan/VulkanDescriptorSet.h"

#include <algorithm>
#include <cctype>

namespace Grapple
{
	constexpr uint32_t MaxTexturesCount = 32;

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 UV;
		int32_t TextuteIndex;
		int32_t EntityIndex;
	};

	struct TextVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 UV;
		int32_t EntityIndex;
	};

	struct QuadsBatch
	{
		inline uint32_t GetEnd() const { return Start + Count; }

		uint32_t GetTextureIndex(const Ref<Texture>& texture)
		{
			Grapple_CORE_ASSERT(TexturesCount < MaxTexturesCount);
			Grapple_CORE_ASSERT(texture);

			for (uint32_t i = 0; i < TexturesCount; i++)
			{
				if (texture.get() == Textures[i].get())
				{
					return i;
				}
			}

			TexturesCount++;

			Textures[TexturesCount - 1] = texture;
			return TexturesCount - 1;
		}

		Ref<Material> Material = nullptr;
		Ref<Texture> Textures[MaxTexturesCount] = { nullptr };
		uint32_t TexturesCount = 0;
		uint32_t Start = 0;
		uint32_t Count = 0;
	};

	struct TextBatch
	{
		inline size_t GetEnd() const { return Start + Count; }

		Ref<const Font> Font = nullptr;
		uint32_t Start = 0;
		uint32_t Count = 0;
	};

	struct Renderer2DData
	{
		Renderer2DStats Stats;

		size_t QuadIndex = 0;
		size_t MaxQuadCount = 0;

		std::vector<QuadVertex> Vertices;

		Ref<VertexBuffer> QuadsVertexBuffer = nullptr;
		Ref<IndexBuffer> IndexBuffer = nullptr;

		std::vector<TextVertex> TextVertices;
		Ref<VertexBuffer> TextVertexBuffer = nullptr;

		size_t TextQuadIndex = 0;

		Ref<Material> DefaultMaterial = nullptr;
		Ref<Material> TextMaterial = nullptr;
		std::optional<uint32_t> FontAtlasPropertyIndex = {};
		Ref<Material> CurrentMaterial = nullptr;

		glm::vec3 QuadVertices[4] = { glm::vec3(0.0f) };
		glm::vec2 QuadUV[4] = { glm::vec2(0.0f) };

		Ref<Font> CurrentFont = nullptr;

		RenderData* FrameData = nullptr;

		std::vector<QuadsBatch> QuadBatches;
		std::vector<Ref<DescriptorSet>> UsedQuadsDescriptorSets;
		Ref<DescriptorSetPool> QuadsDescriptorPool = nullptr;

		Ref<Pipeline> TextPipeline = nullptr;
		Ref<DescriptorSet> TextDescriptorSet = nullptr;
		Ref<DescriptorSetPool> TextDescriptorPool = nullptr;
	};

	Renderer2DData s_Renderer2DData;

	static void ReloadShaders();

	void Renderer2D::Initialize(size_t maxQuads)
	{
		s_Renderer2DData.QuadIndex = 0;
		s_Renderer2DData.MaxQuadCount = maxQuads;
		s_Renderer2DData.Vertices.resize(maxQuads * 4);
		s_Renderer2DData.TextVertices.resize(maxQuads * 4);

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

		s_Renderer2DData.QuadsVertexBuffer = VertexBuffer::Create(maxQuads * 4 * sizeof(QuadVertex));
		s_Renderer2DData.IndexBuffer = IndexBuffer::Create(IndexBuffer::IndexFormat::UInt32, MemorySpan::FromVector(indices));

		// Text
		s_Renderer2DData.TextVertexBuffer = VertexBuffer::Create(maxQuads * 4 * sizeof(TextVertex));

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
				bindings[0].descriptorCount = (uint32_t)MaxTexturesCount;
				bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				bindings[0].pImmutableSamplers = nullptr;

				s_Renderer2DData.QuadsDescriptorPool = CreateRef<VulkanDescriptorSetPool>(32, Span(bindings, 1));
			}

			{
				VkDescriptorSetLayoutBinding bindings[1] = {};
				bindings[0].binding = 0;
				bindings[0].descriptorCount = 1;
				bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				bindings[0].pImmutableSamplers = nullptr;

				s_Renderer2DData.TextDescriptorPool = CreateRef<VulkanDescriptorSetPool>(1, Span(bindings, 1));
				s_Renderer2DData.TextDescriptorSet = s_Renderer2DData.TextDescriptorPool->AllocateSet();

				s_Renderer2DData.TextDescriptorSet->SetDebugName("TextDescriptorSet");
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
			Ref<Shader> textShader = AssetManager::GetAsset<Shader>(textShaderHandle.value());
			s_Renderer2DData.TextMaterial = Material::Create(textShader);
			s_Renderer2DData.FontAtlasPropertyIndex = textShader->GetPropertyIndex("u_MSDF");
		}
	}

	void Renderer2D::Shutdown()
	{
		for (Ref<DescriptorSet> set : s_Renderer2DData.UsedQuadsDescriptorSets)
			s_Renderer2DData.QuadsDescriptorPool->ReleaseSet(set);
		s_Renderer2DData.UsedQuadsDescriptorSets.clear();

		s_Renderer2DData = {};
	}

	void Renderer2D::BeginFrame()
	{
		Grapple_PROFILE_FUNCTION();
		for (Ref<DescriptorSet> set : s_Renderer2DData.UsedQuadsDescriptorSets)
			s_Renderer2DData.QuadsDescriptorPool->ReleaseSet(set);
		s_Renderer2DData.UsedQuadsDescriptorSets.clear();
	}

	void Renderer2D::EndFrame()
	{

	}

	void Renderer2D::Begin(const Ref<Material>& material)
	{
		if (s_Renderer2DData.QuadIndex > 0)
			FlushAll();

		s_Renderer2DData.FrameData = &Renderer::GetCurrentViewport().FrameData;
		s_Renderer2DData.CurrentMaterial = material == nullptr ? s_Renderer2DData.DefaultMaterial : material;
	}

	void Renderer2D::End()
	{
		FlushAll();

		s_Renderer2DData.CurrentFont = nullptr;
		s_Renderer2DData.CurrentMaterial = nullptr;

		s_Renderer2DData.QuadBatches.clear();
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
		Grapple_CORE_ASSERT(s_Renderer2DData.QuadBatches.size() > 0);
		Grapple_CORE_ASSERT(material);

		size_t lastBatchIndex = s_Renderer2DData.QuadBatches.size() - 1;
		QuadsBatch& batch = s_Renderer2DData.QuadBatches.emplace_back();
		batch.Material = material;
		batch.Count = 0;

		if (s_Renderer2DData.QuadBatches.size() > 0)
			batch.Start = s_Renderer2DData.QuadBatches[lastBatchIndex].GetEnd();

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
		if (s_Renderer2DData.QuadBatches.size() == 0)
		{
			QuadsBatch& batch = s_Renderer2DData.QuadBatches.emplace_back();
			batch.Material = s_Renderer2DData.DefaultMaterial;
		}

		if (s_Renderer2DData.QuadIndex >= s_Renderer2DData.MaxQuadCount)
		{
			Ref<Material> lastUsedMaterial = s_Renderer2DData.QuadBatches.back().Material;
			FlushQuadBatches();

			QuadsBatch& batch = s_Renderer2DData.QuadBatches.emplace_back();
			batch.Material = lastUsedMaterial;
		}

		if (s_Renderer2DData.QuadBatches.back().TexturesCount == MaxTexturesCount)
		{
			s_Renderer2DData.QuadBatches.emplace_back();
		}

		QuadsBatch& currentBatch = s_Renderer2DData.QuadBatches.back();
		size_t vertexIndex = s_Renderer2DData.QuadIndex * 4;
		uint32_t textureIndex = currentBatch.GetTextureIndex(texture == nullptr ? Renderer::GetWhiteTexture() : texture);

		for (uint32_t i = 0; i < 4; i++)
		{
			QuadVertex& vertex = s_Renderer2DData.Vertices[vertexIndex + i];
			vertex.Position = vertices[i];
			vertex.Color = tint;
			vertex.UV = uv[i] * tiling;
			vertex.TextuteIndex = textureIndex;
			vertex.EntityIndex = entityIndex;
		}

		currentBatch.Count++;
		s_Renderer2DData.QuadIndex++;
		s_Renderer2DData.Stats.QuadsCount++;
	}

	void Renderer2D::DrawString(std::string_view text, const glm::mat4& transform, const Ref<Font>& font, const glm::vec4& color, int32_t entityIndex)
	{
		Grapple_PROFILE_FUNCTION();

		if (s_Renderer2DData.CurrentFont.get() != font.get())
		{
			FlushText();
			s_Renderer2DData.CurrentFont = font;
		}

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

				{
					TextVertex& vertex = s_Renderer2DData.TextVertices[s_Renderer2DData.TextQuadIndex * 4 + 0];
					vertex.Position = transform * glm::vec4(min, 0.0f, 1.0f);
					vertex.Color = color;
					vertex.UV = glm::vec2((float)atlasBounds.Left, (float)atlasBounds.Bottom);
					vertex.EntityIndex = entityIndex;
				}

				{
					TextVertex& vertex = s_Renderer2DData.TextVertices[s_Renderer2DData.TextQuadIndex * 4 + 1];
					vertex.Position = transform * glm::vec4(min.x, max.y, 0.0f, 1.0f);
					vertex.Color = color;
					vertex.UV = glm::vec2((float)atlasBounds.Left, (float)atlasBounds.Top);
					vertex.EntityIndex = entityIndex;
				}

				{
					TextVertex& vertex = s_Renderer2DData.TextVertices[s_Renderer2DData.TextQuadIndex * 4 + 2];
					vertex.Position = transform * glm::vec4(max, 0.0f, 1.0f);
					vertex.Color = color;
					vertex.UV = glm::vec2((float)atlasBounds.Right, (float)atlasBounds.Top);
					vertex.EntityIndex = entityIndex;
				}

				{
					TextVertex& vertex = s_Renderer2DData.TextVertices[s_Renderer2DData.TextQuadIndex * 4 + 3];
					vertex.Position = transform * glm::vec4(max.x, min.y, 0.0f, 1.0f);
					vertex.Color = color;
					vertex.UV = glm::vec2((float)atlasBounds.Right, (float)atlasBounds.Bottom);
					vertex.EntityIndex = entityIndex;
				}

				s_Renderer2DData.TextQuadIndex++;
				s_Renderer2DData.Stats.QuadsCount++;

				if (s_Renderer2DData.TextQuadIndex >= s_Renderer2DData.MaxQuadCount)
					FlushText();
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
		return s_Renderer2DData.QuadsDescriptorPool->GetLayout();
	}

	static void FlushQuadsBatch(QuadsBatch& batch)
	{
		Grapple_PROFILE_FUNCTION();

		for (uint32_t i = batch.TexturesCount; i < MaxTexturesCount; i++)
			batch.Textures[i] = Renderer::GetWhiteTexture();

		if (RendererAPI::GetAPI() == RendererAPI::API::Vulkan)
		{
			Ref<VulkanCommandBuffer> commandBuffer = VulkanContext::GetInstance().GetPrimaryCommandBuffer();
			Ref<VulkanFrameBuffer> renderTarget = As<VulkanFrameBuffer>(Renderer::GetCurrentViewport().RenderTarget);

			Ref<DescriptorSet> descriptorSet = s_Renderer2DData.QuadsDescriptorPool->AllocateSet();
			descriptorSet->SetDebugName("QuadsDescriptorSet");

			s_Renderer2DData.UsedQuadsDescriptorSets.push_back(descriptorSet);

			descriptorSet->WriteImages(Span((Ref<const Texture>*)batch.Textures, MaxTexturesCount), 0, 0);
			descriptorSet->FlushWrites();

			commandBuffer->SetSecondaryDescriptorSet(descriptorSet);

			commandBuffer->ApplyMaterial(batch.Material);
			commandBuffer->SetViewportAndScisors(Math::Rect(glm::vec2(0.0f), (glm::vec2)renderTarget->GetSize()));
			commandBuffer->BindVertexBuffers(Span((Ref<const VertexBuffer>)s_Renderer2DData.QuadsVertexBuffer));
			commandBuffer->BindIndexBuffer(s_Renderer2DData.IndexBuffer);
			commandBuffer->DrawIndexed(batch.Start * 6, batch.Count * 6);

			s_Renderer2DData.Stats.DrawCalls++;
		}
	}

	void Renderer2D::FlushQuadBatches()
	{
		Grapple_PROFILE_FUNCTION();
		if (s_Renderer2DData.QuadIndex == 0)
			return;

		if (s_Renderer2DData.DefaultMaterial == nullptr)
		{
			s_Renderer2DData.QuadIndex = 0;
			return;
		}

		{
			Grapple_PROFILE_SCOPE("Renderer2D::UpdateQuadsVertexBuffer");
			s_Renderer2DData.QuadsVertexBuffer->SetData(s_Renderer2DData.Vertices.data(), sizeof(QuadVertex) * s_Renderer2DData.QuadIndex * 4);
		}

		for (auto& batch : s_Renderer2DData.QuadBatches)
		{
			if (batch.Count == 0)
				continue;

			FlushQuadsBatch(batch);
		}

		s_Renderer2DData.QuadIndex = 0;
	}

	void Renderer2D::FlushText()
	{
		Grapple_PROFILE_FUNCTION();

		if (s_Renderer2DData.TextQuadIndex == 0)
			return;

		if (s_Renderer2DData.TextMaterial == nullptr || !s_Renderer2DData.FontAtlasPropertyIndex.has_value())
		{
			s_Renderer2DData.TextQuadIndex = 0;
			return;
		}

		if (!s_Renderer2DData.CurrentFont)
			s_Renderer2DData.CurrentFont = Font::GetDefault();

		if (RendererAPI::GetAPI() == RendererAPI::API::Vulkan)
		{
			{
				Grapple_PROFILE_SCOPE("UploadVertexData");
				s_Renderer2DData.TextVertexBuffer->SetData(s_Renderer2DData.TextVertices.data(), s_Renderer2DData.TextQuadIndex * sizeof(TextVertex) * 4);
			}

			if (s_Renderer2DData.TextPipeline == nullptr)
			{
				PipelineSpecifications specificaionts{};
				specificaionts.Shader = s_Renderer2DData.TextMaterial->GetShader();
				specificaionts.Culling = CullingMode::Back;
				specificaionts.DepthTest = true;
				specificaionts.DepthWrite = true;
				specificaionts.InputLayout = PipelineInputLayout({
					{ 0, 0, ShaderDataType::Float3 }, // Position
					{ 0, 1, ShaderDataType::Float4 }, // COlor
					{ 0, 2, ShaderDataType::Float2 }, // UV
					{ 0, 3, ShaderDataType::Int }, // Entity index
					});

				Ref<const DescriptorSetLayout> layouts[] = { Renderer::GetPrimaryDescriptorSetLayout(), s_Renderer2DData.TextDescriptorPool->GetLayout() };

				Ref<VulkanFrameBuffer> renderTarget = As<VulkanFrameBuffer>(Renderer::GetMainViewport().RenderTarget);
				s_Renderer2DData.TextPipeline = CreateRef<VulkanPipeline>(specificaionts,
					renderTarget->GetCompatibleRenderPass(),
					Span<Ref<const DescriptorSetLayout>>(layouts, 2),
					Span<ShaderPushConstantsRange>());
			}

			Ref<VulkanCommandBuffer> commandBuffer = VulkanContext::GetInstance().GetPrimaryCommandBuffer();
			Ref<VulkanFrameBuffer> renderTarget = As<VulkanFrameBuffer>(Renderer::GetCurrentViewport().RenderTarget);

			s_Renderer2DData.TextDescriptorSet->WriteImage(s_Renderer2DData.CurrentFont->GetAtlas(), 0);
			s_Renderer2DData.TextDescriptorSet->FlushWrites();

			VkPipelineLayout pipelineLayout = As<const VulkanPipeline>(s_Renderer2DData.TextPipeline)->GetLayoutHandle();

			commandBuffer->BindDescriptorSet(As<VulkanDescriptorSet>(Renderer::GetPrimaryDescriptorSet()), pipelineLayout, 0);
			commandBuffer->BindDescriptorSet(As<VulkanDescriptorSet>(s_Renderer2DData.TextDescriptorSet), pipelineLayout, 1);

			commandBuffer->BindPipeline(s_Renderer2DData.TextPipeline);
			commandBuffer->SetViewportAndScisors(Math::Rect(glm::vec2(0.0f), (glm::vec2)renderTarget->GetSize()));
			commandBuffer->BindVertexBuffers(Span((Ref<const VertexBuffer>)s_Renderer2DData.TextVertexBuffer));
			commandBuffer->BindIndexBuffer(s_Renderer2DData.IndexBuffer);
			commandBuffer->DrawIndexed((uint32_t)(s_Renderer2DData.TextQuadIndex * 6));
		}

		s_Renderer2DData.TextQuadIndex = 0;
	}

	void Renderer2D::FlushAll()
	{
		Grapple_PROFILE_FUNCTION();

		if (RendererAPI::GetAPI() == RendererAPI::API::Vulkan)
		{
			Ref<VulkanCommandBuffer> commandBuffer = VulkanContext::GetInstance().GetPrimaryCommandBuffer();
			Ref<VulkanFrameBuffer> renderTarget = As<VulkanFrameBuffer>(Renderer::GetCurrentViewport().RenderTarget);

			commandBuffer->BeginRenderPass(renderTarget->GetCompatibleRenderPass(), renderTarget);

			commandBuffer->SetPrimaryDescriptorSet(Renderer::GetPrimaryDescriptorSet());

			FlushQuadBatches();
			FlushText();

			commandBuffer->EndRenderPass();
			return;
		}

		FlushQuadBatches();
		FlushText();
	}
}
