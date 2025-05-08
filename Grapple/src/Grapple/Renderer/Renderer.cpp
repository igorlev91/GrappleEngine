#include "Renderer.h"

#include "Grapple/AssetManager/AssetManager.h"

#include "Grapple/Renderer/UniformBuffer.h"
#include "Grapple/Renderer/ShaderLibrary.h"
#include "Grapple/Renderer2D/Renderer2D.h"
#include "Grapple/Renderer/Renderer.h"

#include "Grapple/Project/Project.h"

namespace Grapple
{
	struct InstanceData
	{
		glm::mat4 Transform;
		int32_t EntityIndex;
	};

	struct RenderableObject
	{
		Ref<Mesh> Mesh;
		Ref<Material> Material;
		glm::mat4 Transform;
		MeshRenderFlags Flags;
		int32_t EntityIndex;
	};

	struct ShadowData
	{
		float Bias;
		float FrustumSize;
		float LightSize;

		float Padding0;

		float CascadeSplits[4];

		glm::mat4 LightProjection[4];
	};

	struct RendererData
	{
		Viewport* MainViewport = nullptr;
		Viewport* CurrentViewport = nullptr;

		Ref<UniformBuffer> CameraBuffer = nullptr;
		Ref<UniformBuffer> LightBuffer = nullptr;
		Ref<VertexArray> FullscreenQuad = nullptr;

		Ref<Texture> WhiteTexture = nullptr;
		
		std::vector<Ref<RenderPass>> RenderPasses;
		RendererStatistics Statistics;

		std::vector<RenderableObject> Queue;

		Ref<VertexBuffer> InstanceBuffer = nullptr;
		std::vector<InstanceData> InstanceDataBuffer;
		uint32_t MaxInstances = 1024;

		Ref<Mesh> CurrentInstancingMesh = nullptr;
		Ref<FrameBuffer> ShadowsRenderTarget[4] = { nullptr };

		Ref<Material> ErrorMaterial = nullptr;

		ShadowSettings ShadowMappingSettings;
		Ref<UniformBuffer> ShadowDataBuffer = nullptr;
	};
	
	RendererData s_RendererData;

	static void ReloadShaders()
	{
		std::optional<AssetHandle> errorShaderHandle = ShaderLibrary::FindShader("Error");
		if (errorShaderHandle && AssetManager::IsAssetHandleValid(*errorShaderHandle))
			s_RendererData.ErrorMaterial = CreateRef<Material>(AssetManager::GetAsset<Shader>(*errorShaderHandle));
		else
			Grapple_CORE_ERROR("Renderer: Failed to find Error shader");
	}

	void Renderer::Initialize()
	{
		s_RendererData.CameraBuffer = UniformBuffer::Create(sizeof(CameraData), 0);
		s_RendererData.LightBuffer = UniformBuffer::Create(sizeof(LightData), 1);
		s_RendererData.ShadowDataBuffer = UniformBuffer::Create(sizeof(ShadowData), 2);

		s_RendererData.ShadowMappingSettings.MaxDistance = 40.0f;
		s_RendererData.ShadowMappingSettings.Resolution = 2048;
		s_RendererData.ShadowMappingSettings.Bias = 0.001f;
		s_RendererData.ShadowMappingSettings.LightSize = 0.02f;

		s_RendererData.ShadowMappingSettings.CascadeSplits[0] = 15.0f;
		s_RendererData.ShadowMappingSettings.CascadeSplits[1] = 45.0f;
		s_RendererData.ShadowMappingSettings.CascadeSplits[2] = 75.0f;
		s_RendererData.ShadowMappingSettings.CascadeSplits[3] = 200.0f;

		float vertices[] = {
			-1, -1,
			-1,  1,
			 1,  1,
			 1, -1,
		};

		uint32_t indices[] = {
			0, 1, 2,
			0, 2, 3,
		};

		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(sizeof(vertices), (const void*)vertices);
		vertexBuffer->SetLayout({
			BufferLayoutElement("i_Position", ShaderDataType::Float2),
		});

		s_RendererData.FullscreenQuad = VertexArray::Create();
		s_RendererData.FullscreenQuad->SetIndexBuffer(IndexBuffer::Create(6, (const void*)indices));
		s_RendererData.FullscreenQuad->AddVertexBuffer(vertexBuffer);
		s_RendererData.FullscreenQuad->Unbind();

		{
			uint32_t whiteTextureData = 0xffffffff;
			s_RendererData.WhiteTexture = Texture::Create(1, 1, &whiteTextureData, TextureFormat::RGBA8);
		}

		s_RendererData.InstanceBuffer = VertexBuffer::Create(s_RendererData.MaxInstances * sizeof(InstanceData));
		s_RendererData.InstanceBuffer->SetLayout({
			{ "i_Transform", ShaderDataType::Matrix4x4 },
			{ "i_EntityIndex", ShaderDataType::Int },
		});

		Project::OnProjectOpen.Bind(ReloadShaders);
	}

