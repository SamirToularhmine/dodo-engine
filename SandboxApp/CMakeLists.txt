cmake_minimum_required(VERSION 3.29)

project(
    SandboxApp
    VERSION 0.1 
    LANGUAGES CXX
)

add_executable(SandboxApp src/main.cpp)
target_link_libraries(SandboxApp PUBLIC DodoEngine)

set_property(TARGET SandboxApp PROPERTY CXX_STANDARD 23)