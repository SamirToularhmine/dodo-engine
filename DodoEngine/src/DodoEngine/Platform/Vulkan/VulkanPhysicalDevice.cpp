#include <DodoEngine/Platform/Vulkan/VulkanPhysicalDevice.h>

#include <DodoEngine/Platform/Vulkan/VulkanInstance.h>
#include <DodoEngine/Platform/Vulkan/VulkanSurface.h>
#include <DodoEngine/Utils/Log.h>


DODO_BEGIN_NAMESPACE

VulkanPhysicalDevice::VulkanPhysicalDevice(const VulkanInstance& _vulkanInstance, const VulkanSurface& _vulkanSurface)
{
    VulkanDevices availablePhysicalDevices = GetAvailablePhysicalDevices(_vulkanInstance);
    m_VkPhysicalDevice = availablePhysicalDevices.front();

    if(m_VkPhysicalDevice == VK_NULL_HANDLE)
    {
        DODO_CRITICAL("Could not find a suitable physical device");
    }

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_VkPhysicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_VkPhysicalDevice, &queueFamilyCount, queueFamilies.data());

    // TODO: Refacto 
    uint32_t i = 0;
    QueueFamilyIndices& queueFamilyIndices = m_VkQueuesInfo.m_QueueFamilyIndices; 
    for(const auto& queueFamilyProperties : queueFamilies)
    {
        // Graphics queue support
        if(queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            queueFamilyIndices.graphicsFamily = i;
        }

        // Presentation queue support
        VkBool32 presentationQueueSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(m_VkPhysicalDevice, i, _vulkanSurface, &presentationQueueSupport);
        if(presentationQueueSupport) {
            queueFamilyIndices.presentFamily = i;
        }

        if(queueFamilyIndices.isComplete()) {
            break;
        }

        ++i;
    }

    if(!queueFamilyIndices.isComplete()) {
        DODO_CRITICAL("No graphics queue family found in the selected device");
    }
}

uint32_t VulkanPhysicalDevice::FindMemoryType(uint32_t _typeFilter, VkMemoryPropertyFlags properties) const
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(m_VkPhysicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if (_typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    DODO_CRITICAL("Could not find a memory type with the specified filter");
}

VulkanPhysicalDevice::VulkanDevices VulkanPhysicalDevice::GetAvailablePhysicalDevices(const VulkanInstance& _vulkanInstance)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(_vulkanInstance, &deviceCount, nullptr);

    if(deviceCount == 0) 
    {
        DODO_CRITICAL("No Vulkan-compatible device found");
    }

    VulkanDevices physicalDevices(deviceCount);
    vkEnumeratePhysicalDevices(_vulkanInstance, &deviceCount, physicalDevices.data());

    return physicalDevices;
}

bool VulkanPhysicalDevice::IsSuitable(const VkPhysicalDevice& _vkPhysicalDevice)
{
    // TODO: perform device suitability checks
    return true;
}

DODO_END_NAMESPACE
