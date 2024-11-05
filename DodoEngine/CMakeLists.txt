cmake_minimum_required(VERSION 3.30)
include(FetchContent)

project(
    DodoEngine 
    VERSION 0.1 
    LANGUAGES CXX
)

# set(CMAKE_BUILD_TYPE Debug)
set(GLM_ENABLE_CXX_20 true)

if (WIN32)
   set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_WIN32_KHR)
endif()

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(volk CONFIG REQUIRED)
find_package(tinyobjloader CONFIG REQUIRED)
find_package(libdeflate CONFIG REQUIRED)
find_package(unofficial-vulkan-memory-allocator-hpp CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(imguizmo CONFIG REQUIRED)

find_path(EARCUT_HPP_INCLUDE_DIRS "mapbox/earcut.hpp")
find_path(TINYGLTF_INCLUDE_DIRS "tiny_gltf.h")

set(INCLUDE_LIST "src" "vendor")
file(GLOB_RECURSE SOURCE_LIST *.cpp *.tpp *.c)
file(GLOB_RECURSE HEADER_LIST *.h)

add_library(DodoEngine STATIC ${SOURCE_LIST} ${HEADER_LIST})
target_link_libraries(DodoEngine glfw volk::volk volk::volk_headers glm::glm unofficial::VulkanMemoryAllocator-Hpp::VulkanMemoryAllocator-Hpp libdeflate::libdeflate_static tinyobjloader::tinyobjloader EnTT::EnTT imgui::imgui imguizmo::imguizmo)
target_compile_definitions(DodoEngine PUBLIC IMGUI_IMPL_VULKAN_USE_VOLK IMGUI_IMPL_VULKAN_NO_PROTOTYPES GLFW_INCLUDE_NONE)
target_include_directories(DodoEngine PUBLIC ${INCLUDE_LIST} ${EARCUT_HPP_INCLUDE_DIRS} ${TINYGLTF_INCLUDE_DIRS})

set_property(TARGET DodoEngine PROPERTY CXX_STANDARD 23)