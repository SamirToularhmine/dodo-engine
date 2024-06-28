#include <DodoEngine/Platform/Vulkan/VulkanFrameBuffer.h>

#include <DodoEngine/Platform/Vulkan/VulkanDevice.h>
#include <DodoEngine/Platform/Vulkan/VulkanRenderPass.h>
#include <DodoEngine/Platform/Vulkan/VulkanSwapChain.h>

#include <DodoEngine/Utils/Log.h>

DODO_BEGIN_NAMESPACE

VulkanFrameBuffer::VulkanFrameBuffer(Ref<VulkanDevice> _vulkanDevice, const VulkanRenderPass& _vulkanRenderPass, const VkImageView _attachments[], const VulkanSwapChainData& _swapChainData)
	: m_VulkanDevice(_vulkanDevice)
{
    VkFramebufferCreateInfo framebufferCreateInfo{};
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.renderPass = _vulkanRenderPass;
    framebufferCreateInfo.attachmentCount = 1;
    framebufferCreateInfo.pAttachments = _attachments;
    framebufferCreateInfo.width = _swapChainData.m_VkExtent.width;
    framebufferCreateInfo.height = _swapChainData.m_VkExtent.height;
    framebufferCreateInfo.layers = 1;
    
    const VkResult createFramebufferResult = vkCreateFramebuffer(*_vulkanDevice, &framebufferCreateInfo, nullptr, &m_FrameBuffer);
    if (createFramebufferResult != VK_SUCCESS) {
        DODO_CRITICAL("Could not create frame buffer");
    }
}

VulkanFrameBuffer::~VulkanFrameBuffer()
{
    //vkDestroyFramebuffer(*m_VulkanDevice, m_FrameBuffer, nullptr);
}

DODO_END_NAMESPACE