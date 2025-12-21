#include "ImGuiLayer.hpp"
#include "MykaEngine/Application.hpp"

// TEMPORARY
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "MykaEngine/KeyCodes.hpp"

namespace Myka
{
    static ImGuiKey ImGui_ImplGlfw_KeyToImGuiKey(int keycode);

    ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer")
    {
    }

    ImGuiLayer::~ImGuiLayer()
    {
    }

    void ImGuiLayer::OnAttach()
    {
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGuiIO &io = ImGui::GetIO();
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui_ImplOpenGL3_Init("#version 460");
    }

    void ImGuiLayer::OnDetach()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiLayer::OnUpdate()
    {
        ImGuiIO &io = ImGui::GetIO();
        Application &app = Application::Get();
        io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

        float time = (float)glfwGetTime();
        io.DeltaTime = m_Time > 0.0 ? (time - m_Time) : (1.0f / 60.0f);
        m_Time = time;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        static bool show = true;
        ImGui::ShowDemoWindow(&show);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void ImGuiLayer::OnEvent(Event &event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<MouseButtonPressedEvent>(MYKA_BIND_EVENT_FN(ImGuiLayer::OnMouseButtonPressedEvent));
        dispatcher.Dispatch<MouseButtonReleasedEvent>(MYKA_BIND_EVENT_FN(ImGuiLayer::OnMouseButtonReleasedEvent));
        dispatcher.Dispatch<MouseMovedEvent>(MYKA_BIND_EVENT_FN(ImGuiLayer::OnMouseMovedEvent));
        dispatcher.Dispatch<MouseScrolledEvent>(MYKA_BIND_EVENT_FN(ImGuiLayer::OnMouseScrolledEvent));
        dispatcher.Dispatch<KeyPressedEvent>(MYKA_BIND_EVENT_FN(ImGuiLayer::OnKeyPressedEvent));
        dispatcher.Dispatch<KeyReleasedEvent>(MYKA_BIND_EVENT_FN(ImGuiLayer::OnKeyReleasedEvent));
        dispatcher.Dispatch<KeyTypedEvent>(MYKA_BIND_EVENT_FN(ImGuiLayer::OnKeyTypedEvent));
        dispatcher.Dispatch<WindowResizeEvent>(MYKA_BIND_EVENT_FN(ImGuiLayer::OnWindowResizeEvent));
    }

    bool ImGuiLayer::OnMouseButtonPressedEvent(MouseButtonPressedEvent &e)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseButtonEvent((int)e.GetMouseButton(), true);

        return false;
    }

