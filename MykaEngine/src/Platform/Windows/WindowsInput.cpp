#include "mykapch.hpp"
#include "MykaEngine/Core/Input.hpp"
#include "MykaEngine/Core/Application.hpp"

#include <GLFW/glfw3.h>

namespace Myka
{
    bool Input::IsKeyPressed(int keycode)
    {
        auto window = static_cast<GLFWwindow *>(Application::Get().GetWindow().GetNativeWindow());
        auto state = glfwGetKey(window, keycode);
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool Input::IsMouseButtonPressed(int button)
    {
        auto window = static_cast<GLFWwindow *>(Application::Get().GetWindow().GetNativeWindow());
        auto state = glfwGetMouseButton(window, button);
        return state == GLFW_PRESS;
    }

    std::pair<float, float> Input::GetMousePosition()
    {
        auto window = static_cast<GLFWwindow *>(Application::Get().GetWindow().GetNativeWindow());
        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);

        return {(float)xPos, (float)yPos};
    }

    float Input::GetMouseX()
    {
        auto [xPos, yPos] = GetMousePosition();

        return xPos;
    }

    float Input::GetMouseY()
    {
        auto [xPos, yPos] = GetMousePosition();

        return yPos;
    }

} // namespace Myka
