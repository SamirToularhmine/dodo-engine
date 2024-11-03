#pragma once

#include <DodoEngine/Core/Types.h>

#include <vector>
#include <volk.h>

DODO_BEGIN_NAMESPACE

class VulkanDevice;
class VulkanDescriptorSetLayout;
struct VulkanSwapChainData;
class VulkanRenderPass;

struct VulkanGraphicPipelineSpecification {
  Ref<VulkanDescriptorSetLayout> m_VulkanDescriptorSetLayout;
  VkShaderModule m_VertexShaderModule;
  VkShaderModule m_FragmentShaderModule;
};

class VulkanGraphicPipeline {
 public:
  VulkanGraphicPipeline(const VulkanGraphicPipelineSpecification& _vulkanPipelineSpec, const VulkanSwapChainData& _swapChainData);
  ~VulkanGraphicPipeline();

  const VkViewport& GetViewPort() const { return m_Viewport; }
  const VkRect2D& GetScissor() const { return m_Scissor; }
  VkPipelineLayout GetPipelineLayout() const { return m_PipelineLayout; }
  const std::vector<VkDescriptorSetLayout>& GetDescriptorSetLayout() const { return m_DescriptorSetLayouts; }

  void Bind(const VkCommandBuffer& _vkCommandBuffer) const;

  operator const VkPipeline&() const { return m_Pipeline; }
  void SetViewPort(const VulkanSwapChainData& swapChainSpec);

 private:
  VkPipeline m_Pipeline;
  VkPipelineLayout m_PipelineLayout;

  VkViewport m_Viewport;
  VkRect2D m_Scissor;
  
  VkShaderModule m_VertexShaderModule;
  VkShaderModule m_FragmentShaderModule;
  
  Ref<VulkanDescriptorSetLayout> m_DescriptorSetLayout;
  std::vector<VkDescriptorSetLayout> m_DescriptorSetLayouts;
};

DODO_END_NAMESPACE
