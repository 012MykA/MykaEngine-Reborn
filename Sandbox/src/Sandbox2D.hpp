#pragma once

#include <MykaEngine.hpp>
#include "ParticleSystem.hpp"

class Sandbox2D : public Myka::Layer
{
public:
    Sandbox2D();
    virtual ~Sandbox2D() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;

    virtual void OnUpdate(Myka::Timestep ts) override;
    virtual void OnImGuiRender() override;
    virtual void OnEvent(Myka::Event &e) override;

private:
    Myka::OrthographicCameraController m_CameraController;

    // Temp
    Myka::Ref<Myka::VertexArray> m_VertexArray;
    Myka::Ref<Myka::Shader> m_FlatColorShader;

    Myka::Ref<Myka::Texture2D> m_BoxTexture;
    Myka::Ref<Myka::Texture2D> m_SpriteSheet;

    ParticleSystem m_ParticleSystem;
    ParticleProps m_Particle;
};