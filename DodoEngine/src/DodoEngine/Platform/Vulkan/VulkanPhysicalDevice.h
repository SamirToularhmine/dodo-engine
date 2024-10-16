#pragma once

#include <DodoEngine/Core/Types.h>

#define VK_NO_PROTOTYPES
#include <volk.h>

#include <optional>
#include <vector>


DODO_BEGIN_NAMESPACE

class VulkanInstance;
class VulkanSurface;

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

struct QueuesInfo {
    QueueFamilyIndices m_QueueFamilyIndices;
    QueueFamilyPriorities m_QueueFamilyPriorities;
};

class VulkanPhysicalDevice
{
public:
    using VulkanDevices = std::vector<VkPhysicalDevice>;
    
    VulkanPhysicalDevice(const VulkanInstance& _vulkanInstance, const VulkanSurface& _vulkanSurface);
    ~VulkanPhysicalDevice() = default;

    const QueuesInfo& GetQueues() const { return m_VkQueuesInfo; }

    uint32_t FindMemoryType(uint32_t _typeFilter, VkMemoryPropertyFlags properties) const;

    VkPhysicalDeviceProperties GetProperties() const;

    VkFormatProperties GetFormatProperties(VkFormat _format) const;

    operator const VkPhysicalDevice&() const { return m_VkPhysicalDevice; }

private:
    static VulkanDevices GetAvailablePhysicalDevices(const VulkanInstance& _vulkanInstance);

    static bool IsSuitable(const VkPhysicalDevice& _vkPhysicalDevice);

private:
    VkPhysicalDevice m_VkPhysicalDevice;
    QueuesInfo m_VkQueuesInfo;
};

DODO_END_NAMESPACE
