#include "Sandbox2D.hpp"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

template <typename Fn>
class Timer
{
public:
    Timer(const char *name, Fn &&func) : m_Name(name), m_Func(func), m_Stopped(false)
    {
        m_StartTimepoint = std::chrono::high_resolution_clock::now();
    }

    ~Timer()
    {
        if (!m_Stopped)
            Stop();
    }

    void Stop()
    {
        auto endTimepoint = std::chrono::high_resolution_clock::now();

        long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
        long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

        m_Stopped = true;

        float duration = (end - start) * 0.001f;

        ProfileResult result(m_Name, duration);
        m_Func(result);
    }

private:
    const char *m_Name;
    Fn m_Func;
    std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
    bool m_Stopped;
};

#define PROFILE_SCOPE(name) Timer timer##__LINE__(name, [&](ProfileResult &profileResult) { m_ProfileResults.push_back(profileResult); })

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
    PROFILE_SCOPE("Sandbox2D::OnUpdate");

    // Update
    {
        PROFILE_SCOPE("m_CameraController::OnUpdate");
        m_CameraController.OnUpdate(ts);
    }

    // Render
    {
        PROFILE_SCOPE("Renderer Prep");
        Myka::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
        Myka::RenderCommand::Clear();
    }

    {
        PROFILE_SCOPE("Renderer Draw");
        Myka::Renderer2D::BeginScene(m_CameraController.GetCamera());
        Myka::Renderer2D::DrawQuad({-1.0f, 0.0f}, {0.8f, 0.8f}, {0.8f, 0.2f, 0.3f, 1.0f});
        Myka::Renderer2D::DrawQuad({0.5f, -0.5f}, {0.5f, 0.75f}, {0.2f, 0.3f, 0.8f, 1.0f});
        Myka::Renderer2D::DrawQuad({0.0f, 0.0f, -0.1f}, {1.0f, 1.0f}, m_BoxTexture);
        Myka::Renderer2D::EndScene();
    }
}

void Sandbox2D::OnImGuiRender()
{
    ImGui::Begin("Debug");

    for (auto &result : m_ProfileResults)
    {
        ImGui::Text("%.3fms    %s", result.Time, result.Name);
    }
    m_ProfileResults.clear();

    ImGui::End();
}

void Sandbox2D::OnEvent(Myka::Event &e)
{
    m_CameraController.OnEvent(e);
}
