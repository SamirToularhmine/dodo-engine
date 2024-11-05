#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Renderer/Mesh.h>

#include <vector>

DODO_BEGIN_NAMESPACE

using ModelId = uint32_t;

static ModelId MODEL_ID = 0;

struct ModelIdProvider
{

  static ModelId GetId() { return MODEL_ID++; }
};

struct Model
{
  ModelId m_Id;
  std::string m_ModelPath;
  std::vector<Ref<Mesh>> m_Meshes;

  Model(const std::vector<Ref<Mesh>> &_meshes, const std::string &_modelPath)
      : m_Id(ModelIdProvider::GetId()), m_Meshes(_meshes), m_ModelPath(_modelPath)
  {
  }

  static Ref<Model> Create(const std::vector<Ref<Mesh>> &_meshes, const std::string &_modelPath)
  {
    return MakeRef<Model>(_meshes, _modelPath);
  }
};

DODO_END_NAMESPACE
