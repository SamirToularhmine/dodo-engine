#include <DodoEngine/Platform/Vulkan/VulkanTextureImage.h>

#include <DodoEngine/Platform/Vulkan/VulkanBuffer.h>
#include <DodoEngine/Platform/Vulkan/VulkanCommandBuffer.h>
#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Platform/Vulkan/VulkanDevice.h>
#include <DodoEngine/Platform/Vulkan/VulkanImage.h>
#include <DodoEngine/Platform/Vulkan/VulkanPhysicalDevice.h>
#include <DodoEngine/Utils/Log.h>

DODO_BEGIN_NAMESPACE

Ptr<VulkanTextureImage> VulkanTextureImage::CreateFromImageData(const unsigned char* _imageData, uint32_t _imageSize, uint32_t _imageWidth,
                                                                uint32_t _imageHeight) {
  return std::make_unique<VulkanTextureImage>(_imageData, _imageSize, _imageWidth, _imageHeight);
}

Ptr<VulkanTextureImage> VulkanTextureImage::CreateEmptyImage(uint32_t _imageWidth, uint32_t _imageHeight) {
  return std::make_unique<VulkanTextureImage>(_imageWidth, _imageHeight);
}

VulkanTextureImage::VulkanTextureImage(const unsigned char* _imageData, uint32_t _imageSize, uint32_t _imageWidth, uint32_t _imageHeight) {
  const VulkanContext& vulkanContext = VulkanContext::Get();
  const Ref<VulkanDevice>& vulkanDevice = vulkanContext.GetVulkanDevice();
  const VulkanPhysicalDevice& vulkanPhysicalDevice = *vulkanContext.GetVulkanPhysicalDevice();
  const VmaAllocator& allocator = vulkanContext.GetAllocator();

  VulkanBuffer textureBuffer({_imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT});
  textureBuffer.SetMemory(_imageData, _imageSize);

  m_VulkanImage = std::make_unique<VulkanImage>(_imageWidth, _imageHeight, VK_IMAGE_TILING_OPTIMAL,
                                                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_FORMAT_B8G8R8A8_SRGB);
  m_VulkanImage->TransitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

  // From buffer to image
  m_VulkanImage->CopyFromBuffer(textureBuffer);

  m_VulkanImage->TransitionImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  Create();
}

VulkanTextureImage::VulkanTextureImage(uint32_t _imageWidth, uint32_t _imageHeight) {
  m_VulkanImage = std::make_unique<VulkanImage>(_imageWidth, _imageHeight, VK_IMAGE_TILING_OPTIMAL,
                                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                                VK_FORMAT_B8G8R8A8_SRGB);
  m_VulkanImage->TransitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  m_VulkanImage->TransitionImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  Create();
}

VulkanTextureImage::~VulkanTextureImage() {
  const VulkanContext& vulkanContext = VulkanContext::Get();
  const VulkanDevice& vulkanDevice = *vulkanContext.GetVulkanDevice();

  vkDeviceWaitIdle(vulkanDevice);

  vkDestroySampler(vulkanDevice, m_Sampler, nullptr);
  vkDestroyImageView(vulkanDevice, m_ImageView, nullptr);
}

void VulkanTextureImage::Create() {
  const VulkanContext& vulkanContext = VulkanContext::Get();
  const Ref<VulkanDevice>& vulkanDevice = vulkanContext.GetVulkanDevice();
  const VulkanPhysicalDevice& vulkanPhysicalDevice = *vulkanContext.GetVulkanPhysicalDevice();

  VkImageViewCreateInfo viewInfo{};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = *m_VulkanImage;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = VK_FORMAT_B8G8R8A8_SRGB;
  viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = 1;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  const VkResult createTextureImageViewResult = vkCreateImageView(*vulkanDevice, &viewInfo, nullptr, &m_ImageView);
  if (createTextureImageViewResult != VK_SUCCESS) {
    DODO_ERROR("Couldn't create image view for texture");
  }

  VkSamplerCreateInfo samplerInfo{};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter = VK_FILTER_LINEAR;
  samplerInfo.minFilter = VK_FILTER_LINEAR;
  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.anisotropyEnable = VK_TRUE;
  samplerInfo.maxAnisotropy = vulkanPhysicalDevice.GetProperties().limits.maxSamplerAnisotropy;
  samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;
  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  samplerInfo.mipLodBias = 0.0f;
  samplerInfo.minLod = 0.0f;
  samplerInfo.maxLod = 0.0f;

  const VkResult textureSamplerCreateResult = vkCreateSampler(*vulkanDevice, &samplerInfo, nullptr, &m_Sampler);
  if (textureSamplerCreateResult != VK_SUCCESS) {
    DODO_ERROR("Couldn't create sampler for texture");
  }
}

DODO_END_NAMESPACE
