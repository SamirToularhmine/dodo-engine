#include <DodoEngine/Platform/Vulkan/VulkanBuffer.h>
#include <DodoEngine/Platform/Vulkan/VulkanCommandBuffer.h>
#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Platform/Vulkan/VulkanDescriptorPool.h>
#include <DodoEngine/Platform/Vulkan/VulkanDescriptorSet.h>
#include <DodoEngine/Platform/Vulkan/VulkanDevice.h>
#include <DodoEngine/Platform/Vulkan/VulkanTextureImage.h>
#include <DodoEngine/Renderer/Texture.h>
#include <DodoEngine/Utils/Log.h>

#include <array>
#include <vector>

DODO_BEGIN_NAMESPACE

VulkanDescriptorSet::VulkanDescriptorSet(
    const std::vector<VkDescriptorSetLayout> &_vulkanDescriptorSetLayouts,
    VulkanDescriptorPool &_vulkanDescriptionPool)
{
  const VkDevice &vkDevice = *VulkanContext::Get().GetVulkanDevice();

  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = _vulkanDescriptionPool;
  allocInfo.descriptorSetCount = 1;
  allocInfo.pSetLayouts = _vulkanDescriptorSetLayouts.data();

  m_VkDescriptorSets.resize(1);

  VkResult allocateDescriptorSetCreateResult =
      vkAllocateDescriptorSets(vkDevice, &allocInfo, m_VkDescriptorSets.data());
  if (allocateDescriptorSetCreateResult != VK_SUCCESS)
  {
    DODO_CRITICAL("Could not create descriptor set");
  }
}

void VulkanDescriptorSet::UpdateUniformDescriptor(const VulkanBuffer &_buffer)
{
  const VkDevice &vkDevice = *VulkanContext::Get().GetVulkanDevice();

  VkDescriptorBufferInfo bufferInfo{};
  bufferInfo.buffer = _buffer;
  bufferInfo.offset = 0;
  bufferInfo.range = _buffer.Size();

  DescriptorUpdate &descriptorUpdate = m_DecriptorUpdates.emplace_back();
  descriptorUpdate.m_Count = 1;
  descriptorUpdate.m_Type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptorUpdate.m_DescriptorBufferInfo.push_back(bufferInfo);
}

void VulkanDescriptorSet::UpdateImageSamplers(const std::vector<Ref<Texture>> _textures)
{
  // DODO_TRACE(VulkanDescriptorSet);

  const VkDevice &vkDevice = *VulkanContext::Get().GetVulkanDevice();
  constexpr uint32_t maxImageCount = 16;
  const uint32_t textureSize = _textures.size();

  DescriptorUpdate &descriptorUpdate = m_DecriptorUpdates.emplace_back();
  descriptorUpdate.m_Count = textureSize;
  descriptorUpdate.m_Type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

  for (uint32_t i = 0; i < textureSize; ++i)
  {
    const VulkanTextureImage &textureImage = *_textures[i]->GetTextureImage();
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = textureImage.GetImageView();
    imageInfo.sampler = textureImage.GetSampler();

    descriptorUpdate.m_DescriptorImageInfo.push_back(imageInfo);
  }
}

void VulkanDescriptorSet::Bind(const VkPipelineLayout &_vkPipelineLayout,
                               const VkCommandBuffer &_commandBuffer)
{
  // DODO_TRACE(VulkanDescriptorSet);

  const VkDevice &vkDevice = *VulkanContext::Get().GetVulkanDevice();
  std::vector<VkWriteDescriptorSet> writeDescriptorSet;

  for (uint32_t i = 0; i < m_DecriptorUpdates.size(); ++i)
  {
    const DescriptorUpdate &descriptorUpdate = m_DecriptorUpdates[i];

    VkWriteDescriptorSet bufferDescriptorWrite{};
    bufferDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    bufferDescriptorWrite.dstSet = GetCurrentDescriptorSet();
    bufferDescriptorWrite.dstBinding = i;
    bufferDescriptorWrite.dstArrayElement = 0;
    bufferDescriptorWrite.descriptorCount = descriptorUpdate.m_Count;
    bufferDescriptorWrite.descriptorType = descriptorUpdate.m_Type;

    if (descriptorUpdate.m_Type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
    {
      bufferDescriptorWrite.pImageInfo = descriptorUpdate.m_DescriptorImageInfo.data();
    }
    else
    {
      bufferDescriptorWrite.pBufferInfo = descriptorUpdate.m_DescriptorBufferInfo.data();
    }

    writeDescriptorSet.push_back(bufferDescriptorWrite);
  }

  vkUpdateDescriptorSets(vkDevice, writeDescriptorSet.size(), writeDescriptorSet.data(), 0,
                         nullptr);

  vkCmdBindDescriptorSets(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _vkPipelineLayout, 0, 1,
                          &GetCurrentDescriptorSet(), 0, nullptr);
}

VkDescriptorSet const &VulkanDescriptorSet::GetCurrentDescriptorSet() const
{
  return m_VkDescriptorSets[0];
}

void VulkanDescriptorSet::Reset() { m_DecriptorUpdates.clear(); }

VulkanDescriptorSet::~VulkanDescriptorSet() {}

DODO_END_NAMESPACE
