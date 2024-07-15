#pragma once

#include <DodoEngine/Core/Types.h>

#include <volk.h>
#include <vk_mem_alloc.h>

DODO_BEGIN_NAMESPACE

class VulkanDevice;
class VulkanBuffer;

class VulkanImage {
 public:
  VulkanImage(uint32_t _imageWidth, uint32_t _imageHeight, VkImageTiling _tiling, uint32_t _usage, uint32_t _featureFlags);

  VulkanImage(uint32_t _imageWidth, uint32_t _imageHeight, VkImageTiling _tiling, uint32_t _usage, VkFormat _format);

  void CreateImage(uint32_t _imageWidth, uint32_t _imageHeight, VkImageTiling _tiling, uint32_t _usage, VkFormat _format);

  ~VulkanImage();

  VkFormat GetFormat() const { return m_Format; }

  void TransitionImageLayout(VkImageLayout _newLayout);

  void CopyFromBuffer(const VulkanBuffer& _imageBuffer);

  operator const VkImage&() const { return m_Image; }

 protected:
  VkImage m_Image;
  VmaAllocation m_Allocation;
  VkImageLayout m_Layout;
  uint32_t m_Width;
  uint32_t m_Height;
  VkImageTiling m_Tiling;
  uint32_t m_Usage;
  VkFormat m_Format;
};

DODO_END_NAMESPACE