	void Renderer::Shutdown()
	{
		s_RendererData.FullscreenQuad = nullptr;
	}

	const RendererStatistics& Renderer::GetStatistics()
	{
		return s_RendererData.Statistics;
	}

	void Renderer::ClearStatistics()
	{
		s_RendererData.Statistics.DrawCallsCount = 0;
		s_RendererData.Statistics.DrawCallsSavedByInstances = 0;
	}

	void Renderer::SetMainViewport(Viewport& viewport)
	{
		s_RendererData.MainViewport = &viewport;
	}

	void Renderer::BeginScene(Viewport& viewport)
	{
		s_RendererData.CurrentViewport = &viewport;

		s_RendererData.LightBuffer->SetData(&viewport.FrameData.Light, sizeof(viewport.FrameData.Light), 0);
		s_RendererData.CameraBuffer->SetData(&viewport.FrameData.Camera, sizeof(CameraData), 0);

		ShadowData shadowData;
		shadowData.Bias = s_RendererData.ShadowMappingSettings.Bias;
		shadowData.LightSize = s_RendererData.ShadowMappingSettings.LightSize;

		for (size_t i = 0; i < 4; i++)
			shadowData.CascadeSplits[i] = s_RendererData.ShadowMappingSettings.CascadeSplits[i];

		for (size_t i = 0; i < 4; i++)
			shadowData.LightProjection[i] = viewport.FrameData.LightView[i].ViewProjection;
			
		shadowData.FrustumSize = 2.0f * s_RendererData.CurrentViewport->FrameData.Camera.Near
			* glm::tan(glm::radians(s_RendererData.CurrentViewport->FrameData.Camera.FOV / 2.0f))
			* s_RendererData.CurrentViewport->GetAspectRatio();

		s_RendererData.ShadowDataBuffer->SetData(&shadowData, sizeof(shadowData), 0);

		if (s_RendererData.ShadowsRenderTarget[0] == nullptr)
		{
			FrameBufferSpecifications shadowMapSpecs;
			shadowMapSpecs.Width = s_RendererData.ShadowMappingSettings.Resolution;
			shadowMapSpecs.Height = s_RendererData.ShadowMappingSettings.Resolution;
			shadowMapSpecs.Attachments = { { FrameBufferTextureFormat::Depth, TextureWrap::Clamp, TextureFiltering::Linear } };

			for (size_t i = 0; i < 2; i++)
			{
				s_RendererData.ShadowsRenderTarget[i] = FrameBuffer::Create(shadowMapSpecs);
			}

			shadowMapSpecs.Width /= 2;
			shadowMapSpecs.Height /= 2;

			for (size_t i = 2; i < 4; i++)
			{
				s_RendererData.ShadowsRenderTarget[i] = FrameBuffer::Create(shadowMapSpecs);
			}
		}
		else
		{
			auto& shadowMapSpecs = s_RendererData.ShadowsRenderTarget[0]->GetSpecifications();
			if (shadowMapSpecs.Width != s_RendererData.ShadowMappingSettings.Resolution
				|| shadowMapSpecs.Height != s_RendererData.ShadowMappingSettings.Resolution)
			{
				for (size_t i = 0; i < 4; i++)
				{
					uint32_t resolution = s_RendererData.ShadowMappingSettings.Resolution;
					if (i >= 2)
						resolution /= 2;

					s_RendererData.ShadowsRenderTarget[i]->Resize(resolution, resolution);
				}
			}
		}

		for (size_t i = 0; i < 4; i++)
		{
			s_RendererData.ShadowsRenderTarget[i]->Bind();
			RenderCommand::Clear();
		}
		viewport.RenderTarget->Bind();
	}

	static void ApplyMaterialFeatures(ShaderFeatures features)
	{
		RenderCommand::SetDepthTestEnabled(features.DepthTesting);
		RenderCommand::SetCullingMode(features.Culling);
		RenderCommand::SetDepthComparisonFunction(features.DepthFunction);
		RenderCommand::SetDepthWriteEnabled(features.DepthWrite);
	}

