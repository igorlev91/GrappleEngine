#include "OpenGLFrameBuffer.h"

#include "Grapple/Core/Assert.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace Grapple
{
    static GLenum FrameBufferTextureFormatToOpenGLFormat(FrameBufferTextureFormat format)
    {
        switch (format)
        {
        case FrameBufferTextureFormat::RGB8:
            return GL_RGB8;
        case FrameBufferTextureFormat::RGBA8:
            return GL_RGBA8;
        }

        Grapple_CORE_ASSERT(false, "Unhandled frame buffer format");
    }

    OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecifications& specifications)
        : m_Specifications(specifications)
    {
        m_ColorAttachments.resize(m_Specifications.Attachments.size());

        glCreateFramebuffers(1, &m_Id);
        glBindFramebuffer(GL_FRAMEBUFFER, m_Id);

        glCreateTextures(GL_TEXTURE_2D, m_ColorAttachments.size(), m_ColorAttachments.data());

        for (size_t i = 0; i < m_ColorAttachments.size(); i++)
        {
            glTextureStorage2D(m_ColorAttachments[i], 1, FrameBufferTextureFormatToOpenGLFormat(m_Specifications.Attachments[i].Format), 
                m_Specifications.Width, m_Specifications.Height);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_ColorAttachments[i], 0);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    OpenGLFrameBuffer::~OpenGLFrameBuffer()
    {
        glDeleteFramebuffers(1, &m_Id);
        glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
    }

    void OpenGLFrameBuffer::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_Id);
    }

    void OpenGLFrameBuffer::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
    {
    }

    void* OpenGLFrameBuffer::GetColorAttachmentRendererId(uint32_t attachmentIndex)
    {
        Grapple_CORE_ASSERT((size_t)attachmentIndex < m_ColorAttachments.size());
        return reinterpret_cast<void*>(m_ColorAttachments[attachmentIndex]);
    }
}
