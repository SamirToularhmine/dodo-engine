#pragma once

#include <DodoEngine/Core/Types.h>

#include <volk.h>

#include <vector>

DODO_BEGIN_NAMESPACE

struct VulkanDescriptorSetLayoutSpec {
  VkDescriptorType m_vkDescriptorType;
  uint32_t m_DescriptorCount;
  VkShaderStageFlagBits m_ShaderStage;
};

class VulkanDevice;

class VulkanDescriptorSetLayout {
 public:
  VulkanDescriptorSetLayout(const std::vector<VulkanDescriptorSetLayoutSpec>& _vulkanDescriptorLayoutSpecs);

  ~VulkanDescriptorSetLayout();

  operator VkDescriptorSetLayout&() { return m_VkDescriptorSetLayout; }

 private:
  VkDescriptorSetLayout m_VkDescriptorSetLayout;
};

DODO_END_NAMESPACE
