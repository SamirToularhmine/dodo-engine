#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Renderer/Mesh.h>

#include <vector>

DODO_BEGIN_NAMESPACE

using ModelId = uint32_t;

static ModelId MODEL_ID = 0;

struct ModelIdProvider {

  static ModelId GetId() { return MODEL_ID++; }
};

struct Model {
  ModelId m_Id;
  std::vector<Ref<Mesh>> m_Meshes;
};

DODO_END_NAMESPACE
