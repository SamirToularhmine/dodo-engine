#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Platform/Vulkan/VulkanPhysicalDevice.h>

#include <volk.h>


DODO_BEGIN_NAMESPACE

class VulkanDevice
{
public:
    struct Queues
    {
        VkQueue m_GraphicsFamilyQueue{ VK_NULL_HANDLE };
        VkQueue m_PresentationQueue{ VK_NULL_HANDLE };
    };

    VulkanDevice(const VulkanPhysicalDevice& _vkPhysicalDevice, const VulkanInstance& _vkInstance);
    ~VulkanDevice();

    const Queues& GetQueues() const { return m_VkQueues; }

    operator const VkDevice& () const { return m_VkDevice; }
private:
    VkDevice m_VkDevice;
    Queues m_VkQueues;
};

DODO_END_NAMESPACE