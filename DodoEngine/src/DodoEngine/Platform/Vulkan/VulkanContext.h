#pragma once

#include <DodoEngine/Core/GraphicContext.h>
#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Utils/Utils.h>

#define VK_NO_PROTOTYPES
#include <volk.h>
#include <vk_mem_alloc.h>

#include <vector>

DODO_BEGIN_NAMESPACE

struct Frame;
struct RenderPass;
class VulkanCommandBuffer;
class VulkanDepthImage;
class VulkanDescriptorPool;
class VulkanDescriptorSet;
class VulkanDescriptorSetLayout;
class VulkanDevice;
class VulkanFrameBuffer;
class VulkanInstance;
class VulkanGraphicPipeline;
class VulkanPhysicalDevice;
class VulkanRenderPass;
class VulkanSurface;
class VulkanSwapChain;
class VulkanTextureImage;
struct VulkanSwapChainData;

class VulkanContext : public GraphicContext {
 public:
  static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

 public:
  void Init(const Window& _window) override;

  void Update(uint32_t _frameId) override {}

  void Shutdown() override;

  void BeginSceneRenderPass(RenderPass& _renderPass) const;
  void EndSceneRenderPass(const RenderPass& _renderPass) const;

  void BeginUIRenderPass(RenderPass& _renderPass);
  void EndUIRenderPass(const RenderPass& _renderPass) const;

  void RescaleOffscreenTextureImage(uint32_t _width, uint32_t _height);
  void RecreateSwapChain(const uint32_t& _frameIndex, RenderPass& _renderPass);

  const Ref<VulkanDevice>& GetVulkanDevice() const { return m_Device; }
  const Ref<VulkanPhysicalDevice>& GetVulkanPhysicalDevice() const { return m_PhysicalDevice; }
  const VmaAllocator& GetAllocator() const { return m_VmaAllocator; }
  const VkCommandPool& GetCommandPool() const { return m_CommandPool; }
  const Ref<VulkanInstance> GetInstance() const { return m_Instance; }
  const Ref<VulkanSwapChain> GetSwapChain() const { return m_SwapChain; }
  const Ref<VulkanDescriptorPool> GetDescriptorPool() const { return m_DescriptorPool; }
  const Ref<VulkanRenderPass> GetSceneRenderPass() const { return m_SceneRenderPass; }
  const Ref<VulkanRenderPass> GetUiRenderPass() const { return m_UiRenderPass; }
  const Ref<VulkanCommandBuffer>& GetCommandBuffer(const uint32_t& _frameNumber) const { return m_CommandBuffers[_frameNumber % MAX_FRAMES_IN_FLIGHT]; }
  const Ref<VulkanTextureImage>& GetOffScreenTextureImage() const { return m_OffScreenTextureImage; }
  
  void SubmitQueue(const Frame& _frame);
  void PresentQueue(const Frame& _frame);

  static VulkanContext& Get() {
    static VulkanContext instance;
    return instance;
  }

 private:
  GLFWwindow* m_NativeWindow;

  VmaAllocator m_VmaAllocator;

  Ref<VulkanInstance> m_Instance;
  Ref<VulkanSurface> m_Surface;
  Ref<VulkanPhysicalDevice> m_PhysicalDevice;
  Ref<VulkanDevice> m_Device;
  Ref<VulkanSwapChain> m_SwapChain;
  Ref<VulkanDescriptorPool> m_DescriptorPool;
  Ref<VulkanFrameBuffer> m_OffScreenFrameBuffer;
  Ref<VulkanRenderPass> m_SceneRenderPass;
  Ref<VulkanRenderPass> m_UiRenderPass;
  Ref<VulkanTextureImage> m_OffScreenTextureImage;
  
  Ref<VulkanDepthImage> m_DepthImage;

  VkCommandPool m_CommandPool;

  std::vector<Ref<VulkanCommandBuffer>> m_CommandBuffers;
  std::vector<VkSemaphore> m_ImagesAvailableSemaphores;
  std::vector<VkSemaphore> m_RenderFinishedSemaphores;
  std::vector<VkFence> m_InFlightFences;
};

DODO_END_NAMESPACE