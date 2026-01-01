#include "Sandbox2D.hpp"

#include <imgui.h>

static const uint32_t s_MapWidth = 24;
static const char *s_MapTiles =
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

    // TL;DR; this demo is more complicated than what most users you would normally use.
    // If we remove all options we are showcasing, this demo would become a simple call to ImGui::DockSpaceOverViewport() !!
    // In this specific demo, we are not using DockSpaceOverViewport() because:
    static bool dockSpaceOpen = true;
    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }
    else
    {
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    if (!opt_padding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &dockSpaceOpen, window_flags);
    if (!opt_padding)
        ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // Submit the DockSpace
    // REMINDER: THIS IS A DEMO FOR ADVANCED USAGE OF DockSpace()!
    // MOST REGULAR APPLICATIONS WILL SIMPLY WANT TO CALL DockSpaceOverViewport(). READ COMMENTS ABOVE.
    ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    // Show demo options and help
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit"))
            {
                Myka::Application::Get().Close();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            ImGui::TextUnformatted(
                "This demo has nothing to do with enabling docking!"
                "\n"
                "This demo only demonstrate the use of ImGui::DockSpace() which allows you to manually\ncreate a docking node _within_ another window."
                "\n"
                "Most application can simply call ImGui::DockSpaceOverViewport() and be done with it.");
            ImGui::Separator();
            ImGui::TextUnformatted("When docking is enabled, you can ALWAYS dock MOST window into another! Try it now!"
                                   "\n"
                                   "- Drag from window title bar or their tab to dock/undock."
                                   "\n"
                                   "- Drag from window menu button (upper-left button) to undock an entire node (all windows)."
                                   "\n"
                                   "- Hold SHIFT to disable docking (if io.ConfigDockingWithShift == false, default)"
                                   "\n"
                                   "- Hold SHIFT to enable docking (if io.ConfigDockingWithShift == true)");
            ImGui::Separator();
            ImGui::TextUnformatted("More details:");
            ImGui::Bullet();
            ImGui::SameLine();
            ImGui::TextLinkOpenURL("Docking Wiki page", "https://github.com/ocornut/imgui/wiki/Docking");
            ImGui::BulletText("Read comments in ShowExampleAppDockSpace()");
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::Begin("Renderer2D Stats");
    auto stats = Myka::Renderer2D::GetStats();
    ImGui::Text("DrawCalls: %d", stats.DrawCalls);
    ImGui::Text("Quads: %d", stats.QuadCount);
    ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

    ImGui::Image(m_BoxTexture->GetRendererID(), ImVec2(256.0f, 256.0f));
    ImGui::End();

    ImGui::End();
}

void Sandbox2D::OnEvent(Myka::Event &e)
{
    m_CameraController.OnEvent(e);
}
