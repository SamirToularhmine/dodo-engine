#include <DodoEngine/Renderer/GltfLoader.h>

#include <DodoEngine/Renderer/Mesh.h>
#include <DodoEngine/Renderer/Vertex.h>
#include <DodoEngine/Utils/Log.h>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>

DODO_BEGIN_NAMESPACE

enum AttributeType { POSITION, NORMAL, TANGENT, TEXCOORD_n, COLOR_n, JOINTS_n, WEIGHTS_n };

enum PrimitiveMode { POINTS = 0, LINES = 1, LINE_LOOP = 2, LINE_STRIP = 3, TRIANGLES = 4, TRIANGLE_STRIP = 5, TRIANGLE_FAN = 6 };

void GltfLoader::ProcessNode(const tinygltf::Node& _node, const tinygltf::Model& _model, std::vector<glm::vec3>& _vertices, std::vector<uint32_t>& _indices,
                             std::vector<glm::vec2>& _texCoords, std::vector<std::string>& _textureFiles, uint32_t _indexOffset, glm::mat4 _accTransform) {

  const tinygltf::Mesh& nodeMesh = _model.meshes[_node.mesh];
  
  for (const tinygltf::Primitive& primitive : nodeMesh.primitives)
  {
    const PrimitiveMode primitiveMode = static_cast<PrimitiveMode>(primitive.mode);
    if (primitive.material >= 0) {
      const tinygltf::Material& material = _model.materials[primitive.material];
      const tinygltf::TextureInfo& textureInfo = material.pbrMetallicRoughness.baseColorTexture;
      if (textureInfo.index >= 0) {
        const tinygltf::Texture texture = _model.textures[textureInfo.index];
        const tinygltf::Image& image = _model.images[texture.source];
        _textureFiles.push_back(image.uri);
      }
    }

    // Transformation
    if (!_node.translation.empty()) {
      const glm::vec3 localTranslation = glm::vec3(_node.translation[0], _node.translation[1], _node.translation[2]);
      _accTransform = glm::translate(_accTransform, localTranslation);
    }

    if (!_node.rotation.empty()) {
      const glm::vec3 localRotation = glm::vec3(_node.rotation[0], _node.rotation[1], _node.rotation[2]);
      _accTransform = glm::rotate(_accTransform, glm::radians(localRotation.x), glm::vec3(1, 0, 0));
      _accTransform = glm::rotate(_accTransform, glm::radians(localRotation.y), glm::vec3(0, 1, 0));
      _accTransform = glm::rotate(_accTransform, glm::radians(localRotation.z), glm::vec3(0, 0, 1));
    }

    if (!_node.scale.empty()) {
      const glm::vec3 localScale = glm::vec3(_node.scale[0], _node.scale[1], _node.scale[2]);
      _accTransform = glm::scale(_accTransform, localScale);
    }

    // Indices
    const tinygltf::Accessor indicesAccessor = _model.accessors[primitive.indices];
    const tinygltf::BufferView indicesBufferView = _model.bufferViews[indicesAccessor.bufferView];
    const tinygltf::Buffer indicesBuffer = _model.buffers[indicesBufferView.buffer];

    const uint32_t indicesSize = _indices.size();
    _indices.resize(indicesSize + indicesAccessor.count);
    if (indicesAccessor.componentType == 5125) {
      std::vector<uint32_t> tempIndices(indicesAccessor.count);
      std::memcpy(tempIndices.data(), indicesBuffer.data.data() + indicesBufferView.byteOffset, indicesAccessor.count * sizeof(uint32_t));
      for (const uint32_t& index : tempIndices) {
        _indices.push_back(index + _indexOffset);
      }
    } else {
      std::vector<uint16_t> tempIndices(indicesAccessor.count);
      std::memcpy(tempIndices.data(), indicesBuffer.data.data() + indicesBufferView.byteOffset, indicesAccessor.count * sizeof(uint16_t));
      for (const uint16_t& index : tempIndices) {
        _indices.push_back(static_cast<uint32_t>(index + _indexOffset));
      }
    }

    // Vertices
    const tinygltf::Accessor verticesAccessor = _model.accessors[primitive.attributes.at("POSITION")];
    const tinygltf::BufferView verticesBufferView = _model.bufferViews[verticesAccessor.bufferView];
    const tinygltf::Buffer verticesBuffer = _model.buffers[verticesBufferView.buffer];
    std::vector<float> vertices(verticesAccessor.count * 3);
    std::memcpy(vertices.data(), verticesBuffer.data.data() + verticesBufferView.byteOffset, verticesAccessor.count * 3 * sizeof(float));

    // Texcoords
    const tinygltf::Accessor texCoordsAccessor = _model.accessors[primitive.attributes.at("TEXCOORD_0")];
    const tinygltf::BufferView texCoordsBufferView = _model.bufferViews[texCoordsAccessor.bufferView];
    const tinygltf::Buffer texCoordsBuffer = _model.buffers[texCoordsBufferView.buffer];

    std::vector<float> texCoords(texCoordsAccessor.count * 2);
    std::memcpy(texCoords.data(), texCoordsBuffer.data.data() + texCoordsBufferView.byteOffset, texCoordsAccessor.count * 2 * sizeof(float));

    for (uint32_t i = 0; i < vertices.size() - 2; i += 3) {
      // glm::vec3 transformedVector = glm::vec3(vertices[i], vertices[i + 1], vertices[i + 3]) * localTranslation;
      glm::vec3 transformedVector = _accTransform * glm::vec4(vertices[i], vertices[i + 1], vertices[i + 2], 1.0f);
      _vertices.push_back(transformedVector);
    }

    for (uint32_t i = 0; i < texCoords.size() - 1; i += 2) {
      _texCoords.push_back({texCoords[i], texCoords[i + 1]});
    }

    _indexOffset += vertices.size();
  }

  // Children
  for (const auto& nodeId : _node.children) {
    ProcessNode(_model.nodes[nodeId], _model, _vertices, _indices, _texCoords, _textureFiles, _indexOffset, _accTransform);
  }
}

