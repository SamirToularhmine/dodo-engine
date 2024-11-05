#include <DodoEngine/Renderer/AssetManager.h>
#include <DodoEngine/Renderer/ModelLoader.h>

#include <filesystem>

DODO_BEGIN_NAMESPACE

Ref<Asset<Model>> AssetManager::LoadModelAsset(const std::string &_assetFilePath,
                                               const SupportedModelFileType &_modelFileType)
{
  const AssetId newAssetId = GetNextAssetId();
  m_LoadedModels[newAssetId] = MakeRef<Asset<Model>>(
      Asset<Model>{.m_Id = newAssetId,
                   .m_FilePath = _assetFilePath,
                   .m_Data = ModelLoader::LoadModel(_assetFilePath, _modelFileType)});

  return m_LoadedModels[newAssetId];
}

Ref<Asset<Texture>> AssetManager::LoadTextureAsset(const std::string &_assetFilePath)
{
  const AssetId newAssetId = GetNextAssetId();
  m_LoadedTextures[newAssetId] =
      MakeRef<Asset<Texture>>(Asset<Texture>{.m_Id = newAssetId,
                                             .m_FilePath = _assetFilePath,
                                             .m_Data = Texture::LoadFromFile(_assetFilePath)});

  return m_LoadedTextures[newAssetId];
}

AssetId AssetManager::GetNextAssetId() { return m_LastAssetId++; }

DODO_END_NAMESPACE
