#include <DodoEngine/Core/Application.h>

int main(int argc, char const *argv[])
{
    auto application = dodo::Application::Create();
    application->Init({800, 600, "Dodo Test Application"});
    application->Run();

    return 0;
}
