#include <DodoEngine/Platform/Vulkan/VulkanDescriptorPool.h>

#include <DodoEngine/Platform/Vulkan/VulkanDevice.h>
#include <DodoEngine/Utils/Log.h>


DODO_BEGIN_NAMESPACE

VulkanDescriptorPool::VulkanDescriptorPool(uint32_t _descriptorCount, const Ref<VulkanDevice>& _vulkanDevice)
	: m_VulkanDevice(_vulkanDevice)
{
	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = _descriptorCount;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = _descriptorCount;

	const VkResult descriptionPoolCreateResult = vkCreateDescriptorPool(*m_VulkanDevice, &poolInfo, nullptr, &m_VkDescriptorPool);

	if(descriptionPoolCreateResult != VK_SUCCESS)
	{
		DODO_CRITICAL("Cannot create description pool");
	}

}

VulkanDescriptorPool::~VulkanDescriptorPool()
{
	vkDestroyDescriptorPool(*m_VulkanDevice, m_VkDescriptorPool, nullptr);
}

DODO_END_NAMESPACE
