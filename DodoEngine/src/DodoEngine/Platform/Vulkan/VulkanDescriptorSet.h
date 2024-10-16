#pragma once

#include <DodoEngine/Core/Types.h>

#define VK_NO_PROTOTYPES
#include <volk.h>
#include <vector>

DODO_BEGIN_NAMESPACE

class VulkanDevice;
class VulkanDescriptorSetLayout;
class VulkanDescriptorPool;
class VulkanBuffer;
struct RenderPass;
class Texture;

struct DescriptorUpdate {
  std::vector<VkDescriptorImageInfo> m_DescriptorImageInfo;
  std::vector<VkDescriptorBufferInfo> m_DescriptorBufferInfo;
  VkDescriptorType m_Type;
  uint32_t m_Count;
};

class VulkanDescriptorSet {
 public:
  VulkanDescriptorSet(const std::vector<VkDescriptorSetLayout>& _vulkanDescriptorSetLayouts, VulkanDescriptorPool& _vulkanDescriptionPool);
  ~VulkanDescriptorSet();

  void UpdateUniformDescriptor(const VulkanBuffer& _buffer);
  void UpdateImageSamplers(const std::vector<Ref<Texture>>& _textures);

  void Bind(const VkPipelineLayout& _vkPipelineLayout, const VkCommandBuffer& _commandBuffer);

  const VkDescriptorSet& GetCurrentDescriptorSet() const;

  void Reset();

 private:
  std::vector<VkDescriptorSet> m_VkDescriptorSets;
  std::vector<DescriptorUpdate> m_DecriptorUpdates;
};

DODO_END_NAMESPACE