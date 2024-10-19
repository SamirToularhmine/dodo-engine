#include <DodoEngine/Platform/Vulkan/VulkanRenderer.h>

#include <DodoEngine/Core/Camera.h>
#include <DodoEngine/Core/Window.h>
#include <DodoEngine/Editor/Component.h>
#include <DodoEngine/Editor/Scene.h>
#include <DodoEngine/Platform/Vulkan/VulkanBuffer.h>
#include <DodoEngine/Platform/Vulkan/VulkanCommandBuffer.h>
#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Platform/Vulkan/VulkanDescriptorSet.h>
#include <DodoEngine/Platform/Vulkan/VulkanDescriptorSetLayout.h>
#include <DodoEngine/Platform/Vulkan/VulkanFrameBuffer.h>
#include <DodoEngine/Platform/Vulkan/VulkanGraphicPipeline.h>
#include <DodoEngine/Platform/Vulkan/VulkanSwapChain.h>
#include <DodoEngine/Platform/Vulkan/VulkanTextureImage.h>
#include <DodoEngine/Renderer/Mesh.h>
#include <DodoEngine/Renderer/MeshTransform.h>
#include <DodoEngine/Renderer/Model.h>
#include <DodoEngine/Renderer/Quad.h>
#include <DodoEngine/Renderer/RendererPushConstants.h>
#include <DodoEngine/Renderer/ShaderManager.h>
#include <DodoEngine/Renderer/Texture.h>
#include <DodoEngine/Renderer/UniformBufferObject.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

DODO_BEGIN_NAMESPACE

VulkanRenderer::VulkanRenderer(VulkanContext& _vulkanContext) : m_VulkanContext(_vulkanContext) {}

