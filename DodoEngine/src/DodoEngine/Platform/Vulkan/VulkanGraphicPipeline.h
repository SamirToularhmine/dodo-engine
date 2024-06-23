#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Platform/Vulkan/VulkanDevice.h>
#include <DodoEngine/Platform/Vulkan/VulkanRenderPass.h>
#include <DodoEngine/Platform/Vulkan/VulkanSwapChain.h>

#include <volk.h>


DODO_BEGIN_NAMESPACE

class VulkanGraphicPipeline
{
public:
    VulkanGraphicPipeline(Ref<VulkanDevice> _vulkanDevice, const VulkanSwapChainData& _swapChainData, const VulkanRenderPass& _vulkanRenderPass);
    ~VulkanGraphicPipeline();

    const VkViewport& GetViewPort() const { return m_Viewport; }
    const VkRect2D& GetScissor() const { return m_Scissor; }

    operator const VkPipeline& () const { return m_VkPipeline; }
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
