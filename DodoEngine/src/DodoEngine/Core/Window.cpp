#include <DodoEngine/Core/Window.h>

#include <DodoEngine/Core/GlfwCallbacks.h>
#include <DodoEngine/Core/InputManager.h>
#include <DodoEngine/Utils/Log.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Key.h"


DODO_BEGIN_NAMESPACE
	void Window::Init(const WindowProps& _windowProps) {
    DODO_INFO("Initializing window...");

    if(!glfwInit()) {
        DODO_CRITICAL("Can't initialize GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_NativeWindow = glfwCreateWindow(
        _windowProps.m_Width,
        _windowProps.m_Height,
        _windowProps.m_Name.c_str(),
        nullptr, nullptr);

    glfwMakeContextCurrent(m_NativeWindow);
    glfwSwapInterval(_windowProps.m_VSync);

    glfwSetKeyCallback(m_NativeWindow, key_callback);
    glfwSetCursorPosCallback(m_NativeWindow, cursor_position_callback);
    glfwSetMouseButtonCallback(m_NativeWindow, mouse_button_callback);
    glfwSetScrollCallback(m_NativeWindow, scroll_callback);

    DODO_INFO("Window initialized successfully");
}

void Window::PollEvents() const {
    glfwPollEvents();
}

void Window::Update() const
{
    PollEvents();

    InputManager& inputManager = InputManager::Get();
    if(inputManager.IsKeyDown(DODO_KEY_ESCAPE))
    {
        glfwSetWindowShouldClose(m_NativeWindow, true);
    }
}

void Window::Shutdown() const
{
    glfwTerminate();
}

float Window::GetTime() const
{
    return glfwGetTime();
}

bool Window::ShouldClose() const {
    return glfwWindowShouldClose(m_NativeWindow);
}

void Window::SwapBuffers() const {
    glfwSwapBuffers(m_NativeWindow);
}

GLFWwindow* Window::GetNativeWindow() const {
    return m_NativeWindow;
}

DODO_END_NAMESPACE