#pragma once

#include <MykaEngine.hpp>

namespace Myka
{
    class EditorLayer : public Layer
    {
    public:
        EditorLayer();
        virtual ~EditorLayer() = default;
    
        virtual void OnAttach() override;
        virtual void OnDetach() override;
    
        virtual void OnUpdate(Timestep ts) override;
        virtual void OnImGuiRender() override;
        virtual void OnEvent(Event &e) override;
    
    private:
        OrthographicCameraController m_CameraController;
    
        // Temp
        Ref<VertexArray> m_VertexArray;
        Ref<Shader> m_FlatColorShader;
        Ref<Framebuffer> m_Framebuffer;
    
        Ref<Texture2D> m_BoxTexture;
    };
} // namespace Myka
