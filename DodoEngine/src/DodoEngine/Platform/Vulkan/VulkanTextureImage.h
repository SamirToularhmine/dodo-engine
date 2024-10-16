#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Platform/Vulkan/VulkanImage.h>

#define VK_NO_PROTOTYPES
#include <volk.h>
#include <vk_mem_alloc.h>

DODO_BEGIN_NAMESPACE

class VulkanDevice;

class VulkanTextureImage {
public:
    static Ptr<VulkanTextureImage> CreateEmptyImage(uint32_t _imageWidth, uint32_t _imageHeight);
    static Ptr<VulkanTextureImage> CreateFromImageData(const unsigned char* _imageData, uint32_t _imageSize, uint32_t _imageWidth, uint32_t _imageHeight);

    VulkanTextureImage(const unsigned char* _imageData, uint32_t _imageSize, uint32_t _imageWidth, uint32_t _imageHeight);
    VulkanTextureImage(uint32_t _imageWidth, uint32_t _imageHeight);
    ~VulkanTextureImage();

    const VkImageView& GetImageView() const { return m_ImageView; }
    const VulkanImage& GetImage() const { return *m_VulkanImage; }
    const VkSampler& GetSampler() const { return m_Sampler; }

    operator const VkImage&() const { return *m_VulkanImage; }

private:
    void Create();

 private:
  Ptr<VulkanImage> m_VulkanImage;
  VkImageView m_ImageView;
  VkSampler m_Sampler;

  Ref<VulkanDevice> m_VulkanDevice;
};

DODO_END_NAMESPACE