	static bool CompareRenderableObjects(const RenderableObject& a, const RenderableObject& b)
	{
		if ((uint64_t)a.Material->Handle < (uint64_t)b.Material->Handle)
			return true;

		if (a.Material->Handle == b.Material->Handle)
		{
			if ((uint64_t)a.Mesh->Handle < (uint64_t)b.Mesh->Handle)
				return true;
		}

		return false;
	}

	void Renderer::Flush()
	{
		std::sort(s_RendererData.Queue.begin(), s_RendererData.Queue.end(), CompareRenderableObjects);

		for (size_t i = 0; i < 4; i++)
		{
			s_RendererData.WhiteTexture->Bind(2 + (uint32_t)i);
		}

		RenderCommand::SetDepthTestEnabled(true);
		RenderCommand::SetCullingMode(CullingMode::Front);

		for (size_t i = 0; i < 4; i++)
		{
			const FrameBufferSpecifications& shadowMapSpecs = s_RendererData.ShadowsRenderTarget[i]->GetSpecifications();
			RenderCommand::SetViewport(0, 0, shadowMapSpecs.Width, shadowMapSpecs.Height);

			s_RendererData.CameraBuffer->SetData(
				&s_RendererData.CurrentViewport->FrameData.LightView[i],
				sizeof(s_RendererData.CurrentViewport->FrameData.LightView[i]), 0);

			s_RendererData.ShadowsRenderTarget[i]->Bind();

			DrawQueued(true);
		}

		RenderCommand::SetViewport(0, 0, s_RendererData.CurrentViewport->GetSize().x, s_RendererData.CurrentViewport->GetSize().y);
		s_RendererData.CurrentViewport->RenderTarget->Bind();

		s_RendererData.CameraBuffer->SetData(
			&s_RendererData.CurrentViewport->FrameData.Camera,
			sizeof(s_RendererData.CurrentViewport->FrameData.Camera), 0);

		FrameBufferAttachmentsMask previousMask = s_RendererData.CurrentViewport->RenderTarget->GetWriteMask();
		
		for (size_t i = 0; i < 4; i++)
		{
			s_RendererData.ShadowsRenderTarget[i]->BindAttachmentTexture(0, 2 + (uint32_t)i);
		}

		DrawQueued(false);
		s_RendererData.CurrentViewport->RenderTarget->SetWriteMask(previousMask);

		s_RendererData.InstanceDataBuffer.clear();
		s_RendererData.Queue.clear();
	}

	void Renderer::DrawQueued(bool shadowPass)
	{
		Ref<Material> currentMaterial = nullptr;

		for (const RenderableObject& object : s_RendererData.Queue)
		{
			if (shadowPass && HAS_BIT(object.Flags, MeshRenderFlags::DontCastShadows))
				continue;

			if (object.Mesh->GetSubMesh().InstanceBuffer == nullptr)
				object.Mesh->SetInstanceBuffer(s_RendererData.InstanceBuffer);

			if (s_RendererData.CurrentInstancingMesh.get() != object.Mesh.get())
			{
				FlushInstances();
				s_RendererData.CurrentInstancingMesh = object.Mesh;
			}

			if (object.Material.get() != currentMaterial.get())
			{
				FlushInstances();

				currentMaterial = object.Material;

				if (!shadowPass)
					ApplyMaterialFeatures(object.Material->GetShader()->GetFeatures());

				currentMaterial->SetShaderProperties();

				if (!shadowPass)
				{
					Ref<Shader> shader = object.Material->GetShader();
					if (shader == nullptr)
						continue;

					FrameBufferAttachmentsMask shaderOutputsMask = 0;
					for (uint32_t output : shader->GetOutputs())
						shaderOutputsMask |= (1 << output);

					s_RendererData.CurrentViewport->RenderTarget->SetWriteMask(shaderOutputsMask);
				}
			}

			auto& instanceData = s_RendererData.InstanceDataBuffer.emplace_back();
			instanceData.Transform = object.Transform;
			instanceData.EntityIndex = object.EntityIndex;

			if (s_RendererData.InstanceDataBuffer.size() == (size_t)s_RendererData.MaxInstances)
				FlushInstances();
		}

		FlushInstances();
		s_RendererData.CurrentInstancingMesh = nullptr;
	}

