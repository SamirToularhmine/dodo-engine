#include <DodoEngine/Platform/Vulkan/VulkanDescriptorPool.h>

#include <DodoEngine/Platform/Vulkan/VulkanDevice.h>
#include <DodoEngine/Utils/Log.h>

#include <array>

DODO_BEGIN_NAMESPACE

VulkanDescriptorPool::VulkanDescriptorPool(uint32_t _descriptorCount, const Ref<VulkanDevice> _vulkanDevice) : m_VulkanDevice(_vulkanDevice) {
  VkDescriptorPoolSize poolSizes[] = {{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
                                      {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                                      {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
                                      {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
                                      {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
                                      {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
                                      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
                                      {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
                                      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                                      {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                                      {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};
  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  poolInfo.poolSizeCount = 11;
  poolInfo.pPoolSizes = poolSizes;
  poolInfo.maxSets = 1000 * 11;

  const VkResult descriptionPoolCreateResult = vkCreateDescriptorPool(*m_VulkanDevice, &poolInfo, nullptr, &m_VkDescriptorPool);

  if (descriptionPoolCreateResult != VK_SUCCESS) {
    DODO_CRITICAL("Cannot create description pool");
  }
}

VulkanDescriptorPool::~VulkanDescriptorPool() {
  vkDestroyDescriptorPool(*m_VulkanDevice, m_VkDescriptorPool, nullptr);
}

DODO_END_NAMESPACE
