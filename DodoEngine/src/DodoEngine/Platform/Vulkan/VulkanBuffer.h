#pragma once

#include <DodoEngine/Core/Types.h>

#include <volk.h>


DODO_BEGIN_NAMESPACE

class VulkanDevice;
class VulkanPhysicalDevice;

struct VulkanBufferSpec
{
	VkDeviceSize m_AllocationSize;
	VkBufferCreateFlags m_Usage;
	VkMemoryPropertyFlags m_Properties;
};

class VulkanBuffer
{
public:
	VulkanBuffer(const VulkanBufferSpec& _vulkanBufferSpec, const Ref<VulkanDevice>& _vulkanDevice, const VulkanPhysicalDevice& _vulkanPhysicalDevice);

	~VulkanBuffer();

	uint64_t Size() const { return m_Size; }

    void SetMemory(const void* _data, VkDeviceSize _allocationSize) const;

	operator const VkBuffer& () const { return m_Buffer; }

private:
	VkBuffer m_Buffer;
	VkDeviceMemory m_DeviceMemory;
	VkDeviceSize m_Size;
	Ref<VulkanDevice> m_VulkanDevice;
};

DODO_END_NAMESPACE