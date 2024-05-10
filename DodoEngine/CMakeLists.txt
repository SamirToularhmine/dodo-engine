cmake_minimum_required(VERSION 3.29)
include(FetchContent)

project(
    DodoEngine 
    VERSION 0.1 
    LANGUAGES CXX
)

set(INCLUDE_LIST "src")
file(GLOB_RECURSE SOURCE_LIST *.cpp)
file(GLOB_RECURSE HEADER_LIST *.h)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 7b6aead9fb88b3623e3b3725ebb42670cbe4c579
    OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(glfw)

add_library(DodoEngine STATIC ${SOURCE_LIST} ${HEADER_LIST})
target_link_libraries(glfw)
target_include_directories(DodoEngine PUBLIC ${INCLUDE_LIST})

set_property(TARGET DodoEngine PROPERTY CXX_STANDARD 23)
