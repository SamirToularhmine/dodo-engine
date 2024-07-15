#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Renderer/Mesh.h>
#include <DodoEngine/Renderer/Texture.h>

#include <tiny_gltf.h>
#include <glm/glm.hpp>

#include <vector>

DODO_BEGIN_NAMESPACE

class GltfLoader {
 public:
  static std::vector<Ref<Mesh>> LoadFromFile(const char* _modelFileName);

 private:
  static constexpr glm::vec4 DEFAULT_COLOR = {1.0f, 0.0f, 0.0f, 1.0f};

  static void ProcessNode(const tinygltf::Node& _node, const tinygltf::Model& _model, std::vector<glm::vec3>& _vertices, std::vector<uint32_t>& _indices,
                          std::vector<glm::vec2>& _texCoords, std::vector<std::string>& _textureFiles, uint32_t _indexOffset, glm::mat4 _accTransform);
};

DODO_END_NAMESPACE
