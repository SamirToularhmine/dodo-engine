#include <DodoEngine/Core/Application.h>
#include <DodoEngine/Platform/VulkanContext.h>
#include <DodoEngine/Utils/Log.h>


DODO_BEGIN_NAMESPACE

std::unique_ptr<Application> Application::Create() {
    return std::make_unique<Application>();
}

void Application::Init(const WindowProps& _windowProps) {
    DODO_INFO("Initializing DodoEngine...");

    m_Window = std::make_unique<Window>();
    m_Window->Init({800, 600, "Dodo Engine"});

    m_GraphicContext = std::make_unique<VulkanContext>();
    m_GraphicContext->Init(m_Window->GetNativeWindow());
}

void Application::Run() {
    while(!m_Window->ShouldClose()) {
        m_Window->PollEvents();
        m_GraphicContext->Update();
    }
}

DODO_END_NAMESPACE
