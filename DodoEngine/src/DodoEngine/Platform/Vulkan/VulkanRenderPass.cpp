#include <DodoEngine/Platform/Vulkan/VulkanRenderPass.h>

#include <DodoEngine/Core/Camera.h>
#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Platform/Vulkan/VulkanDevice.h>
#include <DodoEngine/Platform/Vulkan/VulkanSwapChain.h>
#include <DodoEngine/Utils/Log.h>

#include <array>


DODO_BEGIN_NAMESPACE

VulkanRenderPass::VulkanRenderPass(const Ref<VulkanDevice>& _vulkanDevice, const VulkanRenderPassAttachments& _renderPassAttachments)
	: m_VulkanDevice(_vulkanDevice)
{
    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency subpassDependency{};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpassDependency.srcAccessMask = 0;
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = _renderPassAttachments.m_AttachmentsDescription.size();
    renderPassCreateInfo.pAttachments = _renderPassAttachments.m_AttachmentsDescription.data();
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
    renderPassBeginInfo.renderArea.extent = _vulkanRenderPassData.m_FrameSize;

    std::array<VkClearValue, 2> clearValues = {
        // {{{0.694f, 0.686f, 1.0f, 1.0f}}, 
        {{{0.0f, 0.0f, 0.0f, 1.0f}}, 
        {1.0f, 0.0f}}
    };
    renderPassBeginInfo.clearValueCount = clearValues.size();
    renderPassBeginInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(_vulkanRenderPassData.m_CommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanRenderPass::End(const VulkanRenderPassData& _vulkanRenderPassData) const
{
    vkCmdEndRenderPass(_vulkanRenderPassData.m_CommandBuffer);
}

DODO_END_NAMESPACE
