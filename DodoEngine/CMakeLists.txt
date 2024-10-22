cmake_minimum_required(VERSION 3.21)
include(FetchContent)

project(
    DodoEngine 
    VERSION 0.1 
    LANGUAGES CXX
)

set(CMAKE_BUILD_TYPE Debug)

# GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 7b6aead9fb88b3623e3b3725ebb42670cbe4c579
)

# Volk loader
FetchContent_Declare(
    volk
    GIT_REPOSITORY https://github.com/zeux/volk.git
    GIT_TAG master
)

# GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY git@github.com:g-truc/glm.git
    GIT_TAG 0af55ccecd98d4e5a8d1fad7de25ba429d60e863
)

# Vulkan Memory Allocator
FetchContent_Declare(
    VulkanMemoryAllocator 
    GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
    GIT_TAG 009ecd192c1289c7529bff248a16cfe896254816
)

# libdeflate
FetchContent_Declare(
    libdeflate
    GIT_REPOSITORY git@github.com:ebiggers/libdeflate.git
    GIT_TAG 275aa5141db6eda3587214e0f1d3a134768f557d
    OVERRIDE_FIND_PACKAGE
)

# OpenFBX
FetchContent_Declare(
    OpenFBX 
    GIT_REPOSITORY git@github.com:nem0/OpenFBX.git
    GIT_TAG ef492927cefc16ad8733ec08bcc8df5434dbb1ae
)

# tinyobjloader
FetchContent_Declare(
    tinyobjloader 
    GIT_REPOSITORY git@github.com:tinyobjloader/tinyobjloader.git
    GIT_TAG 9d9594598a99d4d7d7039bc504f7b05b490596c4
)

# ImGui
FetchContent_Declare(
    imgui 
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG b0185efd248e5d56a6728da6a4746d5826a14370
)

# tinygltf
FetchContent_Declare(
    tinygltf 
    GIT_REPOSITORY https://github.com/syoyo/tinygltf.git
    GIT_TAG fea67861296293e33e6caee81682261a2700136a
)

# entt
FetchContent_Declare(
    entt 
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG 78213075654a688e9da6bc49f7f873d25c26d12c
)

# imguizmo
FetchContent_Declare(
    imguizmo
    GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo.git
    GIT_TAG 6d588209f99b1324a608783d1f52faa518886c29
)

FetchContent_MakeAvailable(glfw volk glm VulkanMemoryAllocator OpenFBX tinyobjloader imgui tinygltf entt imguizmo)

if (WIN32)
   set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_WIN32_KHR)
endif()


set(IMGUI_SOURCE_LIST
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
    ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.h
    ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp

    ${imguizmo_SOURCE_DIR}/ImGuizmo.h
    ${imguizmo_SOURCE_DIR}/ImGuizmo.cpp
    ${imguizmo_SOURCE_DIR}/ImSequencer.cpp
    ${imguizmo_SOURCE_DIR}/ImSequencer.h
    ${imguizmo_SOURCE_DIR}/ImGradient.h
    ${imguizmo_SOURCE_DIR}/ImGradient.cpp
    ${imguizmo_SOURCE_DIR}/ImCurveEdit.h
    ${imguizmo_SOURCE_DIR}/ImCurveEdit.cpp
    ${imguizmo_SOURCE_DIR}/GraphEditor.h
    ${imguizmo_SOURCE_DIR}/GraphEditor.cpp
)

set(INCLUDE_LIST "src" "vendor" ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends ${imguizmo_SOURCE_DIR})
file(GLOB_RECURSE SOURCE_LIST *.cpp *.tpp *.c)
file(GLOB_RECURSE HEADER_LIST *.h)

add_library(DodoEngine STATIC ${SOURCE_LIST} ${HEADER_LIST} ${IMGUI_SOURCE_LIST}  "src/DodoEngine/Renderer/AssetManager.h" "src/DodoEngine/Renderer/Asset.h")
target_link_libraries(DodoEngine glfw volk glm GPUOpen::VulkanMemoryAllocator libdeflate::libdeflate_static OpenFBX tinyobjloader tinygltf EnTT::EnTT)
target_compile_definitions(DodoEngine PUBLIC IMGUI_IMPL_VULKAN_USE_VOLK GLFW_INCLUDE_NONE)
target_include_directories(DodoEngine PUBLIC ${INCLUDE_LIST})

set_property(TARGET DodoEngine PROPERTY CXX_STANDARD 23)