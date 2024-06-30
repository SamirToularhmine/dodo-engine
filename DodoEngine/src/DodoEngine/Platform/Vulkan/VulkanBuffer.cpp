#include <DodoEngine/Platform/Vulkan/VulkanBuffer.h>

#include <DodoEngine/Platform/Vulkan/VulkanDevice.h>
#include <DodoEngine/Platform/Vulkan/VulkanPhysicalDevice.h>
#include <DodoEngine/Utils/Log.h>


DODO_BEGIN_NAMESPACE

VulkanBuffer::VulkanBuffer(const VulkanBufferSpec& _vulkanBufferSpec, const Ref<VulkanDevice>& _vulkanDevice, const VulkanPhysicalDevice& _vulkanPhysicalDevice)
	: m_VulkanDevice(_vulkanDevice)
{
	const VulkanDevice& vulkanDevice = *m_VulkanDevice;
	const VkDeviceSize& allocationSize = _vulkanBufferSpec.m_AllocationSize;

	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = _vulkanBufferSpec.m_AllocationSize;
	bufferCreateInfo.usage = _vulkanBufferSpec.m_Usage;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	const VkResult bufferCreateResult = vkCreateBuffer(vulkanDevice, &bufferCreateInfo, nullptr, &m_Buffer);
	if (bufferCreateResult != VK_SUCCESS)
	{
		DODO_CRITICAL("Could not create the vertex buffer");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(vulkanDevice, m_Buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = _vulkanPhysicalDevice.FindMemoryType(
		memRequirements.memoryTypeBits, _vulkanBufferSpec.m_Properties
	);

	const VkResult allocateMemoryResult = vkAllocateMemory(vulkanDevice, &allocInfo, nullptr, &m_DeviceMemory);
	if (allocateMemoryResult != VK_SUCCESS)
	{
		DODO_CRITICAL("Could not allocate memory for buffer");
	}

	const VkResult bindBufferMemoryResult = vkBindBufferMemory(vulkanDevice, m_Buffer, m_DeviceMemory, 0);
	if (bindBufferMemoryResult != VK_SUCCESS)
	{
		DODO_CRITICAL("Could not bind buffer memory");
	}

	m_Size = allocationSize;
}

VulkanBuffer::~VulkanBuffer()
{
	const VulkanDevice& vulkanDevice = *m_VulkanDevice;

	vkDestroyBuffer(vulkanDevice, m_Buffer, nullptr);
	vkFreeMemory(vulkanDevice, m_DeviceMemory, nullptr);
}

void VulkanBuffer::SetMemory(const void*_data, VkDeviceSize _allocationSize) const {
	const VulkanDevice& vulkanDevice = *m_VulkanDevice;

    void* data;
	const VkResult mapMemoryResult = vkMapMemory(vulkanDevice, m_DeviceMemory, 0, _allocationSize, 0, &data);
	if(mapMemoryResult != VK_SUCCESS)
	{
		DODO_CRITICAL("Could not map memory");
	}

    memcpy(data, _data, _allocationSize);
    vkUnmapMemory(vulkanDevice, m_DeviceMemory);
}

DODO_END_NAMESPACE
