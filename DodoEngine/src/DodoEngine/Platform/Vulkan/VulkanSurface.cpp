#include <DodoEngine/Platform/Vulkan/VulkanSurface.h>

#include <DodoEngine/Platform/Vulkan/VulkanInstance.h>
#include <DodoEngine/Utils/Log.h>


DODO_BEGIN_NAMESPACE

VulkanSurface::VulkanSurface(Ref<VulkanInstance> _vulkanInstance, GLFWwindow* _glfwWindow)
{
    m_VulkanInstanceRef = _vulkanInstance;

    VkResult surfaceCreationResult = glfwCreateWindowSurface(*_vulkanInstance, _glfwWindow, nullptr, &m_VkWindowSurface);
    if(surfaceCreationResult != VK_SUCCESS)
    {
        DODO_CRITICAL("Cannot create the window surface from the provided GLFWwindow and the current Vulkan instance");
    }
}

VulkanSurface::~VulkanSurface()
{
    vkDestroySurfaceKHR(*m_VulkanInstanceRef, m_VkWindowSurface, nullptr);
}

DODO_END_NAMESPACE
