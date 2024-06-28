#pragma once

#include <DodoEngine/Core/Types.h>

#include <volk.h>


DODO_BEGIN_NAMESPACE

class VulkanBuffer
{
public:
	VulkanBuffer(const void* _data, VkDeviceSize allocationSize, VkBufferCreateFlags usage, VkMemoryPropertyFlags properties);

	~VulkanBuffer();

	uint32_t Size() const { return m_Size; }

    void SetMemory(const void* _data, VkDeviceSize _allocationSize);

	operator const VkBuffer& () const { return m_Buffer; }

private:
	VkBuffer m_Buffer;
	VkDeviceMemory m_DeviceMemory;
	VkDeviceSize m_Size;
    const void* m_Data;
};

DODO_END_NAMESPACE