#include <DodoEngine/Platform/Vulkan/VulkanCommandBuffer.h>

#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Platform/Vulkan/VulkanDevice.h>

DODO_BEGIN_NAMESPACE

VulkanCommandBuffer::VulkanCommandBuffer() : m_SubmittedQueue(VK_NULL_HANDLE) {
  const VulkanContext& vulkanContext = VulkanContext::Get();
  const Ref<VulkanDevice>& vulkanDevice = vulkanContext.GetVulkanDevice();
  const VkCommandPool& commandPool = vulkanContext.GetCommandPool();

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = commandPool;
  allocInfo.commandBufferCount = 1;

  vkAllocateCommandBuffers(*vulkanDevice, &allocInfo, &m_VkCommandBuffer);
}

VulkanCommandBuffer::~VulkanCommandBuffer() {
  const VulkanContext& vulkanContext = VulkanContext::Get();
  const Ref<VulkanDevice>& vulkanDevice = vulkanContext.GetVulkanDevice();
  const VkCommandPool& commandPool = vulkanContext.GetCommandPool();

  if (m_SubmittedQueue != VK_NULL_HANDLE)
  {
    vkQueueWaitIdle(m_SubmittedQueue);
  }

  vkFreeCommandBuffers(*vulkanDevice, commandPool, 1, &m_VkCommandBuffer);
}

void VulkanCommandBuffer::BeginRecording() const {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(m_VkCommandBuffer, &beginInfo);
}

void VulkanCommandBuffer::EndRecording() const {
  vkEndCommandBuffer(m_VkCommandBuffer);
}

void VulkanCommandBuffer::Submit(const VkQueue& _queue) {
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &m_VkCommandBuffer;

  vkQueueSubmit(_queue, 1, &submitInfo, VK_NULL_HANDLE);

  m_SubmittedQueue = _queue;
}

DODO_END_NAMESPACE
