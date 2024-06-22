#include <DodoEngine/Core/Window.h>
#include <DodoEngine/Utils/Log.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <vector>

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
        NULL, NULL);

    glfwMakeContextCurrent(m_NativeWindow);

    m_VulkanContext.Init(m_NativeWindow);

    DODO_INFO("Window initialized successfully");
}

void Window::PollEvents() const {
    glfwPollEvents();
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