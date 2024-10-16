#include <DodoEngine/Platform/Vulkan/VulkanContext.h>

#include <DodoEngine/Core/Window.h>
#include <DodoEngine/Platform/Vulkan/VulkanDescriptorPool.h>
#include <DodoEngine/Platform/Vulkan/VulkanDescriptorSet.h>
#include <DodoEngine/Platform/Vulkan/VulkanDescriptorSetLayout.h>
#include <DodoEngine/Platform/Vulkan/VulkanDevice.h>
#include <DodoEngine/Platform/Vulkan/VulkanGraphicPipeline.h>
#include <DodoEngine/Platform/Vulkan/VulkanInstance.h>
#include <DodoEngine/Platform/Vulkan/VulkanPhysicalDevice.h>
#include <DodoEngine/Platform/Vulkan/VulkanRenderPass.h>
#include <DodoEngine/Platform/Vulkan/VulkanSurface.h>
#include <DodoEngine/Utils/Log.h>
#include <DodoEngine/Utils/Utils.h>

#define VMA_IMPLEMENTATION
#define VMA_DEBUG_INITIALIZE_ALLOCATIONS 1
#define VMA_DEBUG_DETECT_CORRUPTION 1
#define VMA_LEAK_LOG_FORMAT
#include <vk_mem_alloc.h>

#include <ranges>
#include <set>

DODO_BEGIN_NAMESPACE

