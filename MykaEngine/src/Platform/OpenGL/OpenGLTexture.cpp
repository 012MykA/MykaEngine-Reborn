#include "OpenGLTexture.hpp"

#include <glad/glad.h>

namespace Myka
{
    OpenGLTexture2D::OpenGLTexture2D(const std::string &path) : m_Path(path)
    {

    }

    OpenGLTexture2D::~OpenGLTexture2D()
    {
    }

    void OpenGLTexture2D::Bind() const
    {
    }

} // namespace Myka
