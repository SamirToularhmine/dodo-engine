#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Renderer/Texture.h>

#include <optional>

DODO_BEGIN_NAMESPACE

struct Model;

struct ObjLoader {
  static Ref<Model> LoadFromFile(const std::string& _filePath, const char* _textureFileName = Texture::DEFAULT_TEXTURE_PATH);
};

DODO_END_NAMESPACE
