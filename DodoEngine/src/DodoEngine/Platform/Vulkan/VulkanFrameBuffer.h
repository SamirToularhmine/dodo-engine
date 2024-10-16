#pragma once

#include <DodoEngine/Core/Types.h>

#include <volk.h>

#include <vector>

DODO_BEGIN_NAMESPACE

class VulkanDevice;
class VulkanRenderPass;
struct VulkanSwapChainData;

class VulkanFrameBuffer {
 public:
  VulkanFrameBuffer(Ref<VulkanDevice> _vulkanDevice, const VulkanRenderPass& _vulkanRenderPass, const std::vector<VkImageView>& _attachments,
                    const uint32_t& _width, const uint32_t& _height);

  ~VulkanFrameBuffer();

  operator const VkFramebuffer&() const { return m_FrameBuffer; }

 private:
  VkFramebuffer m_FrameBuffer;
  Ref<VulkanDevice> m_VulkanDevice;
};

DODO_END_NAMESPACE