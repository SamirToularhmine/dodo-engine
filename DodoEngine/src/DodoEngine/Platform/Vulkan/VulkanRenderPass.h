#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Platform/Vulkan/VulkanSwapChain.h>

#include <volk.h>


DODO_BEGIN_NAMESPACE

class VulkanRenderPass
{
public:
	VulkanRenderPass(const Ref<VulkanDevice>& _vulkanDevice, const VulkanSwapChainData& _swapChainData);
    ~VulkanRenderPass();

    operator const VkRenderPass& () const { return m_VkRenderPass; }
private:
    VkRenderPass m_VkRenderPass;
    Ref<VulkanDevice> m_VulkanDevice;
};

DODO_END_NAMESPACE