void VulkanRenderer::Init(const Window& _window) {
  m_VulkanContext.Init(_window);

  const VulkanSwapChainData& swapChainData = m_VulkanContext.GetSwapChain()->GetSpec();
  const Ref<VulkanDevice>& vulkanDevice = m_VulkanContext.GetVulkanDevice();

  // Creation of the default graphic pipeline
  {
    const std::vector<VulkanDescriptorSetLayoutSpec> defaultPipelineLayoutSpec = {
        VulkanDescriptorSetLayoutSpec{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT},
        VulkanDescriptorSetLayoutSpec{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 128, VK_SHADER_STAGE_FRAGMENT_BIT}};
    Ref<VulkanDescriptorSetLayout> defaultDescriptorSetLayout = std::make_shared<VulkanDescriptorSetLayout>(defaultPipelineLayoutSpec);

    const VulkanGraphicPipelineSpecification pipelineSpec{.m_VulkanDescriptorSetLayout = defaultDescriptorSetLayout,
                                                          .m_VertexShaderModule = ShaderManager::LoadShader("resources/shaders/default.vert.spv"),
                                                          .m_FragmentShaderModule = ShaderManager::LoadShader("resources/shaders/default.frag.spv")};

    m_DefaultGraphicPipeline = std::make_shared<VulkanGraphicPipeline>(pipelineSpec, swapChainData);
    m_DefaultDescriptorSet = std::make_shared<VulkanDescriptorSet>(m_DefaultGraphicPipeline->GetDescriptorSetLayout(), *m_VulkanContext.GetDescriptorPool());
  }

  // Creation of the grid pipeline
  {
    const std::vector<VulkanDescriptorSetLayoutSpec> gridPipelineLayoutSpec = {
        VulkanDescriptorSetLayoutSpec{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT}};
    Ref<VulkanDescriptorSetLayout> gridDescriptorSetLayout = std::make_shared<VulkanDescriptorSetLayout>(gridPipelineLayoutSpec);
    const VulkanGraphicPipelineSpecification gridPipelineSpec{.m_VulkanDescriptorSetLayout = gridDescriptorSetLayout,
                                                              .m_VertexShaderModule = ShaderManager::LoadShader("resources/shaders/grid.vert.spv"),
                                                              .m_FragmentShaderModule = ShaderManager::LoadShader("resources/shaders/grid.frag.spv")};

    m_GridGraphicPipeline = std::make_shared<VulkanGraphicPipeline>(gridPipelineSpec, swapChainData);
    m_GridDescriptorSet = std::make_shared<VulkanDescriptorSet>(m_GridGraphicPipeline->GetDescriptorSetLayout(), *m_VulkanContext.GetDescriptorPool());
  }

  // Uniform buffer
  m_UniformBuffer = std::make_shared<VulkanBuffer>(
      VulkanBufferSpec{sizeof(UniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VulkanBuffer::DEFAULT_MEMORY_PROPERTY_FLAGS});

  // Adding the quad model as default
  std::vector<Ref<Mesh>> quadMeshes = {Mesh::Create(QUAD_VERTICES, QUAD_INDICES)};
  Ref<Model> quadModel = std::make_shared<Model>(quadMeshes);
  m_RendererData.m_ModelsToDraw[quadModel->m_Id] = quadModel;
}

void VulkanRenderer::Update(Frame& _frame, const Scene& _scene, Camera& _camera, const Light& _light, float _deltaTime) {
  DODO_TRACE(VulkanRenderer);

  const RenderPass renderPass = BeginSceneRenderPass(_frame);
  const VkExtent2D frameSize = renderPass.m_FrameBuffer->GetDimensions();
  const VkCommandBuffer& commandBuffer = *_frame.m_CommandBuffer;

  _camera.UpdateProjectionMatrix({frameSize.width, frameSize.height});

  // Retrieve all entities with a light component : to do
  m_UniformMvp.m_LightPos = _light.m_Position;

  _scene.ForAll<MeshComponent, TransformComponent>().each([&](const MeshComponent& _meshComponent, const TransformComponent& _transformComponent) {
    const Ref<Model>& model = _meshComponent.m_Model;
    m_RendererData.m_ModelsTransform[model->m_Id].push_back(_transformComponent.m_TransformMatrix);

    std::for_each(std::begin(model->m_Meshes), std::end(model->m_Meshes),
                  [&](const Ref<Mesh>& _mesh) { m_RendererData.m_ModelsTexture[model->m_Id] = _mesh->m_Texture; });
  });

  // To refacto
  BatchMvpUbo(_camera, frameSize, _deltaTime);

  // Grid
  {
    m_GridGraphicPipeline->Bind(commandBuffer);
    m_GridDescriptorSet->UpdateUniformDescriptor(*m_UniformBuffer);
    m_GridDescriptorSet->Bind(m_GridGraphicPipeline->GetPipelineLayout(), commandBuffer);

    const Ref<Model>& quadModel = m_RendererData.m_ModelsToDraw[0];

    const VulkanBuffer& _vertexBuffer = *quadModel->m_Meshes[0]->m_VertexBuffer;
    VkBuffer vertexBuffers[] = {_vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    uint32_t elementCount = _vertexBuffer.GetElementCount();

    const VulkanBuffer& _indexBuffer = *quadModel->m_Meshes[0]->m_IndicesBuffer;
    vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(commandBuffer, _indexBuffer.GetElementCount(), 1, 0, 0, 0);
  }

  // Default shader
  {
    m_DefaultGraphicPipeline->Bind(commandBuffer);

    m_DefaultDescriptorSet->UpdateUniformDescriptor(*m_UniformBuffer);
    m_DefaultDescriptorSet->UpdateImageSamplers(m_RendererData.NeededTextures());
    m_DefaultDescriptorSet->Bind(m_DefaultGraphicPipeline->GetPipelineLayout(), commandBuffer);

    uint32_t modelId = 0;

    // Retrieve entities with meshes
    _scene.ForAll<MeshComponent>().each([&](const MeshComponent& _meshComponent) {
      const Ref<Model> model = _meshComponent.m_Model;
      const uint32_t instanceCount = m_RendererData.m_ModelsTransform[model->m_Id].size();

      std::for_each(std::begin(model->m_Meshes), std::end(model->m_Meshes), [&](const Ref<Mesh>& _mesh) {
        const Ref<Texture>& texture = _mesh->m_Texture;
        const Ptr<VulkanTextureImage>& textureImage = texture->GetTextureImage();

        RendererPushConstants rendererPushConstants{.m_ModelId = modelId, .m_TextureId = texture->GetId()};
        vkCmdPushConstants(commandBuffer, m_DefaultGraphicPipeline->GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(RendererPushConstants),
                           &rendererPushConstants);

        const VulkanBuffer& _vertexBuffer = *_mesh->m_VertexBuffer;
        VkBuffer vertexBuffers[] = {_vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        uint32_t elementCount = _vertexBuffer.GetElementCount();

        if (_mesh->IsIndexed()) {
          const VulkanBuffer& _indexBuffer = *_mesh->m_IndicesBuffer;
          vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
          vkCmdDrawIndexed(commandBuffer, _indexBuffer.GetElementCount(), instanceCount, 0, 0, 0);
        } else {
          vkCmdDraw(commandBuffer, elementCount, instanceCount, 0, 0);
        }
      });

      modelId += instanceCount;
    });
  }

  EndSceneRenderPass(renderPass);
}

void VulkanRenderer::Shutdown() {
  DODO_TRACE(VulkanRenderer);

  m_RendererData.Clear();
  m_UniformBuffer.reset();
  ShaderManager::Shutdown();
  TextureManager::Shutdown();
  m_GridGraphicPipeline.reset();
  m_DefaultGraphicPipeline.reset();
  m_VulkanContext.Shutdown();
}

RenderPass VulkanRenderer::BeginSceneRenderPass(Frame& _frame) {
  DODO_TRACE(VulkanRenderer);

  RenderPass renderPass{_frame};
  m_VulkanContext.BeginSceneRenderPass(renderPass);

  return renderPass;
}

void VulkanRenderer::EndSceneRenderPass(const RenderPass& _renderPass) {
  DODO_TRACE(VulkanRenderer);

  m_VulkanContext.EndSceneRenderPass(_renderPass);

  m_GridDescriptorSet->Reset();
  m_DefaultDescriptorSet->Reset();
  m_RendererData.ResetTransforms();
}

RenderPass VulkanRenderer::BeginUIRenderPass(Frame& _frame) {
  DODO_TRACE(VulkanRenderer);

  RenderPass renderPass{_frame};
  m_VulkanContext.BeginUIRenderPass(renderPass);

  return renderPass;
}

void VulkanRenderer::EndUIRenderPass(const RenderPass& _renderPass) {
  DODO_TRACE(VulkanRenderer);

  m_VulkanContext.EndUIRenderPass(_renderPass);
}

Frame VulkanRenderer::BeginFrame(const uint32_t& _frameNumber) {
  DODO_TRACE(VulkanRenderer);

  vkDeviceWaitIdle(*m_VulkanContext.GetVulkanDevice());

  const Ref<VulkanCommandBuffer> commandBuffer = m_VulkanContext.GetCommandBuffer(_frameNumber);
  commandBuffer->BeginRecording();

  return {.m_FrameNumber = _frameNumber, .m_CommandBuffer = commandBuffer};
}

void VulkanRenderer::EndFrame(const Frame& _frame) {
  DODO_TRACE(VulkanRenderer);

  Ref<VulkanCommandBuffer> commandBuffer = _frame.m_CommandBuffer;
  commandBuffer->EndRecording();

  if (_frame.m_Error) {
    return;
  }

  m_VulkanContext.SubmitQueue(_frame);
  m_VulkanContext.PresentQueue(_frame);
}

// TODO: refacto
std::vector<UniformBufferObject> VulkanRenderer::BatchMvpUbo(const Camera& _camera, const VkExtent2D& _frameDimensions, float _deltaTime) {
  DODO_TRACE(VulkanRenderer);

  std::vector<UniformBufferObject> batchUbo;
  const VkExtent2D extent = _frameDimensions;
  uint32_t transformIndex = 0;
  std::for_each(std::begin(m_RendererData.m_ModelsTransform), std::end(m_RendererData.m_ModelsTransform),
                [&](const std::pair<const ModelId, const std::vector<glm::mat4>>& _entry) {
                  for (const glm::mat4& transform : _entry.second) {
                    m_UniformMvp.m_ModelTransforms[transformIndex++] = transform;
                  }
                });

  m_UniformMvp.m_View = _camera.GetViewMatrix();
  m_UniformMvp.m_Proj = _camera.GetProjectionMatrix();

  batchUbo.push_back(m_UniformMvp);

  m_UniformBuffer->SetMemory(&m_UniformMvp, sizeof(UniformBufferObject));

  return batchUbo;
}

DODO_END_NAMESPACE
