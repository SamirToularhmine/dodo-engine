#include <DodoEngine/Core/Application.h>

int main(int argc, char const *argv[])
{
    auto application = dodo::Application::Create();
    application->Init(dodo::WindowProps{800, 600, "Dodo Engine", true});
    application->Run();

    return 0;
}
