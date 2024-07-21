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

void GltfLoader::ProcessPrimitive(const tinygltf::Primitive& _primitive, const tinygltf::Model& _model, std::vector<Vertex>& _vertices,
                                  std::vector<uint32_t>& _indices, glm::mat4 _accTransform) {
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec2> texCoords;

  ProcessIndices(_primitive, _model, _indices);
  ProcessVertices(_primitive, _model, vertices, _accTransform);
  ProcessTexCoords(_primitive, _model, texCoords);

  if (vertices.size() != texCoords.size())
  {
    DODO_CRITICAL("An error has ocurred during the loading of the mesh : texcoords and vertices size are not equal");
  }

  const uint32_t verticesSize = vertices.size();
  _vertices.resize(verticesSize);
  for (uint32_t i = 0; i < verticesSize; ++i) {
    _vertices[i]= {vertices[i], DEFAULT_COLOR, texCoords[i]};
  }
}

void GltfLoader::ProcessTexCoords(const tinygltf::Primitive& _primitive, const tinygltf::Model& _model, std::vector<glm::vec2>& _texCoords) {
  std::vector<float> texCoords;
  const tinygltf::Accessor& accessor = _model.accessors.at(GetAccessorByName(AttributeType::TEXCOORD_n, _primitive));
  const AttributeSpec attributeSpec = GetDataByAccessor<float>(accessor, {AttributeType::TEXCOORD_n, AccessorType::VEC2}, _primitive, _model, texCoords);

  // Tex Coords
  for (uint32_t i = 0; i < attributeSpec.m_IterSize; i += attributeSpec.m_Increment) {
    _texCoords.push_back({texCoords[i], texCoords[i + 1]});
  }
}

void GltfLoader::ProcessVertices(const tinygltf::Primitive& _primitive, const tinygltf::Model& _model, std::vector<glm::vec3>& _vertices,
                                 glm::mat4& _accTransform) {
  std::vector<float> vertices;
  const tinygltf::Accessor& accessor = _model.accessors.at(GetAccessorByName(AttributeType::POSITION, _primitive));
  const AttributeSpec attributeSpec = GetDataByAccessor<float>(accessor, {AttributeType::POSITION, AccessorType::VEC3}, _primitive, _model, vertices);

  // Vertices
  for (uint32_t i = 0; i < attributeSpec.m_IterSize; i += attributeSpec.m_Increment) {
    glm::vec3 transformedVector = _accTransform * glm::vec4(vertices[i], vertices[i + 1], vertices[i + 2], 1.0f);
    _vertices.push_back(transformedVector);
  }
}

void GltfLoader::ProcessIndices(const tinygltf::Primitive& _primitive, const tinygltf::Model& _model, std::vector<uint32_t>& _indices) {
  // Indices
  /*const tinygltf::Accessor& accessor = _model.accessors[_primitive.indices];
  if (accessor.componentType == 5125) {
    std::vector<uint32_t> indices;
    const AttributeSpec attributeSpec = GetDataByAccessor<uint32_t>(accessor, {AttributeType::POSITION, AccessorType::SCALAR}, _primitive, _model, indices);

    for (uint32_t i = 0; i < attributeSpec.m_IterSize; i += attributeSpec.m_Increment) {
      _indices.push_back(indices[i]);
    }
  } else {

    std::vector<uint16_t> indices;
    const AttributeSpec attributeSpec = GetDataByAccessor<uint16_t>(accessor, {AttributeType::POSITION, AccessorType::SCALAR}, _primitive, _model, indices);

    for (uint32_t i = 0; i < attributeSpec.m_IterSize; i += attributeSpec.m_Increment) {
      _indices.push_back(indices[i]);
    }
  }*/

  // Indices
  const tinygltf::Accessor indicesAccessor = _model.accessors[_primitive.indices];
  const tinygltf::BufferView indicesBufferView = _model.bufferViews[indicesAccessor.bufferView];
  const tinygltf::Buffer indicesBuffer = _model.buffers[indicesBufferView.buffer];

  if (indicesAccessor.componentType == 5125) {
    std::vector<uint32_t> tempIndices(indicesBufferView.byteLength / sizeof(uint32_t));
    std::memcpy(tempIndices.data(), indicesBuffer.data.data() + indicesBufferView.byteOffset + indicesAccessor.byteOffset, indicesBufferView.byteLength);
    const uint32_t increment = indicesBufferView.byteStride != 0 ? indicesBufferView.byteStride / sizeof(uint32_t) : 1;
    const uint32_t iterSize = indicesBufferView.byteStride == 0 ? indicesAccessor.count : tempIndices.size();

    for (uint64_t i = 0; i < iterSize; i += increment) {
      _indices.push_back(tempIndices[i]);
    }
  } else {
    std::vector<uint16_t> tempIndices(indicesBufferView.byteLength / sizeof(uint16_t));
    std::memcpy(tempIndices.data(), indicesBuffer.data.data() + indicesBufferView.byteOffset + indicesAccessor.byteOffset, indicesBufferView.byteLength);
    const uint32_t increment = indicesBufferView.byteStride != 0 ? indicesBufferView.byteStride / sizeof(uint16_t) : 1;
    const uint32_t iterSize = indicesBufferView.byteStride == 0 ? indicesAccessor.count : tempIndices.size();

    for (uint32_t i = 0; i < iterSize; i += increment) {
      _indices.push_back(tempIndices[i]);
    }
  }
}

