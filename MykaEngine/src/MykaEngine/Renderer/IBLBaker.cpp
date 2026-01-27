#include "mykapch.hpp"
#include "IBLBaker.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Renderer3D.hpp"

#include <glad/glad.h> // TODO: refactor

namespace Myka
{
    void IBLBaker::ConvertPanoramaToCubemap(const Ref<Texture2D> &pano, const Ref<CubeTexture> &cube)
    {
        auto shader = Renderer3D::GetEquirectToCubeShader();
        shader->Bind();
        pano->Bind(0);

        glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
        glm::mat4 captureViews[] = {
            glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),  // +X
            glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)), // -X
            glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),   // +Y
            glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)), // -Y
            glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),  // +Z
            glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))  // -Z
        };

        uint32_t captureFBO;
        glCreateFramebuffers(1, &captureFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

        int oldViewport[4];
        glGetIntegerv(GL_VIEWPORT, oldViewport);

        glViewport(0, 0, cube->GetWidth(), cube->GetHeight());

        for (uint32_t i = 0; i < 6; i++)
        {
            glm::mat4 viewProj = captureProjection * captureViews[i];
            Renderer3D::GetCameraUniformBuffer()->SetData(&viewProj, sizeof(glm::mat4));

            glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, cube->GetRendererID(), 0, i);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            Renderer3D::DrawCubeMesh();
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &captureFBO);

        glViewport(oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]);
        cube->GenerateMipmaps();
    }

} // namespace Myka
