#pragma once

#include <DodoEngine/Core/Types.h>

DODO_BEGIN_NAMESPACE

class Mesh;

class FbxLoader
{
public:
  static Ref<Mesh> LoadFromFile(const char* _fileName);
};

DODO_END_NAMESPACE