	void Renderer::FlushInstances()
	{
		size_t instancesCount = s_RendererData.InstanceDataBuffer.size();
		if (instancesCount == 0 || s_RendererData.CurrentInstancingMesh == nullptr)
			return;

		s_RendererData.InstanceBuffer->SetData(s_RendererData.InstanceDataBuffer.data(), sizeof(InstanceData) * instancesCount);

		RenderCommand::DrawInstanced(s_RendererData.CurrentInstancingMesh->GetSubMesh().MeshVertexArray, instancesCount);
		s_RendererData.Statistics.DrawCallsCount++;
		s_RendererData.Statistics.DrawCallsSavedByInstances += (uint32_t)instancesCount - 1;

		s_RendererData.InstanceDataBuffer.clear();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::DrawFullscreenQuad(const Ref<Material>& material)
	{
		material->SetShaderProperties();
		ApplyMaterialFeatures(material->GetShader()->GetFeatures());

		const ShaderOutputs& shaderOutputs = material->GetShader()->GetOutputs();

		FrameBufferAttachmentsMask shaderOutputsMask = 0;
		for (uint32_t output : shaderOutputs)
			shaderOutputsMask |= (1 << output);

		FrameBufferAttachmentsMask previousMask = s_RendererData.CurrentViewport->RenderTarget->GetWriteMask();
		s_RendererData.CurrentViewport->RenderTarget->SetWriteMask(shaderOutputsMask);

		RenderCommand::DrawIndexed(s_RendererData.FullscreenQuad);
		s_RendererData.Statistics.DrawCallsCount++;

		s_RendererData.CurrentViewport->RenderTarget->SetWriteMask(previousMask);
	}

	void Renderer::DrawMesh(const Ref<VertexArray>& mesh, const Ref<Material>& material, size_t indicesCount)
	{
		material->SetShaderProperties();
		ApplyMaterialFeatures(material->GetShader()->GetFeatures());

		const ShaderOutputs& shaderOutputs = material->GetShader()->GetOutputs();

		FrameBufferAttachmentsMask shaderOutputsMask = 0;
		for (uint32_t output : shaderOutputs)
			shaderOutputsMask |= (1 << output);

		FrameBufferAttachmentsMask previousMask = s_RendererData.CurrentViewport->RenderTarget->GetWriteMask();
		s_RendererData.CurrentViewport->RenderTarget->SetWriteMask(shaderOutputsMask);

		RenderCommand::DrawIndexed(mesh, indicesCount == SIZE_MAX ? mesh->GetIndexBuffer()->GetCount() : indicesCount);
		s_RendererData.Statistics.DrawCallsCount++;

		s_RendererData.CurrentViewport->RenderTarget->SetWriteMask(previousMask);
	}

	void Renderer::DrawMesh(const Ref<Mesh>& mesh, const Ref<Material>& material, const glm::mat4& transform, MeshRenderFlags flags, int32_t entityIndex)
	{
		if (s_RendererData.ErrorMaterial == nullptr)
			return;

		RenderableObject& object = s_RendererData.Queue.emplace_back();

		if (material)
			object.Material = material->GetShader() == nullptr ? s_RendererData.ErrorMaterial : material;
		else
			object.Material = s_RendererData.ErrorMaterial;

		object.Flags = flags;
		object.Mesh = mesh;
		object.Transform = transform;
		object.EntityIndex = entityIndex;
	}

	void Renderer::AddRenderPass(Ref<RenderPass> pass)
	{
		s_RendererData.RenderPasses.push_back(pass);
	}

	void Renderer::ExecuteRenderPasses()
	{
		Grapple_CORE_ASSERT(s_RendererData.CurrentViewport);
		RenderingContext context(s_RendererData.CurrentViewport->RenderTarget, s_RendererData.CurrentViewport->RTPool);

		for (Ref<RenderPass>& pass : s_RendererData.RenderPasses)
			pass->OnRender(context);
	}

	Viewport& Renderer::GetMainViewport()
	{
		Grapple_CORE_ASSERT(s_RendererData.MainViewport);
		return *s_RendererData.MainViewport;
	}

	Viewport& Renderer::GetCurrentViewport()
	{
		Grapple_CORE_ASSERT(s_RendererData.CurrentViewport);
		return *s_RendererData.CurrentViewport;
	}

	Ref<Texture> Renderer::GetWhiteTexture()
	{
		return s_RendererData.WhiteTexture;
	}

	Ref<const VertexArray> Renderer::GetFullscreenQuad()
	{
		return s_RendererData.FullscreenQuad;
	}

	Ref<FrameBuffer> Renderer::GetShadowsRenderTarget(size_t index)
	{
		return s_RendererData.ShadowsRenderTarget[index];
	}

	ShadowSettings& Renderer::GetShadowSettings()
	{
		return s_RendererData.ShadowMappingSettings;
	}
}
