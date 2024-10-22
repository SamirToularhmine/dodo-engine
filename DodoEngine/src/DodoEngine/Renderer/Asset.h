#pragma once

#include <DodoEngine/Core/Types.h>

#include <string>

DODO_BEGIN_NAMESPACE

using AssetId = uint32_t;

template<typename AssetType>
struct Asset
{
  AssetId m_Id;
  std::string m_FilePath;
  Ref<AssetType> m_Data;
};

DODO_END_NAMESPACE
