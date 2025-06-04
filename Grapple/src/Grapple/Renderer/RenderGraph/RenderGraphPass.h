#pragma once

#include "GrappleCore/Core.h"

namespace Grapple
{
	class Texture;
	class CommandBuffer;

	class Grapple_API RenderGraphPassSpecifications
	{
	public:
		struct OutputAttachment
		{
			Ref<Texture> AttachmentTexture = nullptr;
			uint32_t AttachmentIndex = 0;
		};

		void AddInput(const Ref<Texture>& texture);
		void AddOutput(const Ref<Texture>& texture, uint32_t attachmentIndex);

		inline const std::vector<Ref<Texture>>& GetInputs() const { return m_Inputs; };
		inline const std::vector<OutputAttachment>& GetOutputs() const { return m_Outputs; }
	private:
		std::vector<Ref<Texture>> m_Inputs;
		std::vector<OutputAttachment> m_Outputs;
	};

	class Grapple_API RenderGraphPass
	{
	public:
		virtual ~RenderGraphPass() = default;
		virtual void OnRender(Ref<CommandBuffer> commandBuffer) = 0;
	};
}
