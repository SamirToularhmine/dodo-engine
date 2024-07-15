#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Platform/Vulkan/VulkanImage.h>

#include <volk.h>
#include <vk_mem_alloc.h>

DODO_BEGIN_NAMESPACE

class VulkanDevice;

class VulkanDepthImage {
 public:
  VulkanDepthImage(uint32_t _imageWidth, uint32_t _imageHeight);

  ~VulkanDepthImage();

  const VkImageView& GetImageView() const { return m_ImageView; }

  VkFormat GetFormat() const { return m_VulkanImage->GetFormat(); }

  operator const VkImage&() const { return *m_VulkanImage; }

 private:
  Ptr<VulkanImage> m_VulkanImage;
  VkImageView m_ImageView;

  Ref<VulkanDevice> m_VulkanDevice;
};

DODO_END_NAMESPACE