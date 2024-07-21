#include <DodoEngine/Platform/Vulkan/VulkanDescriptorSetLayout.h>

#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Platform/Vulkan/VulkanDevice.h>
#include <DodoEngine/Utils/Log.h>

DODO_BEGIN_NAMESPACE

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(const std::vector<VulkanDescriptorSetLayoutSpec>& _vulkanDescriptorLayoutSpecs) {
  const VkDevice& vkDevice = *VulkanContext::Get().GetVulkanDevice();
  std::vector<VkDescriptorSetLayoutBinding> vkDescriptorSetLayoutBindings(_vulkanDescriptorLayoutSpecs.size());

  for (uint32_t i = 0; i < _vulkanDescriptorLayoutSpecs.size(); ++i) {
    const VulkanDescriptorSetLayoutSpec& layoutSpec = _vulkanDescriptorLayoutSpecs[i];
    VkDescriptorSetLayoutBinding& layoutBinding = vkDescriptorSetLayoutBindings[i];

    // TODO: Temporary will refacto later
    layoutBinding.binding = i;
    layoutBinding.descriptorType = layoutSpec.m_vkDescriptorType;
    layoutBinding.descriptorCount = layoutSpec.m_DescriptorCount;
    layoutBinding.stageFlags = layoutSpec.m_ShaderStage;
    layoutBinding.pImmutableSamplers = nullptr;
  }

  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = vkDescriptorSetLayoutBindings.size();
  layoutInfo.pBindings = vkDescriptorSetLayoutBindings.data();

  VkResult descriptorSetLayoutCreateResult = vkCreateDescriptorSetLayout(vkDevice, &layoutInfo, nullptr, &m_VkDescriptorSetLayout);
  if (descriptorSetLayoutCreateResult != VK_SUCCESS) {
    DODO_CRITICAL("Could not create descriptor set layout");
  }
}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout() {
  const VkDevice& vkDevice = *VulkanContext::Get().GetVulkanDevice();

  vkDestroyDescriptorSetLayout(vkDevice, m_VkDescriptorSetLayout, nullptr);
}

DODO_END_NAMESPACE