#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Platform/Vulkan/VulkanTextureImage.h>

#include <string>

DODO_BEGIN_NAMESPACE

using TextureId = uint32_t;

static TextureId TEXTURE_ID = 0;

struct TextureIdProvider {

  static TextureId GetId() { return TEXTURE_ID++; }
};

class Texture {
 public:
  static constexpr const char* DEFAULT_TEXTURE_PATH = "resources/textures/default_texture.png";

  Texture(Ptr<VulkanTextureImage>& _vulkanTextureImage, uint32_t _width, uint32_t _height, uint32_t _channels, const std::string& _filePath);

  static Ptr<Texture> LoadFromFile(const char* _fileName);

  const Ptr<VulkanTextureImage>& GetTextureImage() const { return m_VulkanTextureImage; }

  TextureId GetId() const { return m_Id; }

  ~Texture() = default;

 private:
  uint32_t m_Id;
  std::string m_FilePath;
  Ptr<VulkanTextureImage> m_VulkanTextureImage;
  uint32_t m_Width;
  uint32_t m_Height;
  uint32_t m_Channels;
};

DODO_END_NAMESPACE