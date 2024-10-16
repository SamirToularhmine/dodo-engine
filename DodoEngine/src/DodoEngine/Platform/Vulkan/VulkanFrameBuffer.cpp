#include <DodoEngine/Platform/Vulkan/VulkanFrameBuffer.h>

#include <DodoEngine/Platform/Vulkan/VulkanDevice.h>
#include <DodoEngine/Platform/Vulkan/VulkanRenderPass.h>
#include <DodoEngine/Platform/Vulkan/VulkanSwapChain.h>

#include <DodoEngine/Utils/Log.h>

DODO_BEGIN_NAMESPACE

VulkanFrameBuffer::VulkanFrameBuffer(Ref<VulkanDevice> _vulkanDevice, const VulkanRenderPass& _vulkanRenderPass, const std::vector<VkImageView>& _attachments,
                                     const uint32_t& _width, const uint32_t& _height)
    : m_Dimensions({_width, _height}), m_VulkanDevice(_vulkanDevice) {

  VkFramebufferCreateInfo framebufferCreateInfo{};
  framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  framebufferCreateInfo.renderPass = _vulkanRenderPass;
  framebufferCreateInfo.attachmentCount = _attachments.size();
  framebufferCreateInfo.pAttachments = _attachments.data();
  framebufferCreateInfo.width = _width;
  framebufferCreateInfo.height = _height;
  framebufferCreateInfo.layers = 1;

  const VkResult createFramebufferResult = vkCreateFramebuffer(*_vulkanDevice, &framebufferCreateInfo, nullptr, &m_FrameBuffer);
  if (createFramebufferResult != VK_SUCCESS) {
    DODO_CRITICAL("Could not create frame buffer");
  }
}

VulkanFrameBuffer::~VulkanFrameBuffer() {
  //vkDestroyFramebuffer(*m_VulkanDevice, m_FrameBuffer, nullptr);
}

DODO_END_NAMESPACE
