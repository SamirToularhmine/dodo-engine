#include <Core/Application.h>

int main(int argc, char const *argv[])
{
    auto application = dodo::Application::Create();
    application->Init();
    return 0;
}
