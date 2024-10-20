#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Renderer/Texture.h>

#include <optional>

DODO_BEGIN_NAMESPACE

struct Mesh;

struct ObjLoader {
  static Ref<Mesh> LoadFromFile(const char* _modelFileName, const char* _textureFileName = Texture::DEFAULT_TEXTURE_PATH);
};

DODO_END_NAMESPACE
