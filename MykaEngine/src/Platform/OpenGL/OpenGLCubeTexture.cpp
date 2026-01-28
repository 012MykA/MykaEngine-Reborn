#include "mykapch.hpp"
#include "OpenGLCubeTexture.hpp"

#include <stb_image.h>

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

    OpenGLCubeTexture::OpenGLCubeTexture(const TextureSpecification &spec) : m_Specification(spec), m_Paths({""})
    {
        MYKA_PROFILE_FUNCTION();

        m_InternalFormat = Utils::MykaImageFormatToGLInternalFormat(m_Specification.Format);
        m_DataFormat = Utils::MykaImageFormatToGLDataFormat(m_Specification.Format);

        glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_RendererID);
        glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Specification.Width, m_Specification.Height);

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    OpenGLCubeTexture::OpenGLCubeTexture(const std::vector<std::filesystem::path> &paths)
    {
        MYKA_PROFILE_FUNCTION();
        MYKA_CORE_ASSERT(paths.size() == 6, "Requires 6 images");

        int width, height, channels;
        stbi_set_flip_vertically_on_load(0);

        glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_RendererID);

        for (uint32_t i = 0; i < m_Paths.size(); i++)
        {
            bool isHDR = stbi_is_hdr(paths[i].string().c_str());

            void *data = nullptr;
            if (isHDR)
            {
                data = stbi_loadf(paths[i].string().c_str(), &width, &height, &channels, 0);
                if (data)
                {
                    if (i == 0)
                    {
                        m_InternalFormat = (channels == 4) ? GL_RGBA32F : GL_RGB32F;
                        m_DataFormat = (channels == 4) ? GL_RGBA : GL_RGB;
                        glTextureStorage2D(m_RendererID, 1, m_InternalFormat, width, height);
                    }

                    glTextureSubImage3D(m_RendererID, 0, 0, 0, i, width, height, 1, m_DataFormat, GL_FLOAT, data);
                }
            }
            else
            {
                data = stbi_load(m_Paths[i].string().c_str(), &width, &height, &channels, 0);
                if (data)
                {
                    if (i == 0)
                    {
                        m_Specification.Width = width;
                        m_Specification.Height = height;
                        m_InternalFormat = (channels == 4) ? GL_RGBA8 : GL_RGB8;
                        m_DataFormat = (channels == 4) ? GL_RGBA : GL_RGB;

                        glTextureStorage2D(m_RendererID, 1, m_InternalFormat, width, height);
                    }

                    glTextureSubImage3D(m_RendererID, 0, 0, 0, i, width, height, 1, m_DataFormat, GL_UNSIGNED_BYTE, data);
                    stbi_image_free(data);
                }
            }
        }

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    OpenGLCubeTexture::~OpenGLCubeTexture()
    {
        MYKA_PROFILE_FUNCTION();

        glDeleteTextures(1, &m_RendererID);
    }

    void OpenGLCubeTexture::GenerateMipmaps()
    {
        glGenerateTextureMipmap(m_RendererID);
    }

    void OpenGLCubeTexture::SetData(const void *data, uint32_t size)
    {
        MYKA_PROFILE_FUNCTION();

        uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
        uint32_t faceSize = m_Specification.Width * m_Specification.Height * bpp;

        MYKA_CORE_ASSERT(size == faceSize * 6, "OpenGLCubeTexture::SetData: data must contain 6 faces!")

        for (uint32_t i = 0; i < 6; ++i)
        {
            const unsigned char *faceData = static_cast<const unsigned char *>(data) + (i * faceSize);
            glTextureSubImage3D(m_RendererID, 0, 0, 0, i, m_Specification.Width, m_Specification.Height, 1, m_DataFormat, GL_UNSIGNED_BYTE, faceData);
        }
    }

    void OpenGLCubeTexture::Bind(uint32_t slot) const
    {
        glBindTextureUnit(slot, m_RendererID);
    }

} // namespace Myka
