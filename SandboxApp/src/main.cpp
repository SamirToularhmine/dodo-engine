#include <DodoEngine/Core/Application.h>

int main(int argc, char const *argv[])
{
    srand((unsigned)time(NULL));
    auto application = dodo::Application::Create();
    application->Init(dodo::WindowProps{1920, 1080, "Dodo Engine", true, false});
    application->Run();

    return 0;
}
