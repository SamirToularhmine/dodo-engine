#pragma once

#include <DodoEngine/Core/Types.h>

#define VK_NO_PROTOTYPES
#include <volk.h>

#include <vector>

DODO_BEGIN_NAMESPACE

struct RenderPass;
class VulkanDevice;
struct VulkanSwapChainData;
struct VulkanRenderPassData;

struct VulkanRenderPassAttachments {
  std::vector<VkAttachmentDescription> m_AttachmentsDescription;
  std::vector<VkAttachmentReference> m_AttachmentsReference;
};

class VulkanRenderPass {

 public:
  VulkanRenderPass(const Ref<VulkanDevice>& _vulkanDevice, const VulkanRenderPassAttachments& _renderPassAttachments);
  ~VulkanRenderPass();

  void Begin(const RenderPass& _renderPass) const;
  void End(const RenderPass& _renderPass) const;

  operator const VkRenderPass&() const { return m_VkRenderPass; }

 private:
  VkRenderPass m_VkRenderPass;
  Ref<VulkanDevice> m_VulkanDevice;
};

DODO_END_NAMESPACE
