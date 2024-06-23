cmake_minimum_required(VERSION 3.21)
include(FetchContent)

project(
    DodoEngine 
    VERSION 0.1 
    LANGUAGES CXX
)

set(INCLUDE_LIST "src" "/usr/local/include")
file(GLOB_RECURSE SOURCE_LIST *.cpp)
file(GLOB_RECURSE HEADER_LIST *.h)

# GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 7b6aead9fb88b3623e3b3725ebb42670cbe4c579
)
FetchContent_MakeAvailable(glfw)

# Volk loader
FetchContent_Declare(
    volk
    GIT_REPOSITORY https://github.com/zeux/volk.git
    GIT_TAG master
)
FetchContent_MakeAvailable(volk)

# GLM
# Volk loader
FetchContent_Declare(
    glm
    GIT_REPOSITORY git@github.com:g-truc/glm.git
    GIT_TAG 0af55ccecd98d4e5a8d1fad7de25ba429d60e863
)
FetchContent_MakeAvailable(glm)

add_library(DodoEngine STATIC ${SOURCE_LIST} ${HEADER_LIST})
target_link_libraries(DodoEngine glfw volk glm)
target_include_directories(DodoEngine PUBLIC ${INCLUDE_LIST})

set_property(TARGET DodoEngine PROPERTY CXX_STANDARD 23)
