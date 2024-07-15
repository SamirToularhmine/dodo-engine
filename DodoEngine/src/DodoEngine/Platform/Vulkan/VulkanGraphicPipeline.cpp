#include <DodoEngine/Platform/Vulkan/VulkanGraphicPipeline.h>

#include <DodoEngine/Platform/Vulkan/VulkanDescriptorSetLayout.h>
#include <DodoEngine/Platform/Vulkan/VulkanDevice.h>
#include <DodoEngine/Platform/Vulkan/VulkanRenderPass.h>
#include <DodoEngine/Platform/Vulkan/VulkanSwapChain.h>
#include <DodoEngine/Renderer/RendererPushConstants.h>
#include <DodoEngine/Renderer/RendererSpecializationData.h>
#include <DodoEngine/Renderer/Vertex.h>
#include <DodoEngine/Utils/Log.h>
#include <DodoEngine/Utils/Utils.h>

#include <vector>

DODO_BEGIN_NAMESPACE

VulkanGraphicPipeline::VulkanGraphicPipeline(Ref<VulkanDevice> _vulkanDevice, VkDescriptorSetLayout& _vkDescriptorSetLayout,
                                             const VulkanSwapChainData& _swapChainData, const VulkanRenderPass& _vulkanRenderPass)
    : m_VulkanDevice(_vulkanDevice) {
  // Loading shader
  std::vector<char> vertexShaderFile = readFile("resources/shaders/vert.spv");
  std::vector<char> fragmentShaderFile = readFile("resources/shaders/frag.spv");

  // Vertex Shader
  VkShaderModuleCreateInfo vertexShaderModuleCreateInfo{};
  vertexShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  vertexShaderModuleCreateInfo.codeSize = vertexShaderFile.size();
  vertexShaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(vertexShaderFile.data());

  // Fragment Shader
  VkShaderModuleCreateInfo fragmentShaderModuleCreateInfo{};
  fragmentShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  fragmentShaderModuleCreateInfo.codeSize = fragmentShaderFile.size();
  fragmentShaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(fragmentShaderFile.data());

  VkResult vertexShaderModuleCreateResult = vkCreateShaderModule(*_vulkanDevice, &vertexShaderModuleCreateInfo, nullptr, &m_VertexShaderModule);
  VkResult fragmentShaderModuleCreateResult = vkCreateShaderModule(*_vulkanDevice, &fragmentShaderModuleCreateInfo, nullptr, &m_FragmentShaderModule);

  if (vertexShaderModuleCreateResult != VK_SUCCESS || fragmentShaderModuleCreateResult != VK_SUCCESS) {
    DODO_CRITICAL("Could not create shader modules");
  }

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
  rasterizerCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizerCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizerCreateInfo.depthBiasEnable = VK_FALSE;

  VkPipelineMultisampleStateCreateInfo multisampleCreateInfo{};
  multisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampleCreateInfo.sampleShadingEnable = VK_FALSE;
  multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_FALSE;

  VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
  colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
  colorBlendStateCreateInfo.attachmentCount = 1;
  colorBlendStateCreateInfo.pAttachments = &colorBlendAttachment;

  VkPushConstantRange pushConstantCreateInfo;
  pushConstantCreateInfo.offset = 0;
  pushConstantCreateInfo.size = sizeof(RendererPushConstants);
  pushConstantCreateInfo.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
  pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutCreateInfo.setLayoutCount = 1;
  pipelineLayoutCreateInfo.pSetLayouts = &_vkDescriptorSetLayout;
  pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
  pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantCreateInfo;

  VkResult pipeLayoutCreateResult = vkCreatePipelineLayout(*_vulkanDevice, &pipelineLayoutCreateInfo, nullptr, &m_VkPipelineLayout);
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
  graphicsPipelineCreateInfo.layout = m_VkPipelineLayout;
  graphicsPipelineCreateInfo.renderPass = _vulkanRenderPass;
  graphicsPipelineCreateInfo.subpass = 0;
  graphicsPipelineCreateInfo.pDepthStencilState = &depthStencil;

  VkResult graphicsPipelineCreateResult = vkCreateGraphicsPipelines(*_vulkanDevice, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &m_VkPipeline);
  if (graphicsPipelineCreateResult != VK_SUCCESS) {
    DODO_CRITICAL("Could not create graphics pipeline");
  }
}

VulkanGraphicPipeline::~VulkanGraphicPipeline() {
  vkDestroyShaderModule(*m_VulkanDevice, m_VertexShaderModule, nullptr);
  vkDestroyShaderModule(*m_VulkanDevice, m_FragmentShaderModule, nullptr);
  vkDestroyPipelineLayout(*m_VulkanDevice, m_VkPipelineLayout, nullptr);
  vkDestroyPipeline(*m_VulkanDevice, m_VkPipeline, nullptr);
}

void VulkanGraphicPipeline::Bind(const VkCommandBuffer& _vkCommandBuffer) const {
  vkCmdBindPipeline(_vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_VkPipeline);

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
