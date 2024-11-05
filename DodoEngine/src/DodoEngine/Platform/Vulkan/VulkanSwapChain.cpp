#include <DodoEngine/Platform/Vulkan/VulkanDepthImage.h>
#include <DodoEngine/Platform/Vulkan/VulkanDevice.h>
#include <DodoEngine/Platform/Vulkan/VulkanFrameBuffer.h>
#include <DodoEngine/Platform/Vulkan/VulkanPhysicalDevice.h>
#include <DodoEngine/Platform/Vulkan/VulkanRenderPass.h>
#include <DodoEngine/Platform/Vulkan/VulkanSurface.h>
#include <DodoEngine/Platform/Vulkan/VulkanSwapChain.h>
#include <DodoEngine/Utils/Log.h>

#include <GLFW/glfw3.h>

#include <algorithm>

DODO_BEGIN_NAMESPACE

VulkanSwapChain::VulkanSwapChain(const VulkanSurface &_vulkanSurface,
                                 const VulkanPhysicalDevice &_vulkanPhysicalDevice,
                                 Ref<VulkanDevice> _vulkanDevice, GLFWwindow *_window)
    : m_VulkanDevice(_vulkanDevice)
{
  // Surface capabilities
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_vulkanPhysicalDevice, _vulkanSurface,
                                            &m_Spec.m_VkSurfaceCapabilities);

  // Surface formats
  uint32_t surfaceFormatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(_vulkanPhysicalDevice, _vulkanSurface, &surfaceFormatCount,
                                       nullptr);

  if (surfaceFormatCount > 0)
  {
    std::vector<VkSurfaceFormatKHR> &surfaceFormats = m_Spec.m_VkSurfaceFormats;
    surfaceFormats.resize((surfaceFormatCount));
    vkGetPhysicalDeviceSurfaceFormatsKHR(_vulkanPhysicalDevice, _vulkanSurface, &surfaceFormatCount,
                                         surfaceFormats.data());
  }

  // Presentation modes
  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(_vulkanPhysicalDevice, _vulkanSurface,
                                            &presentModeCount, nullptr);

  if (presentModeCount > 0)
  {
    std::vector<VkPresentModeKHR> &presentModes = m_Spec.m_VkPresentModes;
    presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(_vulkanPhysicalDevice, _vulkanSurface,
                                              &presentModeCount, presentModes.data());
  }

  if (!m_Spec.isFullFilled())
  {
    DODO_CRITICAL("Could not fullfill swapchain details");
  }

  // Chosing the right surface format
  SurfaceFormats &surfaceFormats = m_Spec.m_VkSurfaceFormats;
  VkSurfaceFormatKHR chosenSurfaceFormat = surfaceFormats.front();
  for (const VkSurfaceFormatKHR &surfaceFormat : surfaceFormats)
  {
    if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
        surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
    {
      chosenSurfaceFormat = surfaceFormat;
      break;
    }
  }

  // Chosing the present mode : refacto to chose an available one
  VkPresentModeKHR chosenPresentMode = VK_PRESENT_MODE_FIFO_KHR;

  // Chosing the swap chain extent
  const VkSurfaceCapabilitiesKHR &capabilities = m_Spec.m_VkSurfaceCapabilities;
  VkExtent2D chosenExtent;
  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
  {
    chosenExtent = capabilities.currentExtent;
  }

  int width, height;
  glfwGetFramebufferSize(_window, &width, &height);

  chosenExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
  chosenExtent.width = std::clamp(chosenExtent.width, capabilities.minImageExtent.width,
                                  capabilities.maxImageExtent.width);
  chosenExtent.height = std::clamp(chosenExtent.height, capabilities.minImageExtent.height,
                                   capabilities.maxImageExtent.height);

  // Plus one from the min so the app is not waiting for work
  uint32_t imageCount = capabilities.minImageCount + 1;

  VkSwapchainCreateInfoKHR swapChainCreateInfo{};
  swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapChainCreateInfo.surface = _vulkanSurface;
  swapChainCreateInfo.minImageCount = imageCount;
  swapChainCreateInfo.imageFormat = chosenSurfaceFormat.format;
  swapChainCreateInfo.presentMode = chosenPresentMode;
  swapChainCreateInfo.imageExtent = chosenExtent;
  swapChainCreateInfo.imageArrayLayers = 1;
  swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  swapChainCreateInfo.preTransform = capabilities.currentTransform;
  swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapChainCreateInfo.presentMode = chosenPresentMode;
  swapChainCreateInfo.clipped = VK_TRUE;
  swapChainCreateInfo.oldSwapchain = nullptr;

  const VulkanDevice &vulkanDevice = *m_VulkanDevice;
  VkResult swapChainCreateResult =
      vkCreateSwapchainKHR(vulkanDevice, &swapChainCreateInfo, nullptr, &m_VkSwapChain);
  if (swapChainCreateResult != VK_SUCCESS)
  {
    DODO_CRITICAL("Could not create the Swap Chain");
  }

  // Filling in the swapchain images
  vkGetSwapchainImagesKHR(vulkanDevice, m_VkSwapChain, &m_ImageCount, nullptr);
  m_Images.resize(m_ImageCount);
  vkGetSwapchainImagesKHR(vulkanDevice, m_VkSwapChain, &m_ImageCount, m_Images.data());

  m_DepthImage = MakePtr<VulkanDepthImage>(chosenExtent.width, chosenExtent.height);
  m_ChosenSwapChainDetails = {chosenSurfaceFormat, chosenExtent, chosenPresentMode,
                              m_DepthImage->GetFormat()};

  std::ranges::for_each(
      std::begin(m_Images), std::end(m_Images),
      [&](VkImage _image)
      {
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = _image;
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = m_ChosenSwapChainDetails.m_VkSurfaceFormat.format;
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        VkImageView imageView;
        const VkResult createImageViewResult =
            vkCreateImageView(vulkanDevice, &imageViewCreateInfo, nullptr, &imageView);
        if (createImageViewResult != VK_SUCCESS)
        {
          DODO_CRITICAL("Could not create image view");
        }

        m_ImageViews.emplace_back(imageView);
      });
}

