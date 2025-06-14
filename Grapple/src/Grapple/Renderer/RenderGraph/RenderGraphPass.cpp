#include "RenderGraphPass.h"

#include "GrappleCore/Assert.h"

namespace Grapple
{
	void RenderGraphPassSpecifications::SetDebugName(std::string_view debugName)
	{
		m_DebugName = debugName;
	}

	void RenderGraphPassSpecifications::AddInput(const Ref<Texture>& texture, ImageLayout layout)
	{
		Grapple_CORE_ASSERT(texture != nullptr);
		Input& input = m_Inputs.emplace_back();
		input.InputTexture = texture;
		input.Layout = layout;
	}

	void RenderGraphPassSpecifications::AddOutput(const Ref<Texture>& texture, uint32_t attachmentIndex, ImageLayout layout)
	{
		Grapple_CORE_ASSERT(texture != nullptr);
		auto& output = m_Outputs.emplace_back();
		output.AttachmentTexture = texture;
		output.AttachmentIndex = attachmentIndex;
		output.Layout = layout;
	}

	void RenderGraphPassSpecifications::AddOutput(const Ref<Texture>& texture,
		uint32_t attachmentIndex,
		const glm::vec4& clearColor,
		ImageLayout layout)
	{
		Grapple_CORE_ASSERT(texture != nullptr);
		auto& output = m_Outputs.emplace_back();
		output.AttachmentTexture = texture;
		output.AttachmentIndex = attachmentIndex;
		output.Layout = layout;
		output.ClearValue = AttachmentClearValue(clearColor);

		m_HasOutputClearValues = true;
	}

	void RenderGraphPassSpecifications::AddOutput(const Ref<Texture>& texture,
		uint32_t attachmentIndex,
		float depthClearValue,
		ImageLayout layout)
	{
		Grapple_CORE_ASSERT(texture != nullptr);
		auto& output = m_Outputs.emplace_back();
		output.AttachmentTexture = texture;
		output.AttachmentIndex = attachmentIndex;
		output.Layout = layout;
		output.ClearValue = AttachmentClearValue(depthClearValue);

		m_HasOutputClearValues = true;
	}
}
