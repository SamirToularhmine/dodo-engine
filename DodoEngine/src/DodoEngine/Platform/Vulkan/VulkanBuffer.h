#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Platform/Vulkan/VulkanContext.h>

#define VK_NO_PROTOTYPES
#include <volk.h>
#include <vk_mem_alloc.h>

DODO_BEGIN_NAMESPACE

class VulkanDevice;
class VulkanPhysicalDevice;

struct VulkanBufferSpec {
  VkDeviceSize m_AllocationSize;
  VkBufferCreateFlags m_Usage;
  VkMemoryPropertyFlags m_Properties;
};

class VulkanBuffer {
 public:
  static constexpr VkMemoryPropertyFlags DEFAULT_MEMORY_PROPERTY_FLAGS = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

  VulkanBuffer(const VulkanBufferSpec& _vulkanBufferSpec);

  ~VulkanBuffer();

  uint64_t Size() const { return m_Size; }

  template <typename T>
  void SetMemory(const T* _data, VkDeviceSize _allocationSize) {
    const VulkanContext& vulkanContext = VulkanContext::Get();
    const VmaAllocator& allocator = vulkanContext.GetAllocator();

    m_ElementCount = _allocationSize / sizeof(T);
    void* mappedData;
    vmaMapMemory(allocator, m_Allocation, &mappedData);
    memcpy(mappedData, _data, _allocationSize);
    vmaUnmapMemory(allocator, m_Allocation);
  }

  const uint32_t GetElementCount() const { return m_ElementCount; };

  operator const VkBuffer&() const { return m_Buffer; }

 public:
  static void CopyBuffer(VkBuffer _srcBuffer, VkBuffer _dstBuffer, VkDeviceSize _size);

 private:
  VkBuffer m_Buffer;
  VmaAllocation m_Allocation;
  VkDeviceSize m_Size;
  uint32_t m_ElementCount;
};

DODO_END_NAMESPACE