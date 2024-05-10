#include <Core/Application.h>

#include <print>

DODO_BEGIN_NAMESPACE

std::unique_ptr<Application> Application::Create() {
    return std::make_unique<Application>();
}

void Application::Init() {
    std::println("Initializing DodoENngine...");
}

DODO_END_NAMESPACE