#pragma once

#include <DodoEngine/Core/Types.h>

#include <map>
#include <string>

DODO_BEGIN_NAMESPACE

class Texture;

class TextureManager
{
 public:
  static const Ref<Texture> GetTexture(const char* _filePath);
  static void UnloadTexture(const char* _filePath);
  static void Shutdown();

  private:
  static std::map<std::string, Ref<Texture>> s_Textures;
};

DODO_END_NAMESPACE