#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Platform/Vulkan/VulkanDepthImage.h>
#include <DodoEngine/Platform/Vulkan/VulkanDevice.h>
#include <DodoEngine/Platform/Vulkan/VulkanPhysicalDevice.h>

DODO_BEGIN_NAMESPACE

VulkanDepthImage::VulkanDepthImage(uint32_t _imageWidth, uint32_t _imageHeight)
{
  const VulkanContext &vulkanContext = VulkanContext::Get();
  const Ref<VulkanDevice> vulkanDevice = vulkanContext.GetVulkanDevice();
  const Ref<VulkanPhysicalDevice> vulkanPhysicalDevice = vulkanContext.GetVulkanPhysicalDevice();

  m_VulkanImage = MakePtr<VulkanImage>(_imageWidth, _imageHeight, VK_IMAGE_TILING_OPTIMAL,
                                       VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                       VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

  VkImageViewCreateInfo viewInfo{};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = *m_VulkanImage;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = m_VulkanImage->GetFormat();
  viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = 1;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  const VkResult createTextureImageViewResult =
      vkCreateImageView(*vulkanDevice, &viewInfo, nullptr, &m_ImageView);
  if (createTextureImageViewResult != VK_SUCCESS)
  {
    DODO_ERROR("Couldn't create image view for texture");
  }
}

VulkanDepthImage::~VulkanDepthImage()
{
  const VulkanContext &vulkanContext = VulkanContext::Get();
  const Ref<VulkanDevice> vulkanDevice = vulkanContext.GetVulkanDevice();

  vkDestroyImageView(*vulkanDevice, m_ImageView, nullptr);
}

DODO_END_NAMESPACE
