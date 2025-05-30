#pragma once

#include "Grapple/Renderer/GPUTimer.h"

#include <optional>

namespace Grapple
{
	class OpenGLGPUTImer : public GPUTimer
	{
	public:
		OpenGLGPUTImer();
		~OpenGLGPUTImer();

		void Start();
		void Stop();

		std::optional<float> GetElapsedTime() override;
		uint32_t GetId() const { return m_Id; }
	private:
		uint32_t m_Id;
	};
}
