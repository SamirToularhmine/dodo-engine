#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Renderer/Texture.h>

#define VK_NO_PROTOTYPES
#include <GLFW/glfw3.h>
#include <volk.h>

#include <vector>

DODO_BEGIN_NAMESPACE

class VulkanDevice;
class VulkanDepthImage;
class VulkanFrameBuffer;
class VulkanPhysicalDevice;
class VulkanRenderPass;
class VulkanSurface;

struct VulkanSwapChainData {
  VkSurfaceFormatKHR m_VkSurfaceFormat;
  VkExtent2D m_VkExtent;
  VkPresentModeKHR m_VkPresentMode;
  VkFormat m_DepthImageFormat;
};

using ImageViews = std::vector<VkImageView>;

class VulkanSwapChain {
  using SurfaceFormats = std::vector<VkSurfaceFormatKHR>;
  using PresentModes = std::vector<VkPresentModeKHR>;
  using SwapChainImages = std::vector<VkImage>;
  using FrameBuffers = std::vector<Ref<VulkanFrameBuffer>>;

  struct SwapChainDetails {
    VkSurfaceCapabilitiesKHR m_VkSurfaceCapabilities;
    SurfaceFormats m_VkSurfaceFormats;
    PresentModes m_VkPresentModes;

    const bool isFullFilled() const { return !m_VkSurfaceFormats.empty() && !m_VkPresentModes.empty(); }
  };

 public:
  VulkanSwapChain(const VulkanSurface& _vulkanSurface, const VulkanPhysicalDevice& _vulkanPhysicalDevice, Ref<VulkanDevice> _vulkanDevice, GLFWwindow* _window);
  ~VulkanSwapChain();

  const VulkanSwapChainData& GetSpec() { return m_ChosenSwapChainDetails; }
  const ImageViews& GetImagesViews() { return m_ImageViews; }

  void InitFrameBuffers(const VulkanRenderPass& _renderPass);
  const Ref<VulkanFrameBuffer>& GetCurrentFrameBuffer(uint32_t frameIndex) const { return m_FrameBuffers[frameIndex]; };
  void DestroyFrameBuffers();

  bool AcquireNextImage(const VkSemaphore& _semaphore, uint32_t& _imageIndex) const;

  operator const VkSwapchainKHR&() const { return m_VkSwapChain; }

 private:
  VkSwapchainKHR m_VkSwapChain;
  Ref<VulkanDevice> m_VulkanDevice;

  SwapChainDetails m_Spec;
  FrameBuffers m_FrameBuffers;
  SwapChainImages m_Images;
  ImageViews m_ImageViews;

  Ptr<VulkanDepthImage> m_DepthImage;
  VulkanSwapChainData m_ChosenSwapChainDetails;
};

DODO_END_NAMESPACE
