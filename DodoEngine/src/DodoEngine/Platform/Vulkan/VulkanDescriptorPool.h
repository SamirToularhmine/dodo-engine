#pragma once

#include <DodoEngine/Core/Types.h>

#include <volk.h>


DODO_BEGIN_NAMESPACE

class VulkanDevice;

class VulkanDescriptorPool
{
public:
	VulkanDescriptorPool(uint32_t _descriptorCount, Ref<VulkanDevice>& _vulkanDevice);

	~VulkanDescriptorPool();

	operator VkDescriptorPool& () { return m_VkDescriptorPool; }

private:
	VkDescriptorPool m_VkDescriptorPool;
	Ref<VulkanDevice> m_VulkanDevice;
};

DODO_END_NAMESPACE