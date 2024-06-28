#pragma once

#include <DodoEngine/Core/Types.h>

#include <volk.h>
#include <vector>;


DODO_BEGIN_NAMESPACE

class VulkanDevice;
class VulkanDescriptorSetLayout;
class VulkanDescriptorPool;
class VulkanBuffer;
struct VulkanRenderPassData;

class VulkanDescriptorSet
{
public:
	VulkanDescriptorSet(VulkanDescriptorPool& _vulkanDescriptionPool, Ref<VulkanDevice>& _vulkanDevice, VkDescriptorSetLayout _vulkanDescriptorSetLayouts[]);

	~VulkanDescriptorSet();

	void SetMemory(const std::vector<VulkanBuffer>& _buffers, const uint32_t& _frameIndex);

	void Bind(const VulkanRenderPassData& _vulkanRenderPassData) const;


private:
	std::vector<VkDescriptorSet> m_VkDescriptorSets;
	Ref<VulkanDevice> m_VulkanDevice;
};

DODO_END_NAMESPACE