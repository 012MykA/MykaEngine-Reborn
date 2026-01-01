#include "Sandbox2D.hpp"

#include <imgui.h>

static const uint32_t s_MapWidth = 24;
static const char* s_MapTiles =
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWDDDDDDWWWWWWWW"
"WWWWWWWDDDDDDDDDDDWCWWWW"
"WWWWWWDDDDDDDDDDDDDDWWWW"
"WWWWDDDDDDDDDDDDDDDDDWWW"
"WWWDDDDDDDDDDDDDDDDDDDWW"
"WWDDDDDDDDDDDDDDDDDDDDDD"
"WDDDDDDDDDDDDDDDDDDDDDDW"
"WWDDDDDDDDDDDDDDDDDDDDWW"
"WWDDDDDDDDDDDDDDDDDDDWWW"
"WWWDDDDDDDDDDDDDDDDDDWWW"
"WWWWDDDDDDDDDDDDDDDDWWWW"
"WWWWDDDDDDDDDDDDWDWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW";

Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f) {}

void Sandbox2D::OnAttach()
{
    MYKA_PROFILE_FUNCTION();

    m_BoxTexture = Myka::Texture2D::Create("C:/dev/MykaEngine-Reborn/Sandbox/assets/textures/box.png");
    m_SpriteSheet = Myka::Texture2D::Create("C:/dev/MykaEngine-Reborn/Sandbox/assets/game/textures/RPGpack_sheet_2X.png");

    m_TextureStairs = Myka::SubTexture2D::CreateFromCoords(m_SpriteSheet, {0, 11}, {128, 128});
    m_TextureTree = Myka::SubTexture2D::CreateFromCoords(m_SpriteSheet, {2, 1}, {128, 128}, {1, 2});


    m_MapWidth = s_MapWidth;
    m_MapHeight = std::strlen(s_MapTiles) / s_MapWidth;

    m_TextureMap['D'] = Myka::SubTexture2D::CreateFromCoords(m_SpriteSheet, {6, 11}, {128, 128});
    m_TextureMap['W'] = Myka::SubTexture2D::CreateFromCoords(m_SpriteSheet, {11, 11}, {128, 128});

    m_Particle.ColorBegin = {254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f};
    m_Particle.ColorEnd = {254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f};
    m_Particle.SizeBegin = 0.5f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 0.0f;
    m_Particle.LifeTime = 1.0f;
    m_Particle.Velocity = {0.0f, 0.0f};
    m_Particle.VelocityVariation = {3.0f, 1.0f};
    m_Particle.Position = {0.0f, 0.0f};

    m_CameraController.SetZoomLevel(3.0f);
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

#if 0
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
#endif

    if (Myka::Input::IsMouseButtonPressed(MYKA_MOUSE_BUTTON_LEFT))
    {
        auto [x, y] = Myka::Input::GetMousePosition();
        auto width = Myka::Application::Get().GetWindow().GetWidth();
        auto height = Myka::Application::Get().GetWindow().GetHeight();

        auto bounds = m_CameraController.GetBounds();
        auto pos = m_CameraController.GetCamera().GetPosition();
        x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
        y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
    m_Particle.Position = {x + pos.x, y + pos.y};
        for (int i = 0; i < 5; i++)
            m_ParticleSystem.Emit(m_Particle);
    }

    m_ParticleSystem.OnUpdate(ts);
    m_ParticleSystem.OnRender(m_CameraController.GetCamera());

    Myka::Renderer2D::BeginScene(m_CameraController.GetCamera());

    for (uint32_t y = 0; y < m_MapHeight; ++y)
    {
        for (uint32_t x = 0; x < m_MapWidth; ++x)
        {
            char tileType = s_MapTiles[x + y * m_MapWidth];
            Myka::Ref<Myka::SubTexture2D> texture;
            if (m_TextureMap.find(tileType) != m_TextureMap.end())
            {
                texture = m_TextureMap[tileType];
            }
            else
            {
                texture = m_TextureStairs;
            }

            Myka::Renderer2D::DrawQuad({x - m_MapWidth / 2.0f, y - m_MapHeight / 2.0f}, {1.0f, 1.0f}, texture);
        }
    }

    // Myka::Renderer2D::DrawQuad({0.0f, 0.0f, 0.1f}, {1.0f, 1.0f}, m_TextureStairs);
    // Myka::Renderer2D::DrawQuad({1.0f, 0.0f, 0.1f}, {1.0f, 1.0f}, m_TextureBarrel);
    // Myka::Renderer2D::DrawQuad({-1.0f, 0.0f, 0.1f}, {1.0f, 2.0f}, m_TextureTree);
    Myka::Renderer2D::EndScene();
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
