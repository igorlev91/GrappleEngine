#pragma once

#include "Grapple/Renderer/ShaderStorageBuffer.h"

namespace Grapple
{
    class OpenGLShaderStorageBuffer : public ShaderStorageBuffer
    {
    public:
        OpenGLShaderStorageBuffer(uint32_t binding);
        ~OpenGLShaderStorageBuffer();

        size_t GetSize() const override;
        void SetData(const MemorySpan& data) override;

        void SetDebugName(std::string_view name) override;
        const std::string& GetDebugName() const override;
    private:
        std::string m_DebugName;

        uint32_t m_Id = 0;
        uint32_t m_Binding = 0;
        size_t m_Size = 0;
    };
}