void VulkanContext::Init(const Window& _window) {
  DODO_INFO("Initializing Vulkan context...");

  VkResult volkInitResult = volkInitialize();
  if (volkInitResult != VK_SUCCESS) {
    DODO_CRITICAL("Can't initialize Volk");
  }

  m_NativeWindow = _window.GetNativeWindow();

  VkApplicationInfo applicationInfo{};
  applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  applicationInfo.pApplicationName = DODO_ENGINE_NAME;
  applicationInfo.applicationVersion = VK_MAKE_API_VERSION(0, 0, 1, 0);
  applicationInfo.pEngineName = DODO_ENGINE_NAME;
  applicationInfo.engineVersion = VK_MAKE_API_VERSION(0, 0, 1, 0);
  applicationInfo.apiVersion = VK_API_VERSION_1_3;

  m_VulkanInstance = std::make_shared<VulkanInstance>(applicationInfo);
  m_VulkanSurface = std::make_shared<VulkanSurface>(m_VulkanInstance, m_NativeWindow);
  m_VulkanPhysicalDevice = std::make_shared<VulkanPhysicalDevice>(*m_VulkanInstance, *m_VulkanSurface);
  m_VulkanDevice = std::make_shared<VulkanDevice>(*m_VulkanPhysicalDevice, *m_VulkanInstance);

  VmaVulkanFunctions vulkanFunctions = {};
  vulkanFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
  vulkanFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

  VmaAllocatorCreateInfo vmaAllocatorCreateInfo{};
  vmaAllocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
  vmaAllocatorCreateInfo.instance = *m_VulkanInstance;
  vmaAllocatorCreateInfo.device = *m_VulkanDevice;
  vmaAllocatorCreateInfo.physicalDevice = *m_VulkanPhysicalDevice;
  vmaAllocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;

  vmaCreateAllocator(&vmaAllocatorCreateInfo, &m_VmaAllocator);

  const QueuesInfo& deviceQueuesInfo = m_VulkanPhysicalDevice->GetQueues();

  VkCommandPoolCreateInfo commandPoolCreateInfo{};
  commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  commandPoolCreateInfo.queueFamilyIndex = deviceQueuesInfo.m_QueueFamilyIndices.graphicsFamily.value();

  const VkResult createCommandPoolResult = vkCreateCommandPool(*m_VulkanDevice, &commandPoolCreateInfo, nullptr, &m_VkCommandPool);
  if (createCommandPoolResult != VK_SUCCESS) {
    DODO_CRITICAL("Could not create the command pool");
  }

  m_VkCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

  VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
  commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  commandBufferAllocateInfo.commandPool = m_VkCommandPool;
  commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  commandBufferAllocateInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

  const VkResult allocateCommandBufferResult = vkAllocateCommandBuffers(*m_VulkanDevice, &commandBufferAllocateInfo, m_VkCommandBuffers.data());
  if (allocateCommandBufferResult != VK_SUCCESS) {
    DODO_CRITICAL("Could not allocate a command buffer");
  }

  m_VkImagesAvailable.resize(MAX_FRAMES_IN_FLIGHT);
  m_VkRenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  m_VkInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

  VkSemaphoreCreateInfo semaphoreCreateInfo{};
  semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceCreateInfo{};
  fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
    const VkResult createImageAvailableSemaphoreResult = vkCreateSemaphore(*m_VulkanDevice, &semaphoreCreateInfo, nullptr, &m_VkImagesAvailable[i]);
    const VkResult createRenderFinishedSemaphoreResult = vkCreateSemaphore(*m_VulkanDevice, &semaphoreCreateInfo, nullptr, &m_VkRenderFinishedSemaphores[i]);
    const VkResult createInFlightFenceResult = vkCreateFence(*m_VulkanDevice, &fenceCreateInfo, nullptr, &m_VkInFlightFences[i]);

    if (createImageAvailableSemaphoreResult != VK_SUCCESS || createRenderFinishedSemaphoreResult != VK_SUCCESS || createInFlightFenceResult != VK_SUCCESS) {
      DODO_CRITICAL("Could not create synchronisation objects");
    }
  }

  m_VulkanSwapChain = std::make_shared<VulkanSwapChain>(*m_VulkanSurface, *m_VulkanPhysicalDevice, m_VulkanDevice, m_NativeWindow);
  VulkanSwapChainData swapChainData = m_VulkanSwapChain->GetSpec();

  VulkanRenderPassAttachments renderPassAttachments{{{
                                                         .format = swapChainData.m_VkSurfaceFormat.format,
                                                         .samples = VK_SAMPLE_COUNT_1_BIT,
                                                         .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                                                         .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                                                         .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                                         .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                                         .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                                         .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                                     },
                                                     {
                                                         .format = swapChainData.m_DepthImageFormat,
                                                         .samples = VK_SAMPLE_COUNT_1_BIT,
                                                         .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                                                         .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                                         .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                                         .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                                         .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                                         .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                                                     }}};

  m_VulkanRenderPass = std::make_shared<VulkanRenderPass>(m_VulkanDevice, renderPassAttachments);
  m_VulkanDescriptorPool = std::make_shared<VulkanDescriptorPool>(MAX_FRAMES_IN_FLIGHT, m_VulkanDevice);
  m_VulkanSwapChain->InitFrameBuffers(m_VkFramebuffers, *m_VulkanRenderPass);

  const VkExtent2D viewPortSize = swapChainData.m_VkExtent;
  RescaleOffscreenTextureImage(viewPortSize.width, viewPortSize.height);

  VulkanRenderPassAttachments offScreenRenderPassAttachments{{{
                                                                  .format = swapChainData.m_VkSurfaceFormat.format,
                                                                  .samples = VK_SAMPLE_COUNT_1_BIT,
                                                                  .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                                                                  .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                                                                  .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                                                  .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                                                  .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                                                  .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                              },
                                                              {
                                                                  .format = swapChainData.m_DepthImageFormat,
                                                                  .samples = VK_SAMPLE_COUNT_1_BIT,
                                                                  .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                                                                  .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                                                  .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                                                  .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                                                  .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                                                  .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                                                              }}};
  m_OffScreenRenderPass = std::make_shared<VulkanRenderPass>(m_VulkanDevice, offScreenRenderPassAttachments);

  DODO_INFO("Vulkan context initialized successfully");
}

