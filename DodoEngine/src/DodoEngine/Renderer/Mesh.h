#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Platform/Vulkan/VulkanBuffer.h>
#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Platform/Vulkan/VulkanDevice.h>
#include <DodoEngine/Platform/Vulkan/VulkanPhysicalDevice.h>
#include <DodoEngine/Renderer/Texture.h>
#include <DodoEngine/Renderer/TextureManager.h>
#include <DodoEngine/Renderer/Vertex.h>

#include <vector>

DODO_BEGIN_NAMESPACE

struct Mesh
{
  Ref<VulkanBuffer> m_VertexBuffer;
  Ref<VulkanBuffer> m_IndicesBuffer;
  Ref<Texture> m_Texture;

  bool IsIndexed() { return m_IndicesBuffer != nullptr; }

  static Ref<Mesh> Create(const std::vector<Vertex> &_vertices,
                          const std::vector<uint32_t> &_indices,
                          const char *_textureFileName = Texture::DEFAULT_TEXTURE_PATH)
  {
    Ref<Mesh> mesh = Mesh::Create(_vertices, _textureFileName);

    // Indices
    const uint32_t indicesAllocationSize = sizeof(uint32_t) * _indices.size();
    Ref<VulkanBuffer> indicesBuffer = MakePtr<VulkanBuffer>(
        VulkanBufferSpec{indicesAllocationSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                         VulkanBuffer::DEFAULT_MEMORY_PROPERTY_FLAGS});
    indicesBuffer->SetMemory<uint32_t>(_indices.data(), indicesAllocationSize);
    mesh->m_IndicesBuffer = std::move(indicesBuffer);

    return std::move(mesh);
  }

  static Ref<Mesh> Create(const std::vector<Vertex> &_vertices,
                          const char *_textureFileName = Texture::DEFAULT_TEXTURE_PATH)
  {
    // Vertices
    const uint32_t verticesAllocationSize = sizeof(Vertex) * _vertices.size();
    Ref<VulkanBuffer> vertexBuffer = MakePtr<VulkanBuffer>(
        VulkanBufferSpec{verticesAllocationSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                         VulkanBuffer::DEFAULT_MEMORY_PROPERTY_FLAGS});
    vertexBuffer->SetMemory<Vertex>(_vertices.data(), verticesAllocationSize);

    return std::move(MakeRef<Mesh>(std::move(vertexBuffer), nullptr,
                                   TextureManager::GetTexture(_textureFileName)));
  }
};

DODO_END_NAMESPACE