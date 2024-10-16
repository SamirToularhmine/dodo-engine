#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Debug/ImGuiLayer.h>
#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Renderer/Model.h>
#include <DodoEngine/Renderer/Renderer.h>
#include <DodoEngine/Renderer/Texture.h>
#include <DodoEngine/Renderer/UniformBufferObject.h>

#include <map>
#include <vector>

class GLFWwindow;

DODO_BEGIN_NAMESPACE

class Camera;
class VulkanBuffer;
class VulkanContext;
class VulkanDescriptorSet;
class VulkanDescriptorSetLayout;
class VulkanGraphicPipeline;
struct VulkanSwapChainData;
struct VulkanRenderPassData;

struct RendererData {
  std::map<ModelId, std::vector<glm::mat4>> m_ModelsTransform;
  std::map<ModelId, Ref<Model>> m_ModelsToDraw;
  std::map<TextureId, Ref<Texture>> m_ModelsTexture;
  VulkanRenderPassData m_RenderPassData{0};

  std::vector<Ref<Texture>> NeededTextures() const {
    std::vector<Ref<Texture>> neededTextures;
    for (const auto& [textureId, texture] : m_ModelsTexture) {
      neededTextures.push_back(texture);
    }

    return neededTextures;
  }

  void ResetTransforms() { m_ModelsTransform.clear(); }

  void Clear() {
    ResetTransforms();
    m_ModelsToDraw.clear();
    m_ModelsTexture.clear();
  }
};

struct Light {
  glm::vec3 m_Position;
};

class VulkanRenderer : public Renderer {
 public:
  VulkanRenderer(VulkanContext& _vulkanContext);

  ~VulkanRenderer() override = default;

  void Init(const Window& _window) override;
  void Update(const uint32_t& _frameNumber, const Camera& _camera, const Light& _light, float _deltaTime) override;
  void Shutdown() override;

  bool BeginUIRenderPass() override;
  void EndUIRenderPass() override;

  void RegisterModel(Ref<Model>& _mesh) override;

  void DrawQuad(const MeshTransform& _meshTransform) override;
  void DrawCube(const MeshTransform& _meshTransform) override;
  void DrawModel(Ref<Model>& _mesh, const MeshTransform& _meshTransform) override;

  uint32_t GetFrameIndex() const override;

 private:
  std::vector<UniformBufferObject> BatchMvpUbo(const Camera& _camera, const VulkanSwapChainData& _vulkanSwapChainData, float _deltaTime);

  void BeginRenderPass(const uint32_t& _frameNumber);
  void EndRenderPass();

 private:
  VulkanContext& m_VulkanContext;
  RendererData m_RendererData;
  Ref<VulkanBuffer> m_UniformBuffer;
  UniformBufferObject m_UniformMvp;
  
  Ref<VulkanGraphicPipeline> m_DefaultGraphicPipeline;
  Ref<VulkanDescriptorSet> m_DefaultDescriptorSet;

  Ref<VulkanGraphicPipeline> m_GridGraphicPipeline;
  Ref<VulkanDescriptorSet> m_GridDescriptorSet;
};

DODO_END_NAMESPACE