#include <DodoEngine/Platform/Vulkan/VulkanDevice.h>

#include <DodoEngine/Platform/Vulkan/VulkanInstance.h>
#include <DodoEngine/Platform/Vulkan/VulkanPhysicalDevice.h>

#include <DodoEngine/Utils/Log.h>

#include <set>

DODO_BEGIN_NAMESPACE

VulkanDevice::VulkanDevice(const VulkanPhysicalDevice& _vkPhysicalDevice, const VulkanInstance& _vkInstance)
{
    const QueuesInfo& deviceQueues = _vkPhysicalDevice.GetQueues();
    QueueFamilyIndices queueFamilyIndices = deviceQueues.m_QueueFamilyIndices;

    std::vector<VkDeviceQueueCreateInfo> deviceQueuesCreateInfo{};
    std::set supportedQueueTypesIndex = {
        queueFamilyIndices.graphicsFamily.value(),
        queueFamilyIndices.presentFamily.value()
    };

    for (const QueueIndex& queueFamily : supportedQueueTypesIndex) {
        VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
        deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        deviceQueueCreateInfo.queueFamilyIndex = queueFamily;
        deviceQueueCreateInfo.queueCount = 1;
        // TODO: Handle different priorities
        deviceQueueCreateInfo.pQueuePriorities = &QueueFamilyPriorities::k_DEFAULT_PRIORITY;
        deviceQueuesCreateInfo.emplace_back(deviceQueueCreateInfo);
    }

    VulkanExtensions requiredDeviceExtension = {
        "VK_KHR_portability_subset",
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VulkanValidationLayers requestedValidationLayers = _vkInstance.GetValidationLayers();

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = deviceQueuesCreateInfo.data();
    deviceCreateInfo.queueCreateInfoCount = deviceQueuesCreateInfo.size();
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.enabledExtensionCount = requiredDeviceExtension.size();
    deviceCreateInfo.ppEnabledExtensionNames = requiredDeviceExtension.data();
    deviceCreateInfo.enabledLayerCount = requestedValidationLayers.size();
    deviceCreateInfo.ppEnabledLayerNames = requestedValidationLayers.data();

    VkResult createDeviceResult = vkCreateDevice(_vkPhysicalDevice, &deviceCreateInfo, nullptr, &m_VkDevice);
    if (createDeviceResult != VK_SUCCESS)
    {
        DODO_CRITICAL("Cannot create the logical Vulkan device ", (int)createDeviceResult);
    }

    // Graphics queue handle
    QueueIndex graphicsFamilyQueueIndex = queueFamilyIndices.graphicsFamily.value();
    vkGetDeviceQueue(m_VkDevice, graphicsFamilyQueueIndex, 0, &m_VkQueues.m_GraphicsFamilyQueue);

    // Presentation queue handle
    QueueIndex presentationQueueIndex = queueFamilyIndices.presentFamily.value();
    vkGetDeviceQueue(m_VkDevice, presentationQueueIndex, 0, &m_VkQueues.m_PresentationQueue);
}

VulkanDevice::~VulkanDevice()
{
    vkDestroyDevice(m_VkDevice, nullptr);
}

DODO_END_NAMESPACE
