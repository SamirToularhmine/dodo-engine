#pragma once

#include <DodoEngine/Core/Types.h>

#include <volk.h>


DODO_BEGIN_NAMESPACE

class VulkanDevice;

class VulkanDescriptorSetLayout 
{
public:
    VulkanDescriptorSetLayout(const Ref<VulkanDevice>& _vulkanDevice);
    ~VulkanDescriptorSetLayout();

    operator VkDescriptorSetLayout&() { return m_VkDescriptorSetLayout; }
private:
    VkDescriptorSetLayout m_VkDescriptorSetLayout;
    Ref<VulkanDevice> m_VulkanDevice;
};

DODO_END_NAMESPACE
