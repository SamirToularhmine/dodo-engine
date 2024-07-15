#include <DodoEngine/Renderer/TextureManager.h>

#include <DodoEngine/Renderer/Texture.h>

DODO_BEGIN_NAMESPACE

std::map<std::string, Ref<Texture>> TextureManager::s_Textures{};

const Ref<Texture>& TextureManager::GetTexture(const char* _filePath) {
  if (!s_Textures.contains(_filePath)) {
    s_Textures[_filePath] = std::move(Texture::LoadFromFile(_filePath));
  }

  return s_Textures[_filePath];
}

void TextureManager::UnloadTexture(const char* _filePath) {
  s_Textures.erase(_filePath);
}

void TextureManager::Shutdown() {
  s_Textures.clear();
}

DODO_END_NAMESPACE
