#pragma once

#include "MykaEngine/Renderer/Texture.hpp"

#include <glad/glad.h>

namespace Myka
{
    class OpenGLCubeTexture : public CubeTexture
    {
    public:
        OpenGLCubeTexture(const TextureSpecification &spec);
        OpenGLCubeTexture(const std::vector<std::filesystem::path> &paths);
        virtual ~OpenGLCubeTexture();

        virtual void GenerateMipmaps() override;

        virtual const TextureSpecification &GetSpecification() const override { return m_Specification; }

        virtual uint32_t GetWidth() const override { return m_Specification.Width; }
        virtual uint32_t GetHeight() const override { return m_Specification.Height; }
        virtual uint32_t GetRendererID() const override { return m_RendererID; }
        virtual const std::vector<std::filesystem::path> &GetPaths() const override { return m_Paths; }

        virtual void SetData(const void *data, uint32_t size) override;

        virtual void Bind(uint32_t slot = 0) const override;

        virtual bool operator==(const Texture &other) const override
        {
            return m_RendererID == ((OpenGLCubeTexture &)other).m_RendererID;
        }

    private:
        TextureSpecification m_Specification;
        const std::vector<std::filesystem::path> m_Paths;

        uint32_t m_RendererID;
        GLenum m_InternalFormat, m_DataFormat;
    };
} // namespace Myka