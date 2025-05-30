#include "DebugRenderer.h"

#include "GrappleCore/Assert.h"

#include "Grapple/AssetManager/AssetManager.h"

#include "Grapple/Renderer/Renderer.h"
#include "Grapple/Renderer/VertexArray.h"
#include "Grapple/Renderer/Buffer.h"
#include "Grapple/Renderer/RenderCommand.h"
#include "Grapple/Renderer/ShaderLibrary.h"
#include "Grapple/Renderer/Shader.h"

#include "Grapple/Project/Project.h"

namespace Grapple
{
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
	};

	struct DebugRendererData
	{
		const RenderData* FrameData;

		Vertex* LinesBufferBase;
		Vertex* LinesBuffer;

		Vertex* RaysBufferBase;
		Vertex* RaysBuffer;

		uint32_t LinesCount;
		uint32_t MaxLinesCount;

		uint32_t RaysCount;
		uint32_t MaxRaysCount;

		float RayThickness;

		Ref<Shader> DebugShader;
		Ref<VertexBuffer> LinesVertexBuffer;
		Ref<VertexArray> LinesMesh;

		Ref<VertexBuffer> RayVertexBuffer;
		Ref<VertexArray> RaysMesh;
	};

	DebugRendererData s_DebugRendererData;

	constexpr uint32_t VerticesPerLine = 2;
	constexpr uint32_t VerticesPerRay = 7;
	constexpr uint32_t IndicesPerRay = 9;

	static void ReloadShader()
	{
		s_DebugRendererData.DebugShader = nullptr;

		AssetHandle debugShaderHandle = ShaderLibrary::FindShader("Debug").value_or(NULL_ASSET_HANDLE);
		if (AssetManager::IsAssetHandleValid(debugShaderHandle))
			s_DebugRendererData.DebugShader = AssetManager::GetAsset<Shader>(debugShaderHandle);
	}

	void DebugRenderer::Initialize(uint32_t maxLinesCount)
	{
		s_DebugRendererData.MaxRaysCount = 10000;
		s_DebugRendererData.RayThickness = 0.07f;

		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(sizeof(Vertex) * VerticesPerLine * maxLinesCount);
		vertexBuffer->SetLayout({
			{ "i_Position", ShaderDataType::Float3, false },
			{ "i_Color", ShaderDataType::Float4, false },
		});

		Ref<VertexArray> mesh = VertexArray::Create();
		mesh->AddVertexBuffer(vertexBuffer);
		mesh->Unbind();

		s_DebugRendererData.LinesVertexBuffer = vertexBuffer;
		s_DebugRendererData.LinesMesh = mesh;

		Ref<VertexBuffer> rayVertexBuffer = VertexBuffer::Create(sizeof(Vertex) * VerticesPerRay * s_DebugRendererData.MaxRaysCount);
		rayVertexBuffer->SetLayout({
			{ "i_Position", ShaderDataType::Float3, false },
			{ "i_Color", ShaderDataType::Float4, false },
		});
		Ref<VertexArray> raysMesh = VertexArray::Create();
		raysMesh->AddVertexBuffer(rayVertexBuffer);

		uint32_t* indices = new uint32_t[s_DebugRendererData.MaxRaysCount * IndicesPerRay];
		uint32_t vertexIndex = 0;

		for (uint32_t index = 0; index < s_DebugRendererData.MaxRaysCount * IndicesPerRay; index += IndicesPerRay)
		{
			indices[index + 0] = vertexIndex + 0;
			indices[index + 1] = vertexIndex + 1;
			indices[index + 2] = vertexIndex + 2;
			indices[index + 3] = vertexIndex + 0;
			indices[index + 4] = vertexIndex + 2;
			indices[index + 5] = vertexIndex + 3;

			indices[index + 6] = vertexIndex + 4;
			indices[index + 7] = vertexIndex + 5;
			indices[index + 8] = vertexIndex + 6;

			vertexIndex += VerticesPerRay;
		}

		raysMesh->SetIndexBuffer(IndexBuffer::Create(IndexBuffer::IndexFormat::UInt32, MemorySpan(indices, s_DebugRendererData.MaxRaysCount * IndicesPerRay)));
		raysMesh->Unbind();

		s_DebugRendererData.RayVertexBuffer = rayVertexBuffer;
		s_DebugRendererData.RaysMesh = raysMesh;

		delete[] indices;

		s_DebugRendererData.FrameData = nullptr;

		s_DebugRendererData.RaysCount = 0;
		s_DebugRendererData.LinesCount = 0;

		s_DebugRendererData.MaxLinesCount = maxLinesCount;
		s_DebugRendererData.LinesBufferBase = new Vertex[s_DebugRendererData.MaxLinesCount * VerticesPerLine];
		s_DebugRendererData.LinesBuffer = s_DebugRendererData.LinesBufferBase;

		s_DebugRendererData.RaysBufferBase = new Vertex[s_DebugRendererData.MaxRaysCount * VerticesPerRay];
		s_DebugRendererData.RaysBuffer = s_DebugRendererData.RaysBufferBase;

		Project::OnProjectOpen.Bind(ReloadShader);
	}

	void DebugRenderer::Shutdown()
	{
		Grapple_CORE_ASSERT(s_DebugRendererData.LinesBufferBase);

		delete[] s_DebugRendererData.LinesBufferBase;

		s_DebugRendererData.LinesBufferBase = nullptr;
		s_DebugRendererData.LinesBuffer = nullptr;
	}

	void DebugRenderer::Begin()
	{
		Grapple_CORE_ASSERT(!s_DebugRendererData.FrameData);
		s_DebugRendererData.FrameData = &Renderer::GetCurrentViewport().FrameData;
	}

	void DebugRenderer::End()
	{
		FlushLines();
		FlushRays();
		s_DebugRendererData.FrameData = nullptr;
	}

	void DebugRenderer::DrawLine(const glm::vec3& start, const glm::vec3& end)
	{
		DrawLine(start, end, glm::vec4(1.0f), glm::vec4(1.0f));
	}

	void DebugRenderer::DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color)
	{
		DrawLine(start, end, color, color);
	}

	void DebugRenderer::DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& startColor, const glm::vec4& endColor)
	{
		s_DebugRendererData.LinesBuffer->Position = start;
		s_DebugRendererData.LinesBuffer->Color = startColor;
		s_DebugRendererData.LinesBuffer++;

		s_DebugRendererData.LinesBuffer->Position = end;
		s_DebugRendererData.LinesBuffer->Color = endColor;
		s_DebugRendererData.LinesBuffer++;

		s_DebugRendererData.LinesCount++;

		if (s_DebugRendererData.LinesCount == s_DebugRendererData.MaxLinesCount)
			FlushLines();
	}

	void DebugRenderer::DrawRay(const glm::vec3& origin, const glm::vec3& direction, const glm::vec4& color)
	{
		Grapple_CORE_ASSERT(s_DebugRendererData.FrameData);

		glm::vec3 fromCameraDirection = s_DebugRendererData.FrameData->Camera.Position - origin;
		glm::vec3 up = glm::normalize(glm::cross(fromCameraDirection, direction)) * s_DebugRendererData.RayThickness / 2.0f;

		for (uint32_t i = 0; i < VerticesPerRay; i++)
			s_DebugRendererData.RaysBuffer[i].Color = color;

		glm::vec3 end = origin + direction;

		float triangleHeight = glm::sqrt(3.0f) * s_DebugRendererData.RayThickness;
		float arrowScale = glm::clamp(glm::length(fromCameraDirection), 1.0f, 1.5f);

		up *= arrowScale;
		triangleHeight *= arrowScale;

		s_DebugRendererData.RaysBuffer[0].Position = origin - up;
		s_DebugRendererData.RaysBuffer[1].Position = origin + up;
		s_DebugRendererData.RaysBuffer[2].Position = end + up;
		s_DebugRendererData.RaysBuffer[3].Position = end - up;

		s_DebugRendererData.RaysBuffer[4].Position = end + up * 2.0f;
		s_DebugRendererData.RaysBuffer[5].Position = end + glm::normalize(direction) * triangleHeight;
		s_DebugRendererData.RaysBuffer[6].Position = end - up * 2.0f;

		s_DebugRendererData.RaysCount++;
		s_DebugRendererData.RaysBuffer += VerticesPerRay;
		
		if (s_DebugRendererData.RaysCount == s_DebugRendererData.MaxRaysCount)
			FlushRays();
	}

	void DebugRenderer::DrawCircle(const glm::vec3& position, const glm::vec3& normal, const glm::vec3& tangent, float radius, const glm::vec4& color, uint32_t segments)
	{
		glm::vec3 bitangent = glm::cross(tangent, normal);

		float segmentAngle = glm::pi<float>() * 2.0f / (float)segments;
		float currentAngle = 0.0f;

		glm::vec3 previousPoint(0.0f);
		for (uint32_t i = 0; i <= segments; i++)
		{
			glm::vec2 point = glm::vec2(glm::cos(currentAngle), glm::sin(currentAngle)) * radius;
			glm::vec3 worldSpacePoint = position + tangent * point.x + bitangent * point.y;

			if (i > 0)
			{
				DrawLine(previousPoint, worldSpacePoint, color);
			}

			previousPoint = worldSpacePoint;
			currentAngle += segmentAngle;
		}
	}

	void DebugRenderer::DrawWireQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		glm::vec3 halfSize = glm::vec3(size / 2.0f, 0.0f);
		DrawLine(position + glm::vec3(-halfSize.x, halfSize.y, 0.0f), position + halfSize, color);
		DrawLine(position + glm::vec3(-halfSize.x, -halfSize.y, 0.0f), position + glm::vec3(halfSize.x, -halfSize.y, 0.0f), color);
		DrawLine(position + glm::vec3(-halfSize.x, halfSize.y, 0.0f), position + glm::vec3(-halfSize.x, -halfSize.y, 0.0f), color);
		DrawLine(position + halfSize, position + glm::vec3(halfSize.x, -halfSize.y, 0.0f), color);
	}

	void DebugRenderer::DrawWireQuad(const glm::vec3* corners, const glm::vec4& color)
	{
		DrawLine(corners[0], corners[1], color);
		DrawLine(corners[1], corners[2], color);
		DrawLine(corners[2], corners[3], color);
		DrawLine(corners[3], corners[0], color);
	}

	void DebugRenderer::DrawFrustum(const glm::mat4& inverseViewProjection, const glm::vec4& color)
	{
		std::array<glm::vec4, 8> frustumCorners =
		{
			glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f),
			glm::vec4(1.0f, -1.0f, 0.0f, 1.0f),
			glm::vec4(-1.0f,  1.0f, 0.0f, 1.0f),
			glm::vec4(1.0f,  1.0f, 0.0f, 1.0f),
			glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f),
			glm::vec4(1.0f, -1.0f, 1.0f, 1.0f),
			glm::vec4(-1.0f,  1.0f, 1.0f, 1.0f),
			glm::vec4(1.0f,  1.0f, 1.0f, 1.0f),
		};

		for (size_t i = 0; i < frustumCorners.size(); i++)
		{
			frustumCorners[i] = inverseViewProjection * frustumCorners[i];
			frustumCorners[i] /= frustumCorners[i].w;
		}

		for (size_t i = 0; i < 8; i += 4)
		{
			DrawLine(frustumCorners[i + 0], frustumCorners[i + 1], color);
			DrawLine(frustumCorners[i + 0], frustumCorners[i + 2], color);
			DrawLine(frustumCorners[i + 2], frustumCorners[i + 3], color);
			DrawLine(frustumCorners[i + 1], frustumCorners[i + 3], color);
		}

		for (size_t i = 0; i < 4; i++)
			DrawLine(frustumCorners[i], frustumCorners[i + 4], color);
	}

	void DebugRenderer::FlushLines()
	{
		Grapple_CORE_ASSERT(s_DebugRendererData.FrameData);

		if (s_DebugRendererData.DebugShader)
		{
			s_DebugRendererData.LinesVertexBuffer->SetData(s_DebugRendererData.LinesBufferBase, sizeof(Vertex) * VerticesPerLine * s_DebugRendererData.LinesCount);
			s_DebugRendererData.DebugShader->Bind();
			
			RenderCommand::SetDepthTestEnabled(true);
			RenderCommand::DrawLines(s_DebugRendererData.LinesMesh, s_DebugRendererData.LinesCount * VerticesPerLine);
		}

		s_DebugRendererData.LinesCount = 0;
		s_DebugRendererData.LinesBuffer = s_DebugRendererData.LinesBufferBase;
	}

	void DebugRenderer::DrawWireBox(const glm::vec3 corners[8], const glm::vec4& color)
	{
		for (size_t i = 0; i < 8; i += 4)
		{
			DrawLine(corners[i], corners[i + 1], color);
			DrawLine(corners[i], corners[i + 2], color);
			DrawLine(corners[i + 1], corners[i + 3], color);
			DrawLine(corners[i + 2], corners[i + 3], color);
		}

		for (size_t i = 0; i < 4; i++)
		{
			DrawLine(corners[i], corners[i + 4], color);
		}
	}

	void DebugRenderer::DrawAABB(const Math::AABB& aabb, const glm::vec4& color)
	{
		glm::vec3 corners[8];
		aabb.GetCorners(corners);

		DrawWireBox(corners, color);
	}

	void DebugRenderer::FlushRays()
	{
		Grapple_CORE_ASSERT(s_DebugRendererData.FrameData);

		if (s_DebugRendererData.DebugShader)
		{
			s_DebugRendererData.RayVertexBuffer->SetData(s_DebugRendererData.RaysBufferBase, sizeof(Vertex) * VerticesPerRay * s_DebugRendererData.RaysCount);
			s_DebugRendererData.DebugShader->Bind();

			RenderCommand::SetDepthTestEnabled(true);
			RenderCommand::DrawIndexed(s_DebugRendererData.RaysMesh, s_DebugRendererData.RaysCount * IndicesPerRay);
		}

		s_DebugRendererData.RaysCount = 0;
		s_DebugRendererData.RaysBuffer = s_DebugRendererData.RaysBufferBase;
	}
}
