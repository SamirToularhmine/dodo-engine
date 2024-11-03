#pragma once

#include <DodoEngine/Core/Types.h>

#include <volk.h>

DODO_BEGIN_NAMESPACE

class VulkanDevice;

class VulkanCommandBuffer {
 public:
  VulkanCommandBuffer();
  ~VulkanCommandBuffer();

  void BeginRecording() const;
  void EndRecording() const;
  void Submit(const VkQueue& _queue);
  void Reset();

  operator VkCommandBuffer&() { return m_VkCommandBuffer; }

 private:
  VkCommandBuffer m_VkCommandBuffer;
  VkQueue m_SubmittedQueue;
};

DODO_END_NAMESPACE