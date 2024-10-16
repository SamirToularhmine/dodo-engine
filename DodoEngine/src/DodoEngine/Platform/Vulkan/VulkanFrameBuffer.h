#pragma once

#include <DodoEngine/Core/Types.h>

#define VK_NO_PROTOTYPES
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

  const VkExtent2D GetDimensions() const { return {m_Dimensions.width, m_Dimensions.height}; }

  operator const VkFramebuffer&() const { return m_FrameBuffer; }

 private:
  VkFramebuffer m_FrameBuffer;
  VkExtent2D m_Dimensions;
  Ref<VulkanDevice> m_VulkanDevice;
};

DODO_END_NAMESPACE