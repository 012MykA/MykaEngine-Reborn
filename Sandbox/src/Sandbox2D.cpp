#include "Sandbox2D.hpp"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f) {}

void Sandbox2D::OnAttach()
{
    m_BoxTexture = Myka::Texture2D::Create("C:/dev/MykaEngine-Reborn/Sandbox/assets/textures/box.png");
}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnUpdate(Myka::Timestep ts)
{
    // Update
    m_CameraController.OnUpdate(ts);

    // Render
    Myka::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
    Myka::RenderCommand::Clear();

    Myka::Renderer2D::BeginScene(m_CameraController.GetCamera());
    Myka::Renderer2D::DrawQuad({-1.0f, 0.0f}, {0.8f, 0.8f}, {0.8f, 0.2f, 0.3f, 1.0f});
    Myka::Renderer2D::DrawQuad({0.5f, -0.5f}, {0.5f, 0.75f}, {0.2f, 0.3f, 0.8f, 1.0f});
    Myka::Renderer2D::DrawQuad({0.0f, 0.0f, -0.1f}, {1.0f, 1.0f}, m_BoxTexture);
    Myka::Renderer2D::EndScene();
} 

void Sandbox2D::OnImGuiRender()
{
    ImGui::Begin("Settings");
    ImGui::End();
}

void Sandbox2D::OnEvent(Myka::Event &e)
{
    m_CameraController.OnEvent(e);
}
