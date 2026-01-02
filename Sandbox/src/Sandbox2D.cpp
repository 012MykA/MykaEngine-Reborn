#include "Sandbox2D.hpp"

#include <imgui.h>

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
    Myka::Renderer2D::ResetStats();
    {
        MYKA_PROFILE_SCOPE("Renderer Prep");

        Myka::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
        Myka::RenderCommand::Clear();
    }

    {
        static float rotation = 0.0f;
        rotation += ts * 50.0f;

        MYKA_PROFILE_SCOPE("Renderer Draw");

        Myka::Renderer2D::BeginScene(m_CameraController.GetCamera());

        Myka::Renderer2D::DrawRotatedQuad({0.0f, 0.0f, -0.1f}, {1.0f, 1.0f}, glm::radians(rotation), m_BoxTexture);

        for (float y = -5.0f; y < 5.0f; y += 0.5f)
        {
            for (float x = -5.0f; x < 5.0f; x += 0.5f)
            {
                glm::vec4 color = {(x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.5f};
                Myka::Renderer2D::DrawQuad({x, y}, {0.45f, 0.45f}, color);
            }
        }

        Myka::Renderer2D::EndScene();
    }
}

void Sandbox2D::OnImGuiRender()
{
    MYKA_PROFILE_FUNCTION();

    ImGui::Begin("Renderer2D Stats");
    auto stats = Myka::Renderer2D::GetStats();
    ImGui::Text("DrawCalls: %d", stats.DrawCalls);
    ImGui::Text("Quads: %d", stats.QuadCount);
    ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
    ImGui::End();
}

void Sandbox2D::OnEvent(Myka::Event &e)
{
    m_CameraController.OnEvent(e);
}
