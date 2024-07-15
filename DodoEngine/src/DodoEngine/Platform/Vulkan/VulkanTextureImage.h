#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Platform/Vulkan/VulkanImage.h>

#include <vk_mem_alloc.h>
#include <volk.h>

DODO_BEGIN_NAMESPACE

class VulkanDevice;

class VulkanTextureImage {
 public:
  VulkanTextureImage(const unsigned char* _imageData, uint32_t _imageSize, uint32_t _imageWidth, uint32_t _imageHeight);

  ~VulkanTextureImage();

  const VkImageView& GetImageView() const { return m_ImageView; }
  const VkSampler& GetSampler() const { return m_Sampler; }

  operator const VkImage&() const { return *m_VulkanImage; }

 private:
  Ptr<VulkanImage> m_VulkanImage;
  VkImageView m_ImageView;
  VkSampler m_Sampler;

  Ref<VulkanDevice> m_VulkanDevice;
};

DODO_END_NAMESPACE