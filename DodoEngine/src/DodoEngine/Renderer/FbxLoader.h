#pragma once

#include <DodoEngine/Core/Types.h>

DODO_BEGIN_NAMESPACE

class Mesh;
class Model;

class FbxLoader
{
public:
  static Ref<Model> LoadFromFile(const std::string& _fileName);
};

DODO_END_NAMESPACE