std::vector<Ref<Mesh>> GltfLoader::LoadFromFile(const char* _modelFileName) {
  tinygltf::Model model;
  tinygltf::TinyGLTF loader;
  loader.RemoveImageLoader();
  std::string err, warn;
  std::vector<Ref<Mesh>> meshes;

  bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, _modelFileName);
  //bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, argv[1]); // for binary glTF(.glb)

  if (!warn.empty()) {
    DODO_WARN("[tinygltf] {}", warn.c_str());
  }

  if (!err.empty()) {
    DODO_ERROR("[tinygltf] {}", err.c_str());
  }

  if (!ret) {
    DODO_CRITICAL("[tinygltf] Failed to load model at path {}", _modelFileName);
  }

  const tinygltf::Scene& defaultScene = model.scenes[model.defaultScene];
  const std::vector<int>& rootNodes = defaultScene.nodes;
  const std::filesystem::path modelPath = _modelFileName;

  for (const int& nodeIndex : rootNodes) {
    std::vector<uint32_t> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texCoords;
    std::vector<std::string> textureFiles;
    ProcessNode(model.nodes[nodeIndex], model, vertices, indices, texCoords, textureFiles, vertices.size(), glm::mat4(1.0f));

    std::vector<Vertex> finalVertices;
    for (int i = 0; i < vertices.size(); ++i) {
      finalVertices.push_back({vertices[i], DEFAULT_COLOR, texCoords[i]});
    }

    if (!textureFiles.empty()) {
      const std::string& textureFilePath = (modelPath.parent_path() / textureFiles.front()).string();
      meshes.push_back(Mesh::Create(finalVertices, indices, textureFilePath.c_str()));

    } else {
      meshes.push_back(Mesh::Create(finalVertices, indices, Texture::DEFAULT_TEXTURE_PATH));
    }
  }

  return meshes;
}

DODO_END_NAMESPACE
