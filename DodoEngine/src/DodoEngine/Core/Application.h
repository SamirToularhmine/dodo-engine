#pragma once

#include <DodoEngine/Core/Camera.h>
#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Core/Window.h>
#include <DodoEngine/Renderer/Renderer.h>

#include <memory>

DODO_BEGIN_NAMESPACE

class Application {
public:
    static std::unique_ptr<Application> Create();

    void Init(const WindowProps& _windowProps);

    void Run();
private:
	std::unique_ptr<Window> m_Window;
    std::unique_ptr<Renderer> m_Renderer;
    std::unique_ptr<Camera> m_Camera;

    float m_LastFrameTime;
};

DODO_END_NAMESPACE