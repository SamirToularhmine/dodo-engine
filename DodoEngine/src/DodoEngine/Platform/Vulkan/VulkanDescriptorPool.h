#pragma once

#include <DodoEngine/Core/Types.h>

#define VK_NO_PROTOTYPES
#include <volk.h>


DODO_BEGIN_NAMESPACE

class VulkanDevice;

class VulkanDescriptorPool
{
public:
	VulkanDescriptorPool(uint32_t _descriptorCount, const Ref<VulkanDevice> _vulkanDevice);

	~VulkanDescriptorPool();

	operator const VkDescriptorPool& () const { return m_VkDescriptorPool; }

private:
	VkDescriptorPool m_VkDescriptorPool;
	Ref<VulkanDevice> m_VulkanDevice;
};

DODO_END_NAMESPACE