#pragma once

#include <DodoEngine/Core/Types.h>

#include <volk.h>


DODO_BEGIN_NAMESPACE

class VulkanDevice;
class VulkanRenderPass;
struct VulkanSwapChainData;

class VulkanFrameBuffer
{
public:
	VulkanFrameBuffer(Ref<VulkanDevice> _vulkanDevice, const VulkanRenderPass& _vulkanRenderPass, const VkImageView _attachments[], const VulkanSwapChainData& _swapChainData);

	~VulkanFrameBuffer();

	operator const VkFramebuffer& () const { return m_FrameBuffer; }

private:
	VkFramebuffer m_FrameBuffer;
	Ref<VulkanDevice> m_VulkanDevice;
};

DODO_END_NAMESPACE