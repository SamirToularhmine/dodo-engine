#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Renderer/GltfLoader.h>
#include <DodoEngine/Renderer/Model.h>
#include <DodoEngine/Renderer/ObjLoader.h>

DODO_BEGIN_NAMESPACE

enum class SupportedModelFileType
{
  OBJ,
  GLTF
};

struct ModelLoader
{
  static Ref<Model> LoadModel(const std::string &_modelFilePath,
                              const SupportedModelFileType &_modelFileType)
  {
    switch (_modelFileType)
    {
    case SupportedModelFileType::OBJ:
      return ObjLoader::LoadFromFile(_modelFilePath);
    case SupportedModelFileType::GLTF:
      return GltfLoader::LoadFromFile(_modelFilePath);
    }
  }
};

DODO_END_NAMESPACE