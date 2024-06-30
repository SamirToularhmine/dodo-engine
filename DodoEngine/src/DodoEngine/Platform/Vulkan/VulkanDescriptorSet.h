#pragma once

#include <DodoEngine/Core/Types.h>

#include <volk.h>
#include <vector>


DODO_BEGIN_NAMESPACE

class VulkanDevice;
class VulkanDescriptorSetLayout;
class VulkanDescriptorPool;
class VulkanBuffer;
struct VulkanRenderPassData;

class VulkanDescriptorSet
{
public:
	VulkanDescriptorSet(VulkanDescriptorPool& _vulkanDescriptionPool, const Ref<VulkanDevice>& _vulkanDevice, VkDescriptorSetLayout _vulkanDescriptorSetLayouts[]);

	~VulkanDescriptorSet();

	void UpdateDescriptor(const VulkanBuffer& _buffer, const uint32_t& _frameIndex) const;

	void Bind(const VulkanRenderPassData& _vulkanRenderPassData) const;

private:
	std::vector<VkDescriptorSet> m_VkDescriptorSets;
	Ref<VulkanDevice> m_VulkanDevice;
};

DODO_END_NAMESPACE