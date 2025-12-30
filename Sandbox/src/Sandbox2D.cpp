#include "Sandbox2D.hpp"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f) {}

void Sandbox2D::OnAttach()
{
    MYKA_PROFILE_FUNCTION();

    m_BoxTexture = Myka::Texture2D::Create("C:/dev/MykaEngine-Reborn/Sandbox/assets/textures/box.png");
}

void Sandbox2D::OnDetach()
{
    MYKA_PROFILE_FUNCTION();
}

void Sandbox2D::OnUpdate(Myka::Timestep ts)
{
    MYKA_PROFILE_FUNCTION();

    // Update
    m_CameraController.OnUpdate(ts);

    // Render
    {
        MYKA_PROFILE_SCOPE("Renderer Prep");

        Myka::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
        Myka::RenderCommand::Clear();
    }

    {
        MYKA_PROFILE_SCOPE("Renderer Draw");

        Myka::Renderer2D::BeginScene(m_CameraController.GetCamera());
        
        static float rotation = 0.0f;
        rotation += ts * 50.0f;

        Myka::Renderer2D::DrawQuad({0.5f, -0.5f}, {0.5f, 0.75f}, {0.2f, 0.3f, 0.8f, 1.0f});
        Myka::Renderer2D::DrawRotatedQuad({-1.0f, 0.0f}, {0.5f, 0.5f}, glm::radians(-rotation), {0.8f, 0.2f, 0.3f, 1.0f});

        Myka::Renderer2D::DrawQuad({1.0f, 1.0f, -0.2f}, {1.5f, 1.5f}, m_BoxTexture, 5.0f);
        Myka::Renderer2D::DrawRotatedQuad({0.0f, 0.0f, -0.1f}, {1.0f, 1.0f}, glm::radians(rotation), m_BoxTexture);

        Myka::Renderer2D::EndScene();
    }
}

void Sandbox2D::OnImGuiRender()
{
    MYKA_PROFILE_FUNCTION();
    ImGui::Begin("Debug");

    ImGui::End();
}

void Sandbox2D::OnEvent(Myka::Event &e)
{
    m_CameraController.OnEvent(e);
}