void VulkanContext::BeginRenderPass(VulkanRenderPassData& _vulkanRenderPassData) {
  BeginCommandBuffer(_vulkanRenderPassData);

  const VulkanImage& offScreenImage = m_OffScreenTextureImage->GetImage();
  _vulkanRenderPassData.m_FrameSize = {offScreenImage.GetWidth(), offScreenImage.GetHeight()};
  _vulkanRenderPassData.m_FrameBuffer = *m_OffScreenFrameBuffer;

  m_OffScreenRenderPass->Begin(_vulkanRenderPassData);
}

void VulkanContext::EndRenderPass(const VulkanRenderPassData& _vulkanRenderPassData) {
  DODO_TRACE(GameLoop);

  const uint32_t currentFrameIndex = _vulkanRenderPassData.m_FrameIndex;

  m_OffScreenRenderPass->End(_vulkanRenderPassData);
}

bool VulkanContext::BeginUIRenderPass(VulkanRenderPassData& _vulkanRenderPassData) {
  const uint32_t currentFrameIndex = _vulkanRenderPassData.m_FrameCount % MAX_FRAMES_IN_FLIGHT;

  vkWaitForFences(*m_VulkanDevice, 1, &m_VkInFlightFences[currentFrameIndex], VK_TRUE, UINT64_MAX);

  if (!m_VulkanSwapChain->AcquireNextImage(m_VkImagesAvailable[currentFrameIndex], _vulkanRenderPassData.m_ImageIndex)) {
    DODO_ERROR("Could not acquire a new Swap Chain image, trying to recreate the Swap Chain...");
    
    m_VulkanSwapChain.reset();
    DestroyFrameBuffers();
    m_VulkanSwapChain = std::make_shared<VulkanSwapChain>(*m_VulkanSurface, *m_VulkanPhysicalDevice, m_VulkanDevice, m_NativeWindow);
    m_VulkanSwapChain->InitFrameBuffers(m_VkFramebuffers, *m_VulkanRenderPass);
    _vulkanRenderPassData.m_SwapChainData = m_VulkanSwapChain->GetSpec();
    return false;
  }

  vkResetFences(*m_VulkanDevice, 1, &m_VkInFlightFences[currentFrameIndex]);

  _vulkanRenderPassData.m_FrameSize = {_vulkanRenderPassData.m_SwapChainData.m_VkExtent};
  _vulkanRenderPassData.m_FrameBuffer = m_VkFramebuffers[_vulkanRenderPassData.m_ImageIndex];

  m_VulkanRenderPass->Begin(_vulkanRenderPassData);

  return true;
}

void VulkanContext::EndUIRenderPass(const VulkanRenderPassData& _vulkanRenderPassData) {
  DODO_TRACE(GameLoop);

  const uint32_t currentFrameIndex = _vulkanRenderPassData.m_FrameIndex;

  m_VulkanRenderPass->End(_vulkanRenderPassData);

  EndCommandBuffer(currentFrameIndex);
  SubmitQueue(currentFrameIndex);
  PresentQueue(_vulkanRenderPassData);
}

void VulkanContext::RescaleOffscreenTextureImage(uint32_t _width, uint32_t _height) {
  m_OffScreenTextureImage.reset();
  m_DepthImage.reset();
  m_OffScreenFrameBuffer.reset();

  m_OffScreenTextureImage = VulkanTextureImage::CreateEmptyImage(_width, _height);
  m_DepthImage = std::make_unique<VulkanDepthImage>(_width, _height);

  const std::vector<VkImageView> attachments = {m_OffScreenTextureImage->GetImageView(), m_DepthImage->GetImageView()};
  m_OffScreenFrameBuffer = std::make_unique<VulkanFrameBuffer>(m_VulkanDevice, *m_VulkanRenderPass, attachments, _width, _height);
}

