#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Renderer/Asset.h>

#include <string>
#include <unordered_map>

DODO_BEGIN_NAMESPACE

struct Model;
struct Texture;

class AssetManager
{
public:
  Ref<Asset<Model>> LoadModelAsset(const std::string &_assetFilePath,
                                   const SupportedModelFileType &_modelFileType);
  Ref<Asset<Texture>> LoadTextureAsset(const std::string &_assetFilePath);

  void UnloadAsset(const AssetId &_assetId);

private:
  AssetId GetNextAssetId();

private:
  std::unordered_map<AssetId, Ref<Asset<Model>>> m_LoadedModels;
  std::unordered_map<AssetId, Ref<Asset<Texture>>> m_LoadedTextures;
  AssetId m_LastAssetId;
};

DODO_END_NAMESPACE