#pragma once

#include <DodoEngine/Core/Types.h>

#include <volk.h>
#include <GLFW/glfw3.h>

DODO_BEGIN_NAMESPACE

class VulkanInstance;

class VulkanSurface 
{
public:
    VulkanSurface(Ref<VulkanInstance> _vulkanInstance, GLFWwindow* _glfwWindow);
    ~VulkanSurface();

    operator const VkSurfaceKHR&() const { return m_VkWindowSurface; }

private:
    VkSurfaceKHR m_VkWindowSurface;
    Ref<VulkanInstance> m_VulkanInstanceRef;
};

DODO_END_NAMESPACE