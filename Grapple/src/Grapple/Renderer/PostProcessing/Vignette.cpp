#include "Vignette.h"

#include "Grapple/AssetManager/AssetManager.h"

#include "Grapple/Renderer/Renderer.h"
#include "Grapple/Renderer/RenderCommand.h"
#include "Grapple/Renderer/ShaderLibrary.h"

#include "GrappleCore/Profiler/Profiler.h"

namespace Grapple
{
	Grapple_IMPL_TYPE(Vignette);

	static uint32_t s_ColorPropertyIndex = UINT32_MAX;
	static uint32_t s_RadiusPropertyIndex = UINT32_MAX;
	static uint32_t s_SmoothnessPropertyIndex = UINT32_MAX;

	Vignette::Vignette()
		: RenderPass(RenderPassQueue::PostProcessing), Enabled(false), Color(0.0f, 0.0f, 0.0f, 0.5f), Radius(1.0f), Smoothness(1.0f)
	{
		std::optional<AssetHandle> shaderHandle = ShaderLibrary::FindShader("Vignette");
		if (!shaderHandle || !AssetManager::IsAssetHandleValid(shaderHandle.value()))
		{
			Grapple_CORE_ERROR("Vignette: Failed to find Vignette shader");
			return;
		}

		Ref<Shader> shader = AssetManager::GetAsset<Shader>(shaderHandle.value());
		m_Material = Material::Create(shader);

		s_ColorPropertyIndex = shader->GetPropertyIndex("u_Params.Color").value_or(UINT32_MAX);
		s_RadiusPropertyIndex = shader->GetPropertyIndex("u_Params.Radius").value_or(UINT32_MAX);
		s_SmoothnessPropertyIndex = shader->GetPropertyIndex("u_Params.Smoothness").value_or(UINT32_MAX);
	}

	void Vignette::OnRender(RenderingContext& context)
	{
		Grapple_PROFILE_FUNCTION();

		if (!Enabled || !Renderer::GetCurrentViewport().PostProcessingEnabled)
			return;

		FrameBufferAttachmentsMask writeMask = context.RenderTarget->GetWriteMask();

		m_Material->WritePropertyValue(s_ColorPropertyIndex, Color);
		m_Material->WritePropertyValue(s_RadiusPropertyIndex, Radius);
		m_Material->WritePropertyValue(s_SmoothnessPropertyIndex, Smoothness);

		context.RenderTarget->SetWriteMask(0x1);
		Renderer::DrawFullscreenQuad(m_Material);
		context.RenderTarget->SetWriteMask(writeMask);
	}

	void TypeSerializer<Vignette>::OnSerialize(Vignette& vignette, SerializationStream& stream)
	{
		stream.Serialize("Enabled", SerializationValue(vignette.Enabled));
		stream.Serialize("Color", SerializationValue(vignette.Color, SerializationValueFlags::Color));
		stream.Serialize("Radius", SerializationValue(vignette.Radius));
		stream.Serialize("Smoothness", SerializationValue(vignette.Smoothness));
	}
}