void GltfLoader::ProcessTextureFiles(const tinygltf::Primitive& _primitive, const tinygltf::Model& _model, std::vector<std::string>& _textureFiles) {
  if (_primitive.material < 0) {
    return;
  }

  const tinygltf::Material& material = _model.materials[_primitive.material];
  const tinygltf::TextureInfo& textureInfo = material.pbrMetallicRoughness.baseColorTexture;

  if (textureInfo.index < 0) {
    return;
  }

  const tinygltf::Texture texture = _model.textures[textureInfo.index];
  const tinygltf::Image& image = _model.images[texture.source];
  _textureFiles.push_back(image.uri);
}

void GltfLoader::ProcessNode(tinygltf::Model& model, const int& nodeIndex, const std::filesystem::path& modelPath, std::vector<dodo::Ref<dodo::Mesh>>& meshes, const glm::mat4& _accTransform) {
  tinygltf::Node& node = model.nodes[nodeIndex];
  tinygltf::Mesh& nodeMesh = model.meshes[node.mesh];
  glm::mat4 accTransform = _accTransform;

  // Transformation
  if (!node.translation.empty()) {
    const glm::vec3 localTranslation = glm::vec3(node.translation[0], node.translation[1], node.translation[2]);
    accTransform = glm::translate(accTransform, localTranslation);
  }

  if (!node.rotation.empty()) {
    const glm::vec3 localRotation = glm::vec3(node.rotation[0], node.rotation[1], node.rotation[2]);
    accTransform = glm::rotate(accTransform, glm::radians(localRotation.x), glm::vec3(1, 0, 0));
    accTransform = glm::rotate(accTransform, glm::radians(localRotation.y), glm::vec3(0, 1, 0));
    accTransform = glm::rotate(accTransform, glm::radians(localRotation.z), glm::vec3(0, 0, 1));
  }

  if (!node.scale.empty()) {
    const glm::vec3 localScale = glm::vec3(node.scale[0], node.scale[1], node.scale[2]);
    accTransform = glm::scale(accTransform, localScale);
  }

  for (const tinygltf::Primitive& primitive : nodeMesh.primitives) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<std::string> textureFiles;

    ProcessTextureFiles(primitive, model, textureFiles);
    ProcessPrimitive(primitive, model, vertices, indices, accTransform);

    const std::string textureFile = !textureFiles.empty() ? (modelPath.parent_path() / textureFiles.front()).string() : Texture::DEFAULT_TEXTURE_PATH;
    meshes.push_back(Mesh::Create(vertices, indices, textureFile.c_str()));
  }

  // Children
  for (const auto& childrenNodeId : node.children) {
    ProcessNode(model, childrenNodeId, modelPath, meshes, accTransform);
  }
}

uint32_t GltfLoader::GetAccessorByName(AttributeType _accessorName, const tinygltf::Primitive& _primitive) {
  const std::string accessorName = AttributeSpec::ToString(_accessorName);
  if (!_primitive.attributes.contains(accessorName)) {
    DODO_CRITICAL("Accessor name {} not found in the primitive", accessorName);
  }

  return _primitive.attributes.at(accessorName);
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
    ProcessNode(model, nodeIndex, modelPath, meshes, glm::mat4(1.0f));
  }

  return meshes;
}

DODO_END_NAMESPACE
