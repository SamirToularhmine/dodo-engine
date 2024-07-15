#pragma once

#include <DodoEngine/Core/Types.h>

#include <volk.h>

DODO_BEGIN_NAMESPACE

class VulkanDevice;
struct VulkanSwapChainData;
struct VulkanRenderPassData;

class VulkanRenderPass {
 public:
  VulkanRenderPass(const Ref<VulkanDevice>& _vulkanDevice, const VulkanSwapChainData& _swapChainData);
  ~VulkanRenderPass();

  void Begin(const VulkanRenderPassData& _vulkanRenderPassData) const;
  void End(const VulkanRenderPassData& _vulkanRenderPassData) const;

  operator const VkRenderPass&() const { return m_VkRenderPass; }

 private:
  VkRenderPass m_VkRenderPass;
  Ref<VulkanDevice> m_VulkanDevice;
};

DODO_END_NAMESPACE
