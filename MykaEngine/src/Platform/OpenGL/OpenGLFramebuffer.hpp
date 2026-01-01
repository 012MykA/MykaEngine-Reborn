#pragma once

#include "MykaEngine/Renderer/Framebuffer.hpp"

namespace Myka
{
    class OpenGLFramebuffer : public Framebuffer
    {
    public:
        OpenGLFramebuffer(const FramebufferSpecification &spec);
        virtual ~OpenGLFramebuffer();

        void Invalidate();

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual uint32_t GetColorAttachment() const override { return m_ColorAttachment; }

        virtual const FramebufferSpecification &GetSpecification() const override { return m_Specification; }

    private:
        uint32_t m_RendererID;
        uint32_t m_ColorAttachment, m_DepthAttachment;
        FramebufferSpecification m_Specification;
    };
} // namespace Myka
