#include <DodoEngine/Core/Application.h>

#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Platform/Vulkan/VulkanRenderer.h>
#include <DodoEngine/Utils/Log.h>


DODO_BEGIN_NAMESPACE

std::unique_ptr<Application> Application::Create() {
    return std::make_unique<Application>();
}

void Application::Init(const WindowProps& _windowProps) {
    DODO_INFO("Initializing DodoEngine...");

    m_Window = std::make_unique<Window>();
    m_Window->Init({800, 600, "Dodo Engine"});

    m_Renderer = std::make_unique<VulkanRenderer>(VulkanContext::Get());
    m_Renderer->Init(m_Window->GetNativeWindow());

    // m_Renderer->DrawQuad();
}

void Application::Run() {
    while(!m_Window->ShouldClose()) {
        m_Window->PollEvents();
        m_Renderer->Update();
    }

    m_Renderer->Shutdown();
}

DODO_END_NAMESPACE
