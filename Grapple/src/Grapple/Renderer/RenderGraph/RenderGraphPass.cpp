#include "RenderGraphPass.h"

#include "GrappleCore/Assert.h"

namespace Grapple
{
	void RenderGraphPassSpecifications::SetDebugName(std::string_view debugName)
	{
		m_DebugName = debugName;
	}

	void RenderGraphPassSpecifications::AddInput(const Ref<Texture>& texture)
	{
		Grapple_CORE_ASSERT(texture != nullptr);
		m_Inputs.push_back(texture);
	}

	void RenderGraphPassSpecifications::AddOutput(const Ref<Texture>& texture, uint32_t attachmentIndex)
	{
		Grapple_CORE_ASSERT(texture != nullptr);
		auto& output = m_Outputs.emplace_back();
		output.AttachmentTexture = texture;
		output.AttachmentIndex = attachmentIndex;
	}
}
