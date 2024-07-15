#include <DodoEngine/Platform/Vulkan/VulkanDescriptorSet.h>

#include <DodoEngine/Platform/Vulkan/VulkanBuffer.h>
#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Platform/Vulkan/VulkanDescriptorPool.h>
#include <DodoEngine/Platform/Vulkan/VulkanDevice.h>
#include <DodoEngine/Platform/Vulkan/VulkanTextureImage.h>
#include <DodoEngine/Renderer/Texture.h>
#include <DodoEngine/Renderer/UniformBufferObject.h>
#include <DodoEngine/Utils/Log.h>

#include <array>
#include <vector>

DODO_BEGIN_NAMESPACE

VulkanDescriptorSet::VulkanDescriptorSet(VulkanDescriptorPool& _vulkanDescriptionPool, const Ref<VulkanDevice>& _vulkanDevice,
                                         VkDescriptorSetLayout _vulkanDescriptorSetLayouts[])
    : m_VulkanDevice(_vulkanDevice) {
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = _vulkanDescriptionPool;
  allocInfo.descriptorSetCount = VulkanContext::MAX_FRAMES_IN_FLIGHT;
  allocInfo.pSetLayouts = _vulkanDescriptorSetLayouts;

  m_VkDescriptorSets.resize(VulkanContext::MAX_FRAMES_IN_FLIGHT);

  VkResult allocateDescriptorSetCreateResult = vkAllocateDescriptorSets(*m_VulkanDevice, &allocInfo, m_VkDescriptorSets.data());
  if (allocateDescriptorSetCreateResult != VK_SUCCESS) {
    DODO_CRITICAL("Could not create descriptor set");
  }
}

void VulkanDescriptorSet::UpdateDescriptor(const VulkanBuffer& _buffer, const std::vector<Ref<Texture>>& _textures, const uint32_t& _frameIndex) {
  DODO_TRACE(VulkanDescriptorSet);

  VkDescriptorBufferInfo bufferInfo{};
  bufferInfo.buffer = _buffer;
  bufferInfo.offset = 0;
  bufferInfo.range = _buffer.Size();

  VkWriteDescriptorSet bufferDescriptorWrite{};
  bufferDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  bufferDescriptorWrite.dstSet = m_VkDescriptorSets[_frameIndex];
  bufferDescriptorWrite.dstBinding = 0;
  bufferDescriptorWrite.dstArrayElement = 0;
  bufferDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  bufferDescriptorWrite.descriptorCount = 1;
  bufferDescriptorWrite.pBufferInfo = &bufferInfo;

  VkDescriptorImageInfo imagesInfo[128];
  for (uint32_t i = 0; i < _textures.size(); ++i) {
    const VulkanTextureImage& textureImage = *_textures[i]->GetTextureImage();
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = textureImage.GetImageView();
    imageInfo.sampler = textureImage.GetSampler();

    imagesInfo[i] = imageInfo;
  }

  VkWriteDescriptorSet imageDescriptorWrite{};
  imageDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  imageDescriptorWrite.dstSet = m_VkDescriptorSets[_frameIndex];
  imageDescriptorWrite.dstBinding = 1;
  imageDescriptorWrite.dstArrayElement = 0;
  imageDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  imageDescriptorWrite.descriptorCount = _textures.size();
  imageDescriptorWrite.pImageInfo = imagesInfo;

  std::array<VkWriteDescriptorSet, 2> descriptorWrites = {bufferDescriptorWrite, imageDescriptorWrite};
  vkUpdateDescriptorSets(*m_VulkanDevice, 2, descriptorWrites.data(), 0, nullptr);
}

void VulkanDescriptorSet::Bind(const VulkanRenderPassData& _vulkanRenderPassData) const {
  DODO_TRACE(VulkanDescriptorSet);
  vkCmdBindDescriptorSets(_vulkanRenderPassData.m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _vulkanRenderPassData.m_PipelineLayout, 0, 1,
                          &m_VkDescriptorSets[_vulkanRenderPassData.m_FrameIndex], 0, nullptr);
}

VulkanDescriptorSet::~VulkanDescriptorSet() {}

DODO_END_NAMESPACE
