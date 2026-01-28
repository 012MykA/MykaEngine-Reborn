#include "mykapch.hpp"
#include "IBLBaker.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Renderer3D.hpp"
#include "RenderCommand.hpp"
#include "Framebuffer.hpp"

#include <glad/glad.h> // TODO: refactor

namespace Myka
{
    void IBLBaker::ConvertPanoramaToCubemap(const Ref<Texture2D> &pano, const Ref<CubeTexture> &cube)
    {
        auto shader = Renderer3D::GetEquirectToCubeShader();
        uint32_t size = cube->GetWidth();

        glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
        glm::mat4 captureViews[] = {
            glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
            glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        };

        // Настройка Framebuffer (нужно убедиться, что ваша реализация FB поддерживает привязку Cubemap)
        FramebufferSpecification fbSpec;
        fbSpec.Width = size;
        fbSpec.Height = size;
        fbSpec.Attachments = {FramebufferTextureFormat::RGBA8}; // или RGBA32F для HDR
        Ref<Framebuffer> fb = Framebuffer::Create(fbSpec);

        shader->Bind();
        pano->Bind(0);

        RenderCommand::SetViewport(0, 0, size, size);
        fb->Bind();

        for (uint32_t i = 0; i < 6; ++i)
        {
            // Если в Framebuffer этого нет, используйте glFramebufferTexture2D напрямую:
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cube->GetRendererID(), 0);

            RenderCommand::Clear();

            // Обновляем CameraUniformBuffer (UBO) для шейдера
            Renderer3D::SetCameraMatrices(captureProjection, captureViews[i]);

            Renderer3D::DrawCubeMesh();
        }

        fb->Unbind();
        cube->GenerateMipmaps();
    }

} // namespace Myka
