#include <DodoEngine/Platform/Vulkan/VulkanDescriptorSet.h>

#include <DodoEngine/Platform/Vulkan/VulkanBuffer.h>
#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Platform/Vulkan/VulkanDescriptorPool.h>
#include <DodoEngine/Platform/Vulkan/VulkanDevice.h>
#include <DodoEngine/Renderer/UniformBufferObject.h>
#include <DodoEngine/Utils/Log.h>

DODO_BEGIN_NAMESPACE
	VulkanDescriptorSet::VulkanDescriptorSet(VulkanDescriptorPool& _vulkanDescriptionPool, const Ref<VulkanDevice>& _vulkanDevice, VkDescriptorSetLayout _vulkanDescriptorSetLayouts[]
) : m_VulkanDevice(_vulkanDevice)
{
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = _vulkanDescriptionPool;
	allocInfo.descriptorSetCount = VulkanContext::MAX_FRAMES_IN_FLIGHT;
	allocInfo.pSetLayouts = _vulkanDescriptorSetLayouts;

	m_VkDescriptorSets.resize(VulkanContext::MAX_FRAMES_IN_FLIGHT);

	VkResult allocateDescriptorSetCreateResult = vkAllocateDescriptorSets(*m_VulkanDevice, &allocInfo, m_VkDescriptorSets.data());
	if(allocateDescriptorSetCreateResult != VK_SUCCESS)
	{
		DODO_CRITICAL("Could not create descriptor set");
	}
}

void VulkanDescriptorSet::UpdateDescriptor(const VulkanBuffer& _buffer, const uint32_t& _frameIndex) const
{
	for(uint32_t i = 0; i < VulkanContext::MAX_FRAMES_IN_FLIGHT; ++i)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = _buffer;
		bufferInfo.offset = 0;
		bufferInfo.range = _buffer.Size();

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = m_VkDescriptorSets[_frameIndex];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(*m_VulkanDevice, 1, &descriptorWrite, 0, nullptr);
	}
}

void VulkanDescriptorSet::Bind(const VulkanRenderPassData& _vulkanRenderPassData) const
{
	vkCmdBindDescriptorSets(_vulkanRenderPassData.m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _vulkanRenderPassData.m_PipelineLayout, 0, 1, &m_VkDescriptorSets[_vulkanRenderPassData.m_FrameIndex], 0, nullptr);
}

VulkanDescriptorSet::~VulkanDescriptorSet()
{
}

DODO_END_NAMESPACE
