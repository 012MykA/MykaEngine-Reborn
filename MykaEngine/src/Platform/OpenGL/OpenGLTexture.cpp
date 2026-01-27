#include "mykapch.hpp"
#include "OpenGLTexture.hpp"

#include "MykaEngine/Core/Log.hpp"

namespace Myka
{
    namespace Utils
    {
        static GLenum MykaImageFormatToGLDataFormat(ImageFormat format)
        {
            switch (format)
            {
            case ImageFormat::RGB8:
                return GL_RGB;
            case ImageFormat::RGBA8:
                return GL_RGBA;
            case ImageFormat::RGBA32F:
                return GL_RGBA;
            }

            MYKA_CORE_ASSERT(false);
            return 0;
        }

        static GLenum MykaImageFormatToGLInternalFormat(ImageFormat format)
        {
            switch (format)
            {
            case ImageFormat::RGB8:
                return GL_RGB8;
            case ImageFormat::RGBA8:
                return GL_RGBA8;
            case ImageFormat::RGBA32F:
                return GL_RGBA32F;
            }

            MYKA_CORE_ASSERT(false);
            return 0;
        }

    }

    OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification &specification) : m_Specification(specification)
    {
        MYKA_PROFILE_FUNCTION();

        m_InternalFormat = Utils::MykaImageFormatToGLInternalFormat(m_Specification.Format);
        m_DataFormat = Utils::MykaImageFormatToGLDataFormat(m_Specification.Format);

        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Specification.Width, m_Specification.Height);

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    OpenGLTexture2D::OpenGLTexture2D(const std::filesystem::path &path) : m_Path(path)
    {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(1);

        bool isHDR = stbi_is_hdr(path.string().c_str());
        void *data = nullptr;

        if (isHDR)
        {
            data = stbi_loadf(path.string().c_str(), &width, &height, &channels, 4);
            m_InternalFormat = GL_RGBA32F;
            m_DataFormat = GL_RGBA;
        }
        else
        {
            data = stbi_load(path.string().c_str(), &width, &height, &channels, 4);
            m_InternalFormat = GL_RGBA8;
            m_DataFormat = GL_RGBA;
        }

        MYKA_CORE_ASSERT(data, "Failed to load image!");
        m_Specification.Width = width;
        m_Specification.Height = height;

        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        glTextureStorage2D(m_RendererID, 1, m_InternalFormat, width, height);

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        GLenum type = isHDR ? GL_FLOAT : GL_UNSIGNED_BYTE;
        glTextureSubImage2D(m_RendererID, 0, 0, 0, width, height, m_DataFormat, type, data);

        stbi_image_free(data);
    }

    OpenGLTexture2D::~OpenGLTexture2D()
    {
        MYKA_PROFILE_FUNCTION();

        glDeleteTextures(1, &m_RendererID);
    }

    void OpenGLTexture2D::SetData(const void *data, uint32_t size)
    {
        MYKA_PROFILE_FUNCTION();

        uint32_t bytesPerChannel = (m_InternalFormat == GL_RGBA32F) ? 4 : 1;
        MYKA_CORE_ASSERT(size == m_Specification.Width * m_Specification.Height * channels * bytesPerChannel, "Data must be entire texture!");

        GLenum type = (m_InternalFormat == GL_RGBA32F) ? GL_FLOAT : GL_UNSIGNED_BYTE;
        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Specification.Width, m_Specification.Height, m_DataFormat, type, data);
    }

    void OpenGLTexture2D::Bind(uint32_t slot) const
    {
        MYKA_PROFILE_FUNCTION();

        glBindTextureUnit(slot, m_RendererID);
    }

} // namespace Myka
