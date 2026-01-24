#pragma once

#include "MykaEngine/Renderer/Texture.hpp"

#include <stb_image.h>
#include <glad/glad.h>

namespace Myka
{
    class OpenGLTexture2D : public Texture2D
    {
    public:
        OpenGLTexture2D(const TextureSpecification &specification);
        OpenGLTexture2D(const std::filesystem::path &path);
        virtual ~OpenGLTexture2D();

        virtual const TextureSpecification &GetSpecification() const override { return m_Specification; }

        virtual uint32_t GetWidth() const override { return m_Specification.Width; }
        virtual uint32_t GetHeight() const override { return m_Specification.Height; }
        virtual uint32_t GetRendererID() const override { return m_RendererID; }
        virtual const std::filesystem::path &GetPath() const override { return m_Path; }

        virtual void SetData(const void *data, uint32_t size) override;

        virtual void Bind(uint32_t slot = 0) const override;

        virtual bool operator==(const Texture &other) const override
        {
            return m_RendererID == ((OpenGLTexture2D &)other).m_RendererID;
        }

    private:
        TextureSpecification m_Specification;
        std::filesystem::path m_Path;

        uint32_t m_RendererID;
        GLenum m_InternalFormat, m_DataFormat;
    };
} // namespace Myka
