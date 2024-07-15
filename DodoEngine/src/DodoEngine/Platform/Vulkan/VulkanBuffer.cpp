#include <DodoEngine/Platform/Vulkan/VulkanBuffer.h>

#include <DodoEngine/Platform/Vulkan/VulkanCommandBuffer.h>
#include <DodoEngine/Platform/Vulkan/VulkanDevice.h>
#include <DodoEngine/Platform/Vulkan/VulkanPhysicalDevice.h>
#include <DodoEngine/Utils/Log.h>

DODO_BEGIN_NAMESPACE

VulkanBuffer::VulkanBuffer(const VulkanBufferSpec& _vulkanBufferSpec) : m_ElementCount(0) {

  const VulkanContext& vulkanContext = VulkanContext::Get();
  const Ref<VulkanDevice>& vulkanDevice = vulkanContext.GetVulkanDevice();
  const Ref<VulkanPhysicalDevice>& vulkanPhysicalDevice = vulkanContext.GetVulkanPhysicalDevice();
  const VmaAllocator& allocator = vulkanContext.GetAllocator();
  const VkDeviceSize& allocationSize = _vulkanBufferSpec.m_AllocationSize;

  VmaAllocationCreateInfo vmaAllocInfo = {};
  vmaAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
  vmaAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

  VkBufferCreateInfo bufferCreateInfo{};
  bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferCreateInfo.size = _vulkanBufferSpec.m_AllocationSize;
  bufferCreateInfo.usage = _vulkanBufferSpec.m_Usage;
  bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  const VkResult bufferCreateResult = vmaCreateBuffer(allocator, &bufferCreateInfo, &vmaAllocInfo, &m_Buffer, &m_Allocation, nullptr);
  vmaSetAllocationName(allocator, m_Allocation, std::format("Vertex Buffer", _vulkanBufferSpec.m_AllocationSize).c_str());
  if (bufferCreateResult != VK_SUCCESS) {
    DODO_CRITICAL("Could not create the vertex buffer");
  }

  m_Size = allocationSize;
}

VulkanBuffer::~VulkanBuffer() {
  const VulkanContext& vulkanContext = VulkanContext::Get();
  const Ref<VulkanDevice>& vulkanDevice = vulkanContext.GetVulkanDevice();
  const VmaAllocator& allocator = vulkanContext.GetAllocator();

  vkDeviceWaitIdle(*vulkanDevice);

  vmaDestroyBuffer(allocator, m_Buffer, m_Allocation);
}

void VulkanBuffer::CopyBuffer(VkBuffer _srcBuffer, VkBuffer _dstBuffer, VkDeviceSize _size) {
  VulkanContext vulkanContext = VulkanContext::Get();
  VulkanCommandBuffer commandBuffer;
  commandBuffer.BeginRecording();

  VkBufferCopy copyRegion{};
  copyRegion.size = _size;
  vkCmdCopyBuffer(commandBuffer, _srcBuffer, _dstBuffer, 1, &copyRegion);

  commandBuffer.EndRecording();
}

DODO_END_NAMESPACE