    bool ImGuiLayer::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent &e)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseButtonEvent((int)e.GetMouseButton(), false);

        return false;
    }

    bool ImGuiLayer::OnMouseMovedEvent(MouseMovedEvent &e)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMousePosEvent(e.GetX(), e.GetY());

        return false;
    }

    bool ImGuiLayer::OnMouseScrolledEvent(MouseScrolledEvent &e)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseWheelEvent(e.GetXOffset(), e.GetYOffset());

        return false;
    }

    bool ImGuiLayer::OnKeyPressedEvent(KeyPressedEvent &e)
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiKey imguiKey = ImGui_ImplGlfw_KeyToImGuiKey(e.GetKeyCode());
        if (imguiKey != ImGuiKey_None)
            io.AddKeyEvent(imguiKey, true);

        return false;
    }

    bool ImGuiLayer::OnKeyReleasedEvent(KeyReleasedEvent &e)
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiKey imguiKey = ImGui_ImplGlfw_KeyToImGuiKey(e.GetKeyCode());
        if (imguiKey != ImGuiKey_None)
            io.AddKeyEvent(imguiKey, false);
            
        return false;
    }

    bool ImGuiLayer::OnKeyTypedEvent(KeyTypedEvent& e)
    {
        ImGuiIO& io = ImGui::GetIO();
        unsigned int key = e.GetKeyCode();

        if (key > 0 && key < 0x10000)
            io.AddInputCharacter(key);

        return false;
    }

    bool ImGuiLayer::OnWindowResizeEvent(WindowResizeEvent &e)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2((float)e.GetWidth(), (float)e.GetHeight());
        io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
        glViewport(0, 0, (int)e.GetWidth(), (int)e.GetHeight());

        return false;
    }

    ImGuiKey ImGui_ImplGlfw_KeyToImGuiKey(int keycode)
    {
        switch (keycode)
        {
            case MYKA_KEY_TAB: return ImGuiKey_Tab;
            case MYKA_KEY_LEFT: return ImGuiKey_LeftArrow;
            case MYKA_KEY_RIGHT: return ImGuiKey_RightArrow;
            case MYKA_KEY_UP: return ImGuiKey_UpArrow;
            case MYKA_KEY_DOWN: return ImGuiKey_DownArrow;
            case MYKA_KEY_PAGE_UP: return ImGuiKey_PageUp;
            case MYKA_KEY_PAGE_DOWN: return ImGuiKey_PageDown;
            case MYKA_KEY_HOME: return ImGuiKey_Home;
            case MYKA_KEY_END: return ImGuiKey_End;
            case MYKA_KEY_INSERT: return ImGuiKey_Insert;
            case MYKA_KEY_DELETE: return ImGuiKey_Delete;
            case MYKA_KEY_BACKSPACE: return ImGuiKey_Backspace;
            case MYKA_KEY_SPACE: return ImGuiKey_Space;
            case MYKA_KEY_ENTER: return ImGuiKey_Enter;
            case MYKA_KEY_ESCAPE: return ImGuiKey_Escape;
            case MYKA_KEY_APOSTROPHE: return ImGuiKey_Apostrophe;
            case MYKA_KEY_COMMA: return ImGuiKey_Comma;
            case MYKA_KEY_MINUS: return ImGuiKey_Minus;
            case MYKA_KEY_PERIOD: return ImGuiKey_Period;
            case MYKA_KEY_SLASH: return ImGuiKey_Slash;
            case MYKA_KEY_SEMICOLON: return ImGuiKey_Semicolon;
            case MYKA_KEY_EQUAL: return ImGuiKey_Equal;
            case MYKA_KEY_LEFT_BRACKET: return ImGuiKey_LeftBracket;
            case MYKA_KEY_BACKSLASH: return ImGuiKey_Backslash;
            case MYKA_KEY_WORLD_1: return ImGuiKey_Oem102;
            case MYKA_KEY_WORLD_2: return ImGuiKey_Oem102;
            case MYKA_KEY_RIGHT_BRACKET: return ImGuiKey_RightBracket;
            case MYKA_KEY_GRAVE_ACCENT: return ImGuiKey_GraveAccent;
            case MYKA_KEY_CAPS_LOCK: return ImGuiKey_CapsLock;
            case MYKA_KEY_SCROLL_LOCK: return ImGuiKey_ScrollLock;
            case MYKA_KEY_NUM_LOCK: return ImGuiKey_NumLock;
            case MYKA_KEY_PRINT_SCREEN: return ImGuiKey_PrintScreen;
            case MYKA_KEY_PAUSE: return ImGuiKey_Pause;
            case MYKA_KEY_KP_0: return ImGuiKey_Keypad0;
            case MYKA_KEY_KP_1: return ImGuiKey_Keypad1;
            case MYKA_KEY_KP_2: return ImGuiKey_Keypad2;
            case MYKA_KEY_KP_3: return ImGuiKey_Keypad3;
            case MYKA_KEY_KP_4: return ImGuiKey_Keypad4;
            case MYKA_KEY_KP_5: return ImGuiKey_Keypad5;
            case MYKA_KEY_KP_6: return ImGuiKey_Keypad6;
            case MYKA_KEY_KP_7: return ImGuiKey_Keypad7;
            case MYKA_KEY_KP_8: return ImGuiKey_Keypad8;
            case MYKA_KEY_KP_9: return ImGuiKey_Keypad9;
            case MYKA_KEY_KP_DECIMAL: return ImGuiKey_KeypadDecimal;
            case MYKA_KEY_KP_DIVIDE: return ImGuiKey_KeypadDivide;
            case MYKA_KEY_KP_MULTIPLY: return ImGuiKey_KeypadMultiply;
            case MYKA_KEY_KP_SUBTRACT: return ImGuiKey_KeypadSubtract;
            case MYKA_KEY_KP_ADD: return ImGuiKey_KeypadAdd;
            case MYKA_KEY_KP_ENTER: return ImGuiKey_KeypadEnter;
            case MYKA_KEY_KP_EQUAL: return ImGuiKey_KeypadEqual;
            case MYKA_KEY_LEFT_SHIFT: return ImGuiKey_LeftShift;
            case MYKA_KEY_LEFT_CONTROL: return ImGuiKey_LeftCtrl;
            case MYKA_KEY_LEFT_ALT: return ImGuiKey_LeftAlt;
            case MYKA_KEY_LEFT_SUPER: return ImGuiKey_LeftSuper;
            case MYKA_KEY_RIGHT_SHIFT: return ImGuiKey_RightShift;
            case MYKA_KEY_RIGHT_CONTROL: return ImGuiKey_RightCtrl;
            case MYKA_KEY_RIGHT_ALT: return ImGuiKey_RightAlt;
            case MYKA_KEY_RIGHT_SUPER: return ImGuiKey_RightSuper;
            case MYKA_KEY_MENU: return ImGuiKey_Menu;
            case MYKA_KEY_0: return ImGuiKey_0;
            case MYKA_KEY_1: return ImGuiKey_1;
            case MYKA_KEY_2: return ImGuiKey_2;
            case MYKA_KEY_3: return ImGuiKey_3;
            case MYKA_KEY_4: return ImGuiKey_4;
            case MYKA_KEY_5: return ImGuiKey_5;
            case MYKA_KEY_6: return ImGuiKey_6;
            case MYKA_KEY_7: return ImGuiKey_7;
            case MYKA_KEY_8: return ImGuiKey_8;
            case MYKA_KEY_9: return ImGuiKey_9;
            case MYKA_KEY_A: return ImGuiKey_A;
            case MYKA_KEY_B: return ImGuiKey_B;
            case MYKA_KEY_C: return ImGuiKey_C;
            case MYKA_KEY_D: return ImGuiKey_D;
            case MYKA_KEY_E: return ImGuiKey_E;
            case MYKA_KEY_F: return ImGuiKey_F;
            case MYKA_KEY_G: return ImGuiKey_G;
            case MYKA_KEY_H: return ImGuiKey_H;
            case MYKA_KEY_I: return ImGuiKey_I;
            case MYKA_KEY_J: return ImGuiKey_J;
            case MYKA_KEY_K: return ImGuiKey_K;
            case MYKA_KEY_L: return ImGuiKey_L;
            case MYKA_KEY_M: return ImGuiKey_M;
            case MYKA_KEY_N: return ImGuiKey_N;
            case MYKA_KEY_O: return ImGuiKey_O;
            case MYKA_KEY_P: return ImGuiKey_P;
            case MYKA_KEY_Q: return ImGuiKey_Q;
            case MYKA_KEY_R: return ImGuiKey_R;
            case MYKA_KEY_S: return ImGuiKey_S;
            case MYKA_KEY_T: return ImGuiKey_T;
            case MYKA_KEY_U: return ImGuiKey_U;
            case MYKA_KEY_V: return ImGuiKey_V;
            case MYKA_KEY_W: return ImGuiKey_W;
            case MYKA_KEY_X: return ImGuiKey_X;
            case MYKA_KEY_Y: return ImGuiKey_Y;
            case MYKA_KEY_Z: return ImGuiKey_Z;
            case MYKA_KEY_F1: return ImGuiKey_F1;
            case MYKA_KEY_F2: return ImGuiKey_F2;
            case MYKA_KEY_F3: return ImGuiKey_F3;
            case MYKA_KEY_F4: return ImGuiKey_F4;
            case MYKA_KEY_F5: return ImGuiKey_F5;
            case MYKA_KEY_F6: return ImGuiKey_F6;
            case MYKA_KEY_F7: return ImGuiKey_F7;
            case MYKA_KEY_F8: return ImGuiKey_F8;
            case MYKA_KEY_F9: return ImGuiKey_F9;
            case MYKA_KEY_F10: return ImGuiKey_F10;
            case MYKA_KEY_F11: return ImGuiKey_F11;
            case MYKA_KEY_F12: return ImGuiKey_F12;
            case MYKA_KEY_F13: return ImGuiKey_F13;
            case MYKA_KEY_F14: return ImGuiKey_F14;
            case MYKA_KEY_F15: return ImGuiKey_F15;
            case MYKA_KEY_F16: return ImGuiKey_F16;
            case MYKA_KEY_F17: return ImGuiKey_F17;
            case MYKA_KEY_F18: return ImGuiKey_F18;
            case MYKA_KEY_F19: return ImGuiKey_F19;
            case MYKA_KEY_F20: return ImGuiKey_F20;
            case MYKA_KEY_F21: return ImGuiKey_F21;
            case MYKA_KEY_F22: return ImGuiKey_F22;
            case MYKA_KEY_F23: return ImGuiKey_F23;
            case MYKA_KEY_F24: return ImGuiKey_F24;
            default: return ImGuiKey_None;
        }
    }
} // namespace Myka
