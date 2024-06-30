#pragma once

#include "VulkanContext.h"

#include <DodoEngine/Core/Types.h>

#include <volk.h>


DODO_BEGIN_NAMESPACE

class VulkanDevice;
class VulkanDescriptorSetLayout;
struct VulkanSwapChainData;
class VulkanRenderPass;

class VulkanGraphicPipeline
{
public:
    VulkanGraphicPipeline(Ref<VulkanDevice> _vulkanDevice,
                          VkDescriptorSetLayout& _vkDescriptorSetLayout,
                          const VulkanSwapChainData& _swapChainData,
                          const VulkanRenderPass& _vulkanRenderPass);
    ~VulkanGraphicPipeline();

    const VkViewport& GetViewPort() const { return m_Viewport; }
    const VkRect2D& GetScissor() const { return m_Scissor; }
    VkPipelineLayout GetPipelineLayout() const { return m_VkPipelineLayout; }

	void Bind(const VkCommandBuffer& _vkCommandBuffer) const;

    operator const VkPipeline& () const { return m_VkPipeline; }
    void SetViewPort(const VulkanSwapChainData& swapChainSpec);
private:
    VkPipeline m_VkPipeline;
    VkPipelineLayout m_VkPipelineLayout;
    VkViewport m_Viewport;
    VkRect2D m_Scissor;
    Ref<VulkanDevice> m_VulkanDevice;
    VkShaderModule m_VertexShaderModule;
    VkShaderModule m_FragmentShaderModule;
};

DODO_END_NAMESPACE
