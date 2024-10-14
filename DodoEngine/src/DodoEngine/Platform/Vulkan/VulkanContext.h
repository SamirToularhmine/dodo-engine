#pragma once

#include <DodoEngine/Platform/Vulkan/VulkanDepthImage.h>
#include <DodoEngine/Core/GraphicContext.h>
#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Platform/Vulkan/VulkanFrameBuffer.h>
#include <DodoEngine/Platform/Vulkan/VulkanSwapChain.h>
#include <DodoEngine/Utils/Utils.h>

#include <vk_mem_alloc.h>
#include <volk.h>

#include <vector>

DODO_BEGIN_NAMESPACE

class VulkanInstance;
class VulkanSurface;
class VulkanPhysicalDevice;
class VulkanDevice;
class VulkanSwapChain;
class VulkanRenderPass;
class VulkanGraphicPipeline;
class VulkanDescriptorPool;
class VulkanDescriptorSet;
class VulkanDescriptorSetLayout;
struct VulkanSwapChainData;

struct VulkanRenderPassData {
  uint32_t m_FrameCount;
  uint32_t m_ImageIndex{0};
  uint32_t m_FrameIndex{0};
  VkCommandBuffer m_CommandBuffer;
  VkFramebuffer m_FrameBuffer;
  VulkanSwapChainData m_SwapChainData;
  VkPipelineLayout m_PipelineLayout;
  Ref<VulkanDescriptorSet> m_DescriptorSet;
  bool m_RenderPassStarted{false};

  VulkanRenderPassData(uint32_t _frameCount) : m_FrameCount(_frameCount) {}
};

class VulkanContext : public GraphicContext {
  using FrameBuffers = std::vector<VulkanFrameBuffer>;

 public:
  static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

 public:
  void Init(const Window& _window) override;

  void Update(uint32_t _frameId) override {}

  void Shutdown() override;


  void BeginRenderPass(VulkanRenderPassData& _vulkanRenderPassData);
  void EndRenderPass(const VulkanRenderPassData& _vulkanRenderPassData);

  void BeginUIRenderPass(VulkanRenderPassData& _vulkanRenderPassData);
  void EndUIRenderPass(const VulkanRenderPassData& _vulkanRenderPassData);

  const Ref<VulkanDevice>& GetVulkanDevice() const { return m_VulkanDevice; }
  const Ref<VulkanPhysicalDevice>& GetVulkanPhysicalDevice() const { return m_VulkanPhysicalDevice; }
  const VmaAllocator& GetAllocator() const { return m_VmaAllocator; }
  const VkCommandPool& GetCommandPool() const { return m_VkCommandPool; }
  const Ref<VulkanInstance> GetInstance() const { return m_VulkanInstance; }
  const Ref<VulkanSwapChain> GetSwapChain() const { return m_VulkanSwapChain; }
  const Ref<VulkanDescriptorPool> GetDescriptorPool() const { return m_VulkanDescriptorPool; }
  const VkCommandBuffer& GetCommandBuffer(uint32_t _frameIndex) const { return m_VkCommandBuffers[_frameIndex]; }
  const Ref<VulkanRenderPass>& GetRenderPass() const { return m_VulkanRenderPass; }
  const VulkanTextureImage& GetOffScreenTextureImage() const { return *m_OffScreenTextureImage; }
  
  static VulkanContext& Get() {
    static VulkanContext instance;
    return instance;
  }

 private:
  void BeginCommandBuffer(VulkanRenderPassData& _vulkanRenderPassData);
  void EndCommandBuffer(uint32_t _frameIndex);
  void SubmitQueue(uint32_t _frameIndex) const;
  void PresentQueue(const VulkanRenderPassData& _vulkanRenderPassData);
  void DestroyFrameBuffers();

 private:
  Ref<VulkanInstance> m_VulkanInstance;
  Ref<VulkanSurface> m_VulkanSurface;
  Ref<VulkanPhysicalDevice> m_VulkanPhysicalDevice;
  Ref<VulkanDevice> m_VulkanDevice;
  Ref<VulkanSwapChain> m_VulkanSwapChain;
  Ref<VulkanRenderPass> m_VulkanRenderPass;
  Ref<VulkanDescriptorPool> m_VulkanDescriptorPool;

  VmaAllocator m_VmaAllocator;

  GLFWwindow* m_NativeWindow;
  FrameBuffers m_VkFramebuffers;
  VkCommandPool m_VkCommandPool;
  std::vector<VkCommandBuffer> m_VkCommandBuffers;
  std::vector<VkSemaphore> m_VkImagesAvailable;
  std::vector<VkSemaphore> m_VkRenderFinishedSemaphores;
  std::vector<VkFence> m_VkInFlightFences;


  Ptr<VulkanFrameBuffer> m_OffScreenFrameBuffer;
  Ptr<VulkanDepthImage> m_DepthImage;
  Ptr<VulkanTextureImage> m_OffScreenTextureImage;
  Ref<VulkanRenderPass> m_OffScreenRenderPass;
};

DODO_END_NAMESPACE