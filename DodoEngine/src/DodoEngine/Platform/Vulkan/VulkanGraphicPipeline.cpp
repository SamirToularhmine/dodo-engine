#include <DodoEngine/Platform/Vulkan/VulkanGraphicPipeline.h>

#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Platform/Vulkan/VulkanDescriptorSetLayout.h>
#include <DodoEngine/Platform/Vulkan/VulkanDevice.h>
#include <DodoEngine/Platform/Vulkan/VulkanRenderPass.h>
#include <DodoEngine/Platform/Vulkan/VulkanSwapChain.h>
#include <DodoEngine/Renderer/RendererPushConstants.h>
#include <DodoEngine/Renderer/RendererSpecializationData.h>
#include <DodoEngine/Renderer/ShaderManager.h>
#include <DodoEngine/Renderer/Vertex.h>
#include <DodoEngine/Utils/Log.h>
#include <DodoEngine/Utils/Utils.h>

DODO_BEGIN_NAMESPACE

VulkanGraphicPipeline::VulkanGraphicPipeline(const VulkanGraphicPipelineSpecification& _vulkanPipelineSpec, const VulkanSwapChainData& _swapChainData)
    : m_DescriptorSetLayout(std::move(_vulkanPipelineSpec.m_VulkanDescriptorSetLayout)) {
  const VkDevice& vkDevice = *VulkanContext::Get().GetVulkanDevice();
  const Ref<VulkanRenderPass>& vulkanRenderPass = VulkanContext::Get().GetSceneRenderPass();

  // Loading default shader
  m_VertexShaderModule = _vulkanPipelineSpec.m_VertexShaderModule;
  m_FragmentShaderModule = _vulkanPipelineSpec.m_FragmentShaderModule;

  RendererSpecializationData rendererSpecializationData{};
  std::vector<VkSpecializationMapEntry> rendererSpecializationMapEntries = rendererSpecializationData.GetMapEntries();

  VkSpecializationInfo vertexShaderStageSpecializationInfo{};
  vertexShaderStageSpecializationInfo.mapEntryCount = 1;
  vertexShaderStageSpecializationInfo.pMapEntries = rendererSpecializationMapEntries.data();
  vertexShaderStageSpecializationInfo.dataSize = sizeof(RendererSpecializationData);
  vertexShaderStageSpecializationInfo.pData = &rendererSpecializationData;

  VkPipelineShaderStageCreateInfo vertexShaderStageCreateInfo{};
  vertexShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertexShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertexShaderStageCreateInfo.module = m_VertexShaderModule;
  vertexShaderStageCreateInfo.pName = "main";
  vertexShaderStageCreateInfo.pSpecializationInfo = &vertexShaderStageSpecializationInfo;

  VkPipelineShaderStageCreateInfo fragmentShaderStageCreateInfo{};
  fragmentShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragmentShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragmentShaderStageCreateInfo.module = m_FragmentShaderModule;
  fragmentShaderStageCreateInfo.pName = "main";

  VkPipelineShaderStageCreateInfo shaderStageCreateInfo[] = {vertexShaderStageCreateInfo, fragmentShaderStageCreateInfo};

  std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

  VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
  dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicStateCreateInfo.dynamicStateCount = dynamicStates.size();
  dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

  // Description des Vertex dans la pipeline
  auto vertexBindingDescription = Vertex::GetBindingDescription();
  auto vertexAttributeDescription = Vertex::GetAttributeDescriptions();

  VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
  vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
  vertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexBindingDescription;
  vertexInputStateCreateInfo.vertexAttributeDescriptionCount = vertexAttributeDescription.size();
  vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexAttributeDescription.data();

  VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
  inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

  m_Viewport.x = 0.0f;
  m_Viewport.y = 0.0f;
  m_Viewport.width = _swapChainData.m_VkExtent.width;
  m_Viewport.height = _swapChainData.m_VkExtent.height;
  m_Viewport.minDepth = 0.0f;
  m_Viewport.maxDepth = 1.0f;

  m_Scissor.offset = {0, 0};
  m_Scissor.extent = _swapChainData.m_VkExtent;

  VkPipelineViewportStateCreateInfo viewportStateCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
      .viewportCount = 1,
      .pViewports = &m_Viewport,
      .scissorCount = 1,
      .pScissors = &m_Scissor,
  };

  VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo{};
  rasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizerCreateInfo.depthClampEnable = VK_FALSE;
  rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
  rasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizerCreateInfo.lineWidth = 1.0f;
  rasterizerCreateInfo.cullMode = VK_CULL_MODE_NONE;
  rasterizerCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizerCreateInfo.depthBiasEnable = VK_FALSE;

  VkPipelineMultisampleStateCreateInfo multisampleCreateInfo{};
  multisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampleCreateInfo.sampleShadingEnable = VK_FALSE;
  multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_TRUE;
  colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

  VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
  colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
  colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
  colorBlendStateCreateInfo.attachmentCount = 1;
  colorBlendStateCreateInfo.pAttachments = &colorBlendAttachment;
  colorBlendStateCreateInfo.blendConstants[0] = 0.0f;  // Optional
  colorBlendStateCreateInfo.blendConstants[1] = 0.0f;  // Optional
  colorBlendStateCreateInfo.blendConstants[2] = 0.0f;  // Optional
  colorBlendStateCreateInfo.blendConstants[3] = 0.0f;  // Optional

  VkPushConstantRange pushConstantCreateInfo;
  pushConstantCreateInfo.offset = 0;
  pushConstantCreateInfo.size = sizeof(RendererPushConstants);
  pushConstantCreateInfo.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkDescriptorSetLayout descriptorSetLayouts[] = {*m_DescriptorSetLayout};
  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
  pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutCreateInfo.setLayoutCount = 1;
  pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts;
  pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
  pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantCreateInfo;

  VkResult pipeLayoutCreateResult = vkCreatePipelineLayout(vkDevice, &pipelineLayoutCreateInfo, nullptr, &m_PipelineLayout);
  if (pipeLayoutCreateResult != VK_SUCCESS) {
    DODO_CRITICAL("Could not create pipeline layout");
  }

  VkPipelineDepthStencilStateCreateInfo depthStencil{};
  depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencil.depthTestEnable = VK_TRUE;
  depthStencil.depthWriteEnable = VK_TRUE;
  depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
  depthStencil.depthBoundsTestEnable = VK_FALSE;
  depthStencil.stencilTestEnable = VK_FALSE;
  depthStencil.minDepthBounds = 0.0f;
  depthStencil.maxDepthBounds = 1.0f;

  VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
  graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  graphicsPipelineCreateInfo.stageCount = 2;
  graphicsPipelineCreateInfo.pStages = shaderStageCreateInfo;
  graphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
  graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
  graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
  graphicsPipelineCreateInfo.pRasterizationState = &rasterizerCreateInfo;
  graphicsPipelineCreateInfo.pMultisampleState = &multisampleCreateInfo;
  graphicsPipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
  graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
  graphicsPipelineCreateInfo.layout = m_PipelineLayout;
  graphicsPipelineCreateInfo.renderPass = *vulkanRenderPass;
  graphicsPipelineCreateInfo.subpass = 0;
  graphicsPipelineCreateInfo.pDepthStencilState = &depthStencil;

  VkResult graphicsPipelineCreateResult = vkCreateGraphicsPipelines(vkDevice, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &m_Pipeline);
  if (graphicsPipelineCreateResult != VK_SUCCESS) {
    DODO_CRITICAL("Could not create graphics pipeline");
  }

  m_DescriptorSetLayouts.resize(VulkanContext::MAX_FRAMES_IN_FLIGHT, *m_DescriptorSetLayout);
}

VulkanGraphicPipeline::~VulkanGraphicPipeline() {
  const VkDevice& vkDevice = *VulkanContext::Get().GetVulkanDevice();

  vkDestroyPipelineLayout(vkDevice, m_PipelineLayout, nullptr);
  vkDestroyPipeline(vkDevice, m_Pipeline, nullptr);
}

void VulkanGraphicPipeline::Bind(const VkCommandBuffer& _vkCommandBuffer) const {
  vkCmdBindPipeline(_vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);

  vkCmdSetViewport(_vkCommandBuffer, 0, 1, &m_Viewport);
  vkCmdSetScissor(_vkCommandBuffer, 0, 1, &m_Scissor);
}

void VulkanGraphicPipeline::SetViewPort(const VulkanSwapChainData& swapChainSpec) {
  const VkExtent2D extent = swapChainSpec.m_VkExtent;
  m_Viewport.width = extent.width;
  m_Viewport.height = extent.height;
  m_Scissor.extent = extent;
}

DODO_END_NAMESPACE