void VulkanContext::BeginCommandBuffer(VulkanRenderPassData& _vulkanRenderPassData) {
  const uint32_t currentFrameIndex = _vulkanRenderPassData.m_FrameCount % MAX_FRAMES_IN_FLIGHT;

  vkDeviceWaitIdle(*m_VulkanDevice);

  const VkCommandBuffer chosenCommandBuffer = m_VkCommandBuffers[currentFrameIndex];
  vkResetCommandBuffer(chosenCommandBuffer, 0);

  vkWaitForFences(*m_VulkanDevice, 1, &m_VkInFlightFences[currentFrameIndex], VK_TRUE, UINT64_MAX);

  VkCommandBufferBeginInfo commandBufferBeginInfo{};
  commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  const VkResult beginCommandBufferResult = vkBeginCommandBuffer(chosenCommandBuffer, &commandBufferBeginInfo);
  if (beginCommandBufferResult != VK_SUCCESS) {
    DODO_CRITICAL("Could not begin recording command buffer");
  }

  _vulkanRenderPassData.m_RenderPassStarted = true;
  _vulkanRenderPassData.m_CommandBuffer = chosenCommandBuffer;
  _vulkanRenderPassData.m_SwapChainData = m_VulkanSwapChain->GetSpec();
  _vulkanRenderPassData.m_FrameIndex = currentFrameIndex;
}

void VulkanContext::EndCommandBuffer(uint32_t _frameIndex) {
  VkResult endCommandBufferResult = vkEndCommandBuffer(m_VkCommandBuffers[_frameIndex]);
  if (endCommandBufferResult != VK_SUCCESS) {
    DODO_CRITICAL("Could not end the render pass");
  }
}

void VulkanContext::SubmitQueue(uint32_t _frameIndex) const {
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  const VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = &m_VkImagesAvailable[_frameIndex];
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &m_VkCommandBuffers[_frameIndex];

  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &m_VkRenderFinishedSemaphores[_frameIndex];

  const VulkanDevice::Queues& deviceQueues = m_VulkanDevice->GetQueues();

  const VkResult submitQueueResult = vkQueueSubmit(deviceQueues.m_GraphicsFamilyQueue, 1, &submitInfo, m_VkInFlightFences[_frameIndex]);
  if (submitQueueResult != VK_SUCCESS) {
    DODO_CRITICAL("Could not submit the queue command buffer");
  }
}

void VulkanContext::PresentQueue(const VulkanRenderPassData& _vulkanRenderPassData) {
  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &m_VkRenderFinishedSemaphores[_vulkanRenderPassData.m_FrameIndex];

  const VkSwapchainKHR swapChains[] = {*m_VulkanSwapChain};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &_vulkanRenderPassData.m_ImageIndex;

  const VulkanDevice::Queues& deviceQueues = m_VulkanDevice->GetQueues();

  vkQueuePresentKHR(deviceQueues.m_PresentationQueue, &presentInfo);
}

void VulkanContext::DestroyFrameBuffers() {
  for (uint32_t i = 0; i < m_VkFramebuffers.size(); ++i) {
    vkDestroyFramebuffer(*m_VulkanDevice, m_VkFramebuffers[i], nullptr);
  }

  m_VkFramebuffers.clear();
}

void VulkanContext::Shutdown() {
  vkDeviceWaitIdle(*m_VulkanDevice);

  DestroyFrameBuffers();

  vkDestroyFramebuffer(*m_VulkanDevice, *m_OffScreenFrameBuffer, nullptr);

  for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
    vkDestroySemaphore(*m_VulkanDevice, m_VkImagesAvailable[i], nullptr);
    vkDestroySemaphore(*m_VulkanDevice, m_VkRenderFinishedSemaphores[i], nullptr);
    vkDestroyFence(*m_VulkanDevice, m_VkInFlightFences[i], nullptr);
  }

  vkDestroyCommandPool(*m_VulkanDevice, m_VkCommandPool, nullptr);

  m_VulkanSwapChain.reset();
  m_DepthImage.reset();
  m_OffScreenTextureImage.reset();

  vmaDestroyAllocator(m_VmaAllocator);
}

DODO_END_NAMESPACE
