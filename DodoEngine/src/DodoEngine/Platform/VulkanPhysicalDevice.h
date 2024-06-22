#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Platform/VulkanInstance.h>
#include <DodoEngine/Platform/VulkanSurface.h>

#include <volk.h>

#include <optional>
#include <vector>


DODO_BEGIN_NAMESPACE

using QueueIndex = uint32_t;

struct QueueFamilyPriorities {
    constexpr static float k_DEFAULT_PRIORITY = 1.0f;
    
    float m_GraphicsFamilyQueuePriority{ 1.0f };
};

struct QueueFamilyIndices {
    std::optional<QueueIndex> graphicsFamily;
    std::optional<QueueIndex> presentFamily;

    bool isComplete() const { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

struct Queues {
    QueueFamilyIndices m_QueueFamilyIndices;
    QueueFamilyPriorities m_QueueFamilyPriorities;
    VkQueue m_GraphicsFamilyQueue{VK_NULL_HANDLE};
    VkQueue m_PresentationQueue{VK_NULL_HANDLE};
};

class VulkanPhysicalDevice
{
public:
    using VulkanDevices = std::vector<VkPhysicalDevice>;
    
    VulkanPhysicalDevice(const VulkanInstance& _vulkanInstance, const VulkanSurface& _vulkanSurface);
    ~VulkanPhysicalDevice() = default;

    Queues& GetQueues() { return m_VkQueues; }

    operator const VkPhysicalDevice&() const { return m_VkPhysicalDevice; }

private:
    static VulkanDevices GetAvailablePhysicalDevices(const VulkanInstance& _vulkanInstance);

    static bool IsSuitable(const VkPhysicalDevice& _vkPhysicalDevice);

private:
    VkPhysicalDevice m_VkPhysicalDevice;
    Queues m_VkQueues;
};

DODO_END_NAMESPACE
