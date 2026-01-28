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

        FramebufferSpecification fbSpec;
        fbSpec.Width = size;
        fbSpec.Height = size;
        fbSpec.Attachments = {FramebufferTextureFormat::RGBA32F};
        Ref<Framebuffer> fb = Framebuffer::Create(fbSpec);

        shader->Bind();
        pano->Bind(0);
        fb->Bind();

        for (uint32_t i = 0; i < 6; ++i)
        {
            fb->BindColorAttachmentToTexture(0, cube->GetRendererID(), i);

            RenderCommand::Clear();
            Renderer3D::SetCameraMatrices(captureProjection, captureViews[i]);
            Renderer3D::DrawCubeMesh();
        }

        fb->Unbind();
        cube->GenerateMipmaps();
    }

} // namespace Myka
