#include <DodoEngine/Platform/Vulkan/VulkanRenderer.h>

#include <DodoEngine/Core/Camera.h>
#include <DodoEngine/Core/Window.h>
#include <DodoEngine/Platform/Vulkan/VulkanBuffer.h>
#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Platform/Vulkan/VulkanDescriptorSet.h>
#include <DodoEngine/Platform/Vulkan/VulkanDescriptorSetLayout.h>
#include <DodoEngine/Platform/Vulkan/VulkanGraphicPipeline.h>
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
        VulkanDescriptorSetLayoutSpec{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT}
    };
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
}

void VulkanRenderer::Update(const Camera& _camera, const Light& _light, float _deltaTime) {
  DODO_TRACE(VulkanRenderer);

  VulkanRenderPassData& renderPassData = m_RendererData.m_RenderPassData;
  const VkCommandBuffer& commandBuffer = renderPassData.m_CommandBuffer;
  const uint32_t& frameIndex = renderPassData.m_FrameIndex;

  m_UniformMvp.m_LightPos = _light.m_Position;
  BatchMvpUbo(_camera, renderPassData.m_SwapChainData, _deltaTime);

  // Grid pass
  {
    m_GridGraphicPipeline->Bind(commandBuffer);
    m_GridDescriptorSet->UpdateUniformDescriptor(*m_UniformBuffer, frameIndex);
    m_GridDescriptorSet->Bind(m_GridGraphicPipeline->GetPipelineLayout(), commandBuffer, frameIndex);

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

  // Default shader pass
  {
    m_DefaultGraphicPipeline->Bind(commandBuffer);

    m_DefaultDescriptorSet->UpdateUniformDescriptor(*m_UniformBuffer, frameIndex);
    m_DefaultDescriptorSet->UpdateImageSamplers(m_RendererData.NeededTextures(), frameIndex);
    m_DefaultDescriptorSet->Bind(m_DefaultGraphicPipeline->GetPipelineLayout(), commandBuffer, frameIndex);

    uint32_t modelId = 0;

    // Draw Meshes
    for (const auto& [id, model] : m_RendererData.m_ModelsToDraw) {
      const uint32_t instanceCount = m_RendererData.m_ModelsTransform[id].size();

      std::for_each(std::begin(model->m_Meshes), std::end(model->m_Meshes), [&](const Ref<Mesh>& _mesh) {
        const Ref<Texture>& texture = _mesh->m_Texture;
        const Ptr<VulkanTextureImage>& textureImage = texture->GetTextureImage();

        RendererPushConstants rendererPushConstants{.m_ModelId = modelId, .m_TextureId = texture->GetId()};
        vkCmdPushConstants(commandBuffer, renderPassData.m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(RendererPushConstants),
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
    }
  }
}

void VulkanRenderer::Shutdown() {
  m_RendererData.Clear();
  m_UniformBuffer.reset();
  ShaderManager::Shutdown();
  TextureManager::Shutdown();
  m_VulkanContext.Shutdown();
}

void VulkanRenderer::BeginRenderPass() {
  DODO_TRACE(VulkanRenderer);

  m_RendererData.m_RenderPassData = {m_FrameCount++};
  m_VulkanContext.BeginRenderPass(m_RendererData.m_RenderPassData);
  m_RendererData.m_RenderPassData.m_PipelineLayout = m_DefaultGraphicPipeline->GetPipelineLayout();
}

void VulkanRenderer::EndRenderPass() {
  DODO_TRACE(VulkanRenderer);
  
  m_VulkanContext.EndRenderPass(m_RendererData.m_RenderPassData);

  m_GridDescriptorSet->Reset();
  m_DefaultDescriptorSet->Reset();
  m_RendererData.ResetTransforms();
}

void VulkanRenderer::BeginUIRenderPass() {
  DODO_TRACE(VulkanRenderer);
  
  m_VulkanContext.BeginUIRenderPass(m_RendererData.m_RenderPassData);
}

void VulkanRenderer::EndUIRenderPass() {
  DODO_TRACE(VulkanRenderer);

  m_VulkanContext.EndUIRenderPass(m_RendererData.m_RenderPassData);
}

void VulkanRenderer::RegisterModel(Ref<Model>& _model) {
  m_RendererData.m_ModelsToDraw[_model->m_Id] = _model;

  for (const auto& _mesh : _model->m_Meshes) {
    Ref<Texture> meshTexture = _mesh->m_Texture;
    const TextureId& textureId = meshTexture->GetId();
    if (!m_RendererData.m_ModelsTexture.contains(textureId)) {
      m_RendererData.m_ModelsTexture[textureId] = meshTexture;
    }
  }
}

void VulkanRenderer::DrawQuad(const MeshTransform& _meshTransform) {
  m_RendererData.m_ModelsTransform[0].push_back(_meshTransform.GetModelMatrix());
}

void VulkanRenderer::DrawCube(const MeshTransform& _meshTransform) {
  m_RendererData.m_ModelsTransform[1].push_back(_meshTransform.GetModelMatrix());
}

void VulkanRenderer::DrawModel(Ref<Model>& _model, const MeshTransform& _meshTransform) {
  const glm::mat4 modelMatrix = _meshTransform.GetModelMatrix();
  const ModelId modelId = _model->m_Id;
  if (!m_RendererData.m_ModelsToDraw.contains(modelId)) {
    RegisterModel(_model);
  }

  m_RendererData.m_ModelsTransform[modelId].push_back(modelMatrix);
}

uint32_t VulkanRenderer::GetFrameIndex() const {
  return m_RendererData.m_RenderPassData.m_FrameIndex;
}

std::vector<UniformBufferObject> VulkanRenderer::BatchMvpUbo(const Camera& _camera, const VulkanSwapChainData& _vulkanSwapChainData, float _deltaTime) {
  DODO_TRACE(VulkanRenderer);

  std::vector<UniformBufferObject> batchUbo;
  const VkExtent2D extent = _vulkanSwapChainData.m_VkExtent;
  uint32_t lastTransform = 0;
  for (const auto& [modelId, transforms] : m_RendererData.m_ModelsTransform) {
    for (int i = 0; i < transforms.size(); ++i) {
      m_UniformMvp.m_ModelTransforms[lastTransform++] = transforms[i];
    }
  }

  m_UniformMvp.m_View = _camera.GetViewMatrix();
  m_UniformMvp.m_Proj = _camera.GetProjectionMatrix({extent.width, extent.height});

  batchUbo.push_back(m_UniformMvp);

  m_UniformBuffer->SetMemory(&m_UniformMvp, sizeof(UniformBufferObject));

  return batchUbo;
}

DODO_END_NAMESPACE
