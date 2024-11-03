cmake_minimum_required(VERSION 3.30)

project(
    DodoEngine 
    VERSION 0.1 
    LANGUAGES CXX
)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/local/opt/llvm/lib/c++ -Wl,-rpath,/usr/local/opt/llvm/lib/c++")

add_subdirectory(DodoEngine)
add_subdirectory(SandboxApp)