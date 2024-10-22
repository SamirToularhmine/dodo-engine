#include <DodoEngine/Renderer/FbxLoader.h>

#include <DodoEngine/Renderer/Model.h>
#include <DodoEngine/Utils/Utils.h>

#include <ofbx.h>

#include <memory>

DODO_BEGIN_NAMESPACE

Ref<Model> FbxLoader::LoadFromFile(const std::string& _fileName) {
  std::vector<Vertex> vertices;
  std::vector<char> fbxFile = readFile(_fileName.c_str());

  ofbx::LoadFlags flags = ofbx::LoadFlags::IGNORE_BLEND_SHAPES | ofbx::LoadFlags::IGNORE_CAMERAS | ofbx::LoadFlags::IGNORE_LIGHTS |
                          ofbx::LoadFlags::IGNORE_SKIN | ofbx::LoadFlags::IGNORE_BONES | ofbx::LoadFlags::IGNORE_PIVOTS | ofbx::LoadFlags::IGNORE_POSES |
                          ofbx::LoadFlags::IGNORE_VIDEOS | ofbx::LoadFlags::IGNORE_LIMBS | ofbx::LoadFlags::IGNORE_ANIMATIONS;

  const ofbx::IScene* ofbxScene = ofbx::load((ofbx::u8*)fbxFile.data(), fbxFile.size() * sizeof(char), (ofbx::u16)flags);
  const uint32_t meshCount = ofbxScene->getMeshCount();

  for (uint32_t meshIndex = 0; meshIndex < meshCount; ++meshIndex) {
    const ofbx::Mesh& mesh = *ofbxScene->getMesh(meshIndex);
    const ofbx::GeometryData& geom = mesh.getGeometryData();
    const ofbx::Vec3Attributes positions = geom.getPositions();
    const uint32_t geometryPartitionCount = geom.getPartitionCount();

    for (uint32_t partitionIndex = 0; partitionIndex < geometryPartitionCount; ++partitionIndex) {
      const ofbx::GeometryPartition& partition = geom.getPartition(partitionIndex);
      std::vector<int> triangleIndices(partition.max_polygon_triangles * 3);

      for (int polygonIndex = 0; polygonIndex < partition.polygon_count; ++polygonIndex) {
        const ofbx::GeometryPartition::Polygon& polygon = partition.polygons[polygonIndex];
        const uint32_t triangleCount = ofbx::triangulate(geom, polygon, triangleIndices.data());

        for (uint32_t i = 0; i < triangleCount; ++i) {
          ofbx::Vec3 v = positions.get(triangleIndices[i]);
          vertices.push_back({{v.x, v.y, v.z}, {0.0f, 0.0f, 1.0f}});
        }
      }
    }
  }

  // To fill with normals and index and stuff
  return Model::Create({Mesh::Create({vertices}, {}, {})}, _fileName);
}

DODO_END_NAMESPACE