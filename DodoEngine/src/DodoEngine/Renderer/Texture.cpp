#include <DodoEngine/Renderer/Texture.h>

#include <DodoEngine/Platform/Vulkan/VulkanBuffer.h>
#include <DodoEngine/Platform/Vulkan/VulkanCommandBuffer.h>
#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Platform/Vulkan/VulkanDevice.h>
#include <DodoEngine/Platform/Vulkan/VulkanPhysicalDevice.h>
#include <DodoEngine/Platform/Vulkan/VulkanTextureImage.h>
#include <DodoEngine/Utils/Utils.h>

#include <memory>

#include <stb_image/stb_image.h>
#include <vk_mem_alloc.h>
#include <volk.h>

DODO_BEGIN_NAMESPACE

Texture::Texture(Ptr<VulkanTextureImage>& _vulkanTextureImage, uint32_t _width, uint32_t _height, uint32_t _channels, const std::string& _filePath)
    : m_Id(TEXTURE_ID++), m_VulkanTextureImage(std::move(_vulkanTextureImage)), m_Width(_width), m_Height(_height), m_Channels(_channels), m_FilePath(_filePath) {}

Ptr<Texture> Texture::LoadFromFile(const char* _fileName) {
  int texWidth, texHeight, texChannels;
  stbi_uc* pixels = stbi_load(_fileName, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

  if (!pixels) {
    DODO_WARN("Couldn't load texture at path : {}. Loading default texture...", _fileName);
    pixels = stbi_load(DEFAULT_TEXTURE_PATH, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
  }

  uint32_t imageSize = texWidth * texHeight * 4;
  Ptr<VulkanTextureImage> vulkanTextureImage = std::make_unique<VulkanTextureImage>(pixels, imageSize, texWidth, texHeight);

  stbi_image_free(pixels);

  return std::move(std::make_unique<Texture>(vulkanTextureImage, texWidth, texHeight, texChannels, _fileName));
}

DODO_END_NAMESPACE
