#include <DodoEngine/Platform/Vulkan/VulkanDescriptorSetLayout.h>

#include <DodoEngine/Platform/Vulkan/VulkanDevice.h>

#include <DodoEngine/Utils/Log.h>


DODO_BEGIN_NAMESPACE

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(const Ref<VulkanDevice>& _vulkanDevice)
    : m_VulkanDevice(_vulkanDevice)
{
    // TODO: Temporary will refacto later
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    VkResult descriptorSetLayoutCreateResult = vkCreateDescriptorSetLayout(*_vulkanDevice, &layoutInfo, nullptr, &m_VkDescriptorSetLayout);
    if(descriptorSetLayoutCreateResult != VK_SUCCESS)
    {
        DODO_CRITICAL("Could not create descriptor set layout");
    }
}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
{
    vkDestroyDescriptorSetLayout(*m_VulkanDevice, m_VkDescriptorSetLayout, nullptr);
}

DODO_END_NAMESPACE