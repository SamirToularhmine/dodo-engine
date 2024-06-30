#include <DodoEngine/Core/Application.h>

#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Platform/Vulkan/VulkanRenderer.h>
#include <DodoEngine/Utils/Log.h>
#include <DodoEngine/Utils/Utils.h>


DODO_BEGIN_NAMESPACE

std::unique_ptr<Application> Application::Create() {
    return std::make_unique<Application>();
}

void Application::Init(const WindowProps& _windowProps) {
    DODO_INFO("Initializing DodoEngine...");

    m_Window = std::make_unique<Window>();
    m_Window->Init(_windowProps);

    m_Renderer = std::make_unique<VulkanRenderer>(VulkanContext::Get());
    m_Renderer->Init(m_Window->GetNativeWindow());

    m_Camera = std::make_unique<Camera>(glm::vec3{2, 2, 2});
}

void Application::Run() {
	while(!m_Window->ShouldClose()) 
    {
        m_Window->Update();

        const float time = m_Window->GetTime();
        float deltaTime = time - m_LastFrameTime;
        m_LastFrameTime = time;

        // std::cout << "\r" << deltaTime * 1000;

        m_Camera->Update(deltaTime);
		m_Renderer->Update(*m_Camera, deltaTime);

        m_Window->SwapBuffers();
	}

    m_Window->Shutdown();
    m_Renderer->Shutdown();
}

DODO_END_NAMESPACE
