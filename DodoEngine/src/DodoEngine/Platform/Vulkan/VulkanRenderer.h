#pragma once

#include <DodoEngine/Core/Types.h>
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
class Scene;
class VulkanBuffer;
class VulkanContext;
class VulkanCommandBuffer;
class VulkanDescriptorSet;
class VulkanDescriptorSetLayout;
class VulkanGraphicPipeline;
struct VulkanSwapChainData;
struct VulkanRenderPassData;

struct RendererData
{
  std::map<ModelId, std::vector<glm::mat4>> m_ModelsTransform;
  std::map<ModelId, Ref<Model>> m_ModelsToDraw;
  std::map<TextureId, Ref<Texture>> m_ModelsTexture;

  std::vector<Ref<Texture>> NeededTextures() const
  {
    std::vector<Ref<Texture>> neededTextures;
    for (const auto &[textureId, texture] : m_ModelsTexture)
    {
      neededTextures.push_back(texture);
    }

    return neededTextures;
  }

  void ResetTransforms() { m_ModelsTransform.clear(); }

  void Clear()
  {
    ResetTransforms();
    m_ModelsToDraw.clear();
    m_ModelsTexture.clear();
  }
};

struct Light
{
  glm::vec3 m_Position;
};

struct Frame
{
  uint32_t m_FrameNumber;
  uint32_t m_ImageIndex;
  Ref<VulkanCommandBuffer> m_CommandBuffer;
  bool m_Error = false;
};

struct RenderPass
{
  Frame &m_Frame;
  Ref<VulkanRenderPass> m_RenderPass;
  Ref<VulkanFrameBuffer> m_FrameBuffer;
  Ref<VulkanDescriptorSet> m_DescriptorSet;
};

class VulkanRenderer : public Renderer
{
public:
  VulkanRenderer(VulkanContext &_vulkanContext);
  ~VulkanRenderer() override = default;

  void Init(const Window &_window) override;
  void Update(Frame &_frame, const Scene &_scene, Camera &_camera, const Light &_light,
              float _deltaTime) override;
  void Shutdown() override;

  RenderPass BeginUIRenderPass(Frame &_frame) override;
  void EndUIRenderPass(const RenderPass &_renderPass) override;

  Frame BeginFrame(const uint32_t &_frameNumber) override;
  void EndFrame(const Frame &_frame) override;

private:
  void UpdateUbo(const Camera &_camera, const Scene &_scene);

  RenderPass BeginSceneRenderPass(Frame &_frame);
  void EndSceneRenderPass(const RenderPass &_renderPass);

private:
  VulkanContext &m_VulkanContext;

  RendererData m_RendererData;
  Ref<VulkanBuffer> m_UniformBuffer;
  UniformBufferObject m_UniformMvp;

  Ref<VulkanGraphicPipeline> m_DefaultGraphicPipeline;
  Ref<VulkanDescriptorSet> m_DefaultDescriptorSet;

  Ref<VulkanGraphicPipeline> m_GridGraphicPipeline;
  Ref<VulkanDescriptorSet> m_GridDescriptorSet;
};

DODO_END_NAMESPACE