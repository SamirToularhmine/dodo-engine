#include <DodoEngine/Renderer/Model.h>
#include <DodoEngine/Renderer/ObjLoader.h>
#include <DodoEngine/Utils/Log.h>
#include <DodoEngine/Utils/Utils.h>

#define TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_USE_MAPBOX_EARCUT
#include <tiny_obj_loader.h>

#include <memory>

DODO_BEGIN_NAMESPACE

Ref<Model> ObjLoader::LoadFromFile(const std::string& _filePath, const char *_textureFileName)
{
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;

  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, _filePath.c_str()))
  {
    return nullptr;
  }

  for (const auto &shape : shapes)
  {
    for (const auto &index : shape.mesh.indices)
    {
      Vertex vertex{};

      vertex.m_Positions = {attrib.vertices[3 * index.vertex_index + 0],
                            attrib.vertices[3 * index.vertex_index + 1],
                            attrib.vertices[3 * index.vertex_index + 2]};
      vertex.m_TexCoords = {attrib.texcoords[2 * index.texcoord_index + 0],
                            1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};
      // vertex.m_Color = {attrib.colors[3 * index.texcoord_index + 0], attrib.colors[3 *
      // index.texcoord_index + 1], attrib.colors[3 * index.texcoord_index + 2]};

      vertices.push_back(vertex);
      indices.push_back(indices.size());
    }
  }

  return Model::Create({Mesh::Create({vertices}, {indices}, _textureFileName)}, _filePath);
}

DODO_END_NAMESPACE