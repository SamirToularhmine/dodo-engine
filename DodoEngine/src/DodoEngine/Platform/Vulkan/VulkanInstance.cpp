#include <DodoEngine/Platform/Vulkan/VulkanInstance.h>
#include <DodoEngine/Utils/Log.h>

#include <GLFW/glfw3.h>

#include <vector>


DODO_BEGIN_NAMESPACE

VulkanInstance::VulkanInstance(const VkApplicationInfo& _applicationInfo) 
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    
    m_InstanceExtensions.assign(glfwExtensions, glfwExtensions + glfwExtensionCount);
    m_InstanceExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

    m_ValidationLayers.emplace_back("VK_LAYER_KHRONOS_validation");

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &_applicationInfo;
    createInfo.enabledExtensionCount = m_InstanceExtensions.size();
    createInfo.ppEnabledExtensionNames = m_InstanceExtensions.data();
    createInfo.enabledLayerCount = m_ValidationLayers.size();
    createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

    VkResult vkCreateResult = vkCreateInstance(&createInfo, nullptr, &m_VkInstance);

    if(vkCreateResult != VK_SUCCESS) 
    {
        DODO_CRITICAL("Failed to create Vulkan instance");
    }

    volkLoadInstance(m_VkInstance);
}

VulkanInstance::~VulkanInstance()
{
    vkDestroyInstance(m_VkInstance, nullptr);
}

DODO_END_NAMESPACE