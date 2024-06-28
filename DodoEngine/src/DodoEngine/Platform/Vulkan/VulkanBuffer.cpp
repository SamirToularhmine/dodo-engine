#include <DodoEngine/Platform/Vulkan/VulkanBuffer.h>

#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Utils/Log.h>


DODO_BEGIN_NAMESPACE

VulkanBuffer::VulkanBuffer(const void* _data, VkDeviceSize _allocationSize, VkBufferCreateFlags _usage, VkMemoryPropertyFlags _properties)
    : m_Data(_data)
{
	VulkanContext vulkanContext = VulkanContext::Get();
	const VulkanDevice& vulkanDevice = *vulkanContext.GetVulkanDevice();

	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = _allocationSize;
	bufferCreateInfo.usage = _usage;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	const VkResult bufferCreateResult = vkCreateBuffer(vulkanDevice, &bufferCreateInfo, nullptr, &m_Buffer);
	if (bufferCreateResult != VK_SUCCESS)
	{
		DODO_CRITICAL("Could not create Quad buffer");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(vulkanDevice, m_Buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = vulkanContext.GetVulkanPhysicalDevice()->FindMemoryType(
		memRequirements.memoryTypeBits, _properties
	);

	const VkResult allocateMemoryResult = vkAllocateMemory(vulkanDevice, &allocInfo, nullptr, &m_DeviceMemory);
	if (allocateMemoryResult != VK_SUCCESS)
	{
		DODO_CRITICAL("Could not allocate memory for buffer");
	}

    SetMemory(_data, _allocationSize);

	m_Size = _allocationSize;
}

VulkanBuffer::~VulkanBuffer()
{
	VulkanContext& vulkanContext = VulkanContext::Get();
	const VulkanDevice& vulkanDevice = *vulkanContext.GetVulkanDevice();

	vkDestroyBuffer(vulkanDevice, m_Buffer, nullptr);
	vkFreeMemory(vulkanDevice, m_DeviceMemory, nullptr);
}

void VulkanBuffer::SetMemory(const void*_data, VkDeviceSize _allocationSize) {
    VulkanContext vulkanContext = VulkanContext::Get();
    const VulkanDevice& vulkanDevice = *vulkanContext.GetVulkanDevice();

    vkBindBufferMemory(vulkanDevice, m_Buffer, m_DeviceMemory, 0);

    void* data;
    vkMapMemory(vulkanDevice, m_DeviceMemory, 0, _allocationSize, 0, &data);
    memcpy(data, _data, _allocationSize);
    vkUnmapMemory(vulkanDevice, m_DeviceMemory);
}

DODO_END_NAMESPACE
