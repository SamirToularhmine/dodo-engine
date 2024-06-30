#include <DodoEngine/Platform/Vulkan/VulkanRenderPass.h>

#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Platform/Vulkan/VulkanDevice.h>
#include <DodoEngine/Platform/Vulkan/VulkanSwapChain.h>
#include <DodoEngine/Utils/Log.h>

DODO_BEGIN_NAMESPACE
VulkanRenderPass::VulkanRenderPass(const Ref<VulkanDevice>& _vulkanDevice, const VulkanSwapChainData& _swapChainData)
	: m_VulkanDevice(_vulkanDevice)
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = _swapChainData.m_VkSurfaceFormat.format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency subpassDependency{};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.srcAccessMask = 0;
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = 1;
    renderPassCreateInfo.pAttachments = &colorAttachment;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpass;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &subpassDependency;

    VkResult createRenderPassResult = vkCreateRenderPass(*m_VulkanDevice, &renderPassCreateInfo, nullptr, &m_VkRenderPass);
    if (createRenderPassResult != VK_SUCCESS) {
        DODO_CRITICAL("Could not create the render pass");
    }
}

VulkanRenderPass::~VulkanRenderPass()
{
    vkDestroyRenderPass(*m_VulkanDevice, m_VkRenderPass, nullptr);
}

void VulkanRenderPass::Begin(const VulkanRenderPassData& _vulkanRenderPassData) const
{
    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = m_VkRenderPass;
    renderPassBeginInfo.framebuffer = _vulkanRenderPassData.m_FrameBuffer;
    renderPassBeginInfo.renderArea.offset = { 0, 0 };
    renderPassBeginInfo.renderArea.extent = _vulkanRenderPassData.m_SwapChainData.m_VkExtent;

    VkClearValue clearColor = { {{	0.694f, 0.686f, 1.0f, 1.0f}} };
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(_vulkanRenderPassData.m_CommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanRenderPass::End(const VulkanRenderPassData& _vulkanRenderPassData) const
{
    vkCmdEndRenderPass(_vulkanRenderPassData.m_CommandBuffer);
}

DODO_END_NAMESPACE
