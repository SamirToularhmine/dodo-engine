#pragma once

#include <DodoEngine/Core/GraphicContext.h>
#include <DodoEngine/Core/Window.h>
#include <DodoEngine/Utils/Utils.h>

#include <memory>

DODO_BEGIN_NAMESPACE

class Application {
    public:
        static std::unique_ptr<Application> Create();

        void Init(const WindowProps& _windowProps);

        void Run();
    private:
        std::unique_ptr<Window> m_Window;
        std::unique_ptr<GraphicContext> m_GraphicContext;
};

DODO_END_NAMESPACE