#pragma once

#include <DodoEngine/Core/Types.h>

#include <tiny_gltf.h>

#include <glm/glm.hpp>
#include <vector>

#include <filesystem>

DODO_BEGIN_NAMESPACE

class Texture;
struct Mesh;
struct Model;
struct Vertex;

enum class AttributeType
{
  POSITION,
  NORMAL,
  TANGENT,
  TEXCOORD_n,
  COLOR_n,
  JOINTS_n,
  WEIGHTS_n
};

enum class AccessorType
{
  SCALAR,
  VEC2,
  VEC3,
  VEC4,
  MAT2,
  MAT3,
  MAT4
};

struct AttributeSpec
{

  AttributeType m_AttributeType;
  AccessorType m_AccessorType;
  uint32_t m_Dimension;
  uint32_t m_IterSize;
  uint32_t m_Increment;

  AttributeSpec(AttributeType _attributeType, AccessorType _accessorType)
      : m_AttributeType(_attributeType), m_AccessorType(_accessorType),
        m_Dimension(GetDimension(_accessorType))
  {
  }

  static std::string ToString(AttributeType _attribute)
  {
    switch (_attribute)
    {
    case AttributeType::POSITION:
      return "POSITION";

    case AttributeType::NORMAL:
      return "NORMAL";

    case AttributeType::TEXCOORD_n:
      return "TEXCOORD_0";
    };
  }

  static uint8_t GetDimension(AccessorType _accessorType)
  {
    switch (_accessorType)
    {
    case AccessorType::SCALAR:
      return 1;

    case AccessorType::VEC2:
      return 2;

    case AccessorType::VEC3:
      return 3;
    };
  }
};

enum PrimitiveMode
{
  POINTS = 0,
  LINES = 1,
  LINE_LOOP = 2,
  LINE_STRIP = 3,
  TRIANGLES = 4,
  TRIANGLE_STRIP = 5,
  TRIANGLE_FAN = 6
};

class GltfLoader
{
  static inline const std::string RESOURCES_DIRECTORY = "resources/models";

public:
  static Ref<Model> LoadFromFile(const std::string& _modelFileName);

private:
  static constexpr glm::vec4 DEFAULT_COLOR = {0.0f, 0.0f, 1.0f, 1.0f};

  static void ProcessMeshNode(tinygltf::Model &model, const int &nodeIndex,
                              const std::filesystem::path &modelPath,
                              std::vector<Ref<Mesh>> &meshes, const glm::mat4 &_accTransform);

  static void ProcessPrimitive(const tinygltf::Primitive &_primitive, const tinygltf::Model &_model,
                               std::vector<Vertex> &_vertices, std::vector<uint32_t> &_indices,
                               glm::mat4 _accTransform);

  static void ProcessTextureFiles(const tinygltf::Primitive &_primitive,
                                  const tinygltf::Model &_model,
                                  std::vector<std::string> &_textureFiles);

  static void ProcessIndices(const tinygltf::Primitive &_primitive, const tinygltf::Model &_model,
                             std::vector<uint32_t> &_indices);

  static void ProcessVertices(const tinygltf::Primitive &_primitive, const tinygltf::Model &_model,
                              std::vector<glm::vec3> &_vertices, glm::mat4 &_accTransform);

  static void ProcessTexCoords(const tinygltf::Primitive &_primitive, const tinygltf::Model &_model,
                               std::vector<glm::vec2> &_texCoords);

  static void ProcessNormals(const tinygltf::Primitive &_primitive, const tinygltf::Model &_model,
                             std::vector<glm::vec3> &_normals);

  template <typename T>
  static AttributeSpec GetDataByAccessor(const tinygltf::Accessor &_accessor,
                                         AttributeSpec _attributeSpec,
                                         const tinygltf::Primitive &_primitive,
                                         const tinygltf::Model &_model, std::vector<T> &_outData);

  static uint32_t GetAccessorByName(AttributeType _accessorName,
                                    const tinygltf::Primitive &_primitive);
};

template <typename T>
AttributeSpec GltfLoader::GetDataByAccessor(const tinygltf::Accessor &_accessor,
                                            AttributeSpec _attributeSpec,
                                            const tinygltf::Primitive &_primitive,
                                            const tinygltf::Model &_model, std::vector<T> &_outData)
{
  const tinygltf::BufferView bufferView = _model.bufferViews[_accessor.bufferView];
  const tinygltf::Buffer buffer = _model.buffers[bufferView.buffer];
  const bool stridedValues = bufferView.byteStride != 0;
  const uint32_t attributeDimension = AttributeSpec::GetDimension(_attributeSpec.m_AccessorType);

  // Retrieving data from the buffer
  uint32_t dataSize = _accessor.count * attributeDimension * sizeof(T);
  uint32_t outputSize = _accessor.count * attributeDimension;
  uint32_t offset = bufferView.byteOffset + _accessor.byteOffset;
  uint32_t increment = attributeDimension;
  uint32_t iterSize = _accessor.count * attributeDimension;

  if (stridedValues)
  {
    outputSize = bufferView.byteLength * sizeof(T);
    increment = bufferView.byteStride / sizeof(T);
  }

  _outData.resize(dataSize);
  std::memcpy(_outData.data(), buffer.data.data() + offset, dataSize);

  _attributeSpec.m_IterSize = stridedValues ? _outData.size() : iterSize;
  _attributeSpec.m_Increment = increment;

  return _attributeSpec;
}

DODO_END_NAMESPACE