VulkanSwapChain::~VulkanSwapChain()
{
  DestroyFrameBuffers();
  vkDestroySwapchainKHR(*m_VulkanDevice, m_VkSwapChain, nullptr);

  for (uint32_t i = 0; i < m_ImageViews.size(); ++i)
  {
    vkDestroyImageView(*m_VulkanDevice, m_ImageViews[i], nullptr);
  }
}

void VulkanSwapChain::InitFrameBuffers(const VulkanRenderPass &_renderPass)
{
  std::ranges::for_each(
      std::begin(m_ImageViews), std::end(m_ImageViews),
      [&](VkImageView _imageView)
      {
        const std::vector<VkImageView> attachments = {_imageView, m_DepthImage->GetImageView()};
        const VkExtent2D frameBufferSize = m_ChosenSwapChainDetails.m_VkExtent;

        m_FrameBuffers.emplace_back(MakeRef<VulkanFrameBuffer>(m_VulkanDevice, _renderPass,
                                                               attachments, frameBufferSize.width,
                                                               frameBufferSize.height));
      });
}

void VulkanSwapChain::DestroyFrameBuffers() { m_FrameBuffers.clear(); }

bool VulkanSwapChain::AcquireNextImage(VkSemaphore _semaphore, uint32_t &_imageIndex)
{
  return vkAcquireNextImageKHR(*m_VulkanDevice, m_VkSwapChain, UINT64_MAX, _semaphore,
                               VK_NULL_HANDLE, &_imageIndex) == VK_SUCCESS;
}

DODO_END_NAMESPACE
