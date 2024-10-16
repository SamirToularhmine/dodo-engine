#include <DodoEngine/Platform/Vulkan/VulkanContext.h>

#include <DodoEngine/Core/Window.h>
#include <DodoEngine/Platform/Vulkan/VulkanCommandBuffer.h>
#include <DodoEngine/Platform/Vulkan/VulkanDepthImage.h>
#include <DodoEngine/Platform/Vulkan/VulkanDescriptorPool.h>
#include <DodoEngine/Platform/Vulkan/VulkanDescriptorSet.h>
#include <DodoEngine/Platform/Vulkan/VulkanDescriptorSetLayout.h>
#include <DodoEngine/Platform/Vulkan/VulkanDevice.h>
#include <DodoEngine/Platform/Vulkan/VulkanFrameBuffer.h>
#include <DodoEngine/Platform/Vulkan/VulkanGraphicPipeline.h>
#include <DodoEngine/Platform/Vulkan/VulkanInstance.h>
#include <DodoEngine/Platform/Vulkan/VulkanPhysicalDevice.h>
#include <DodoEngine/Platform/Vulkan/VulkanRenderPass.h>
#include <DodoEngine/Platform/Vulkan/VulkanRenderer.h>
#include <DodoEngine/Platform/Vulkan/VulkanSurface.h>
#include <DodoEngine/Platform/Vulkan/VulkanSwapChain.h>
#include <DodoEngine/Platform/Vulkan/VulkanTextureImage.h>
#include <DodoEngine/Utils/Log.h>
#include <DodoEngine/Utils/Utils.h>

#define VK_NO_PROTOTYPES
#include <volk.h>

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

  m_Instance = std::make_shared<VulkanInstance>(applicationInfo);
  m_Surface = std::make_shared<VulkanSurface>(m_Instance, m_NativeWindow);
  m_PhysicalDevice = std::make_shared<VulkanPhysicalDevice>(*m_Instance, *m_Surface);
  m_Device = std::make_shared<VulkanDevice>(*m_PhysicalDevice, *m_Instance);

  const VkDevice& device = *m_Device;

  VmaVulkanFunctions vulkanFunctions = {};
  vulkanFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
  vulkanFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

  VmaAllocatorCreateInfo vmaAllocatorCreateInfo{};
  vmaAllocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
  vmaAllocatorCreateInfo.instance = *m_Instance;
  vmaAllocatorCreateInfo.device = device;
  vmaAllocatorCreateInfo.physicalDevice = *m_PhysicalDevice;
  vmaAllocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;

  vmaCreateAllocator(&vmaAllocatorCreateInfo, &m_VmaAllocator);

  const QueuesInfo& deviceQueuesInfo = m_PhysicalDevice->GetQueues();

  VkCommandPoolCreateInfo commandPoolCreateInfo{};
  commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  commandPoolCreateInfo.queueFamilyIndex = deviceQueuesInfo.m_QueueFamilyIndices.graphicsFamily.value();

  const VkResult createCommandPoolResult = vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &m_CommandPool);
  if (createCommandPoolResult != VK_SUCCESS) {
    DODO_CRITICAL("Could not create the command pool");
  }

  m_CommandBuffers.resize(MAX_FRAMES_IN_FLIGHT, std::make_shared<VulkanCommandBuffer>());

  m_ImagesAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

  VkSemaphoreCreateInfo semaphoreCreateInfo{};
  semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceCreateInfo{};
  fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
    const VkResult createImageAvailableSemaphoreResult = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &m_ImagesAvailableSemaphores[i]);
    const VkResult createRenderFinishedSemaphoreResult = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &m_RenderFinishedSemaphores[i]);
    const VkResult createInFlightFenceResult = vkCreateFence(device, &fenceCreateInfo, nullptr, &m_InFlightFences[i]);

    if (createImageAvailableSemaphoreResult != VK_SUCCESS || createRenderFinishedSemaphoreResult != VK_SUCCESS || createInFlightFenceResult != VK_SUCCESS) {
      DODO_CRITICAL("Could not create synchronisation objects");
    }
  }

  m_SwapChain = std::make_shared<VulkanSwapChain>(*m_Surface, *m_PhysicalDevice, m_Device, m_NativeWindow);
  VulkanSwapChainData swapChainData = m_SwapChain->GetSpec();
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
  m_UiRenderPass = std::make_shared<VulkanRenderPass>(m_Device, renderPassAttachments);
  m_SwapChain->InitFrameBuffers(*m_UiRenderPass);

  m_DescriptorPool = std::make_shared<VulkanDescriptorPool>(MAX_FRAMES_IN_FLIGHT, m_Device);

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
  m_SceneRenderPass = std::make_shared<VulkanRenderPass>(m_Device, offScreenRenderPassAttachments);

  const VkExtent2D viewPortSize = swapChainData.m_VkExtent;
  RescaleOffscreenTextureImage(viewPortSize.width, viewPortSize.height);

  DODO_INFO("Vulkan context initialized successfully");
}

void VulkanContext::BeginSceneRenderPass(RenderPass& _renderPass) const {
  _renderPass.m_FrameBuffer = m_OffScreenFrameBuffer;
  _renderPass.m_RenderPass = m_SceneRenderPass;

  m_SceneRenderPass->Begin(_renderPass);
}

void VulkanContext::EndSceneRenderPass(const RenderPass& _renderPass) const {
  DODO_TRACE(GameLoop);

  m_SceneRenderPass->End(_renderPass);
}

void VulkanContext::BeginUIRenderPass(RenderPass& _renderPass) {
  const VkDevice& device = *m_Device;
  const uint32_t frameIndex = _renderPass.m_Frame.m_FrameNumber % MAX_FRAMES_IN_FLIGHT;
  _renderPass.m_RenderPass = m_UiRenderPass;

  vkWaitForFences(device, 1, &m_InFlightFences[frameIndex], VK_TRUE, UINT64_MAX);

  bool swapChainImageRetrieved = m_SwapChain->AcquireNextImage(m_ImagesAvailableSemaphores[frameIndex], _renderPass.m_Frame.m_ImageIndex);
  if (!swapChainImageRetrieved) {
    RecreateSwapChain(frameIndex, _renderPass);
    return;
  }

  vkResetFences(device, 1, &m_InFlightFences[frameIndex]);

  _renderPass.m_FrameBuffer = m_SwapChain->GetCurrentFrameBuffer(frameIndex);

  m_UiRenderPass->Begin(_renderPass);
}

void VulkanContext::EndUIRenderPass(const RenderPass& _renderPass) const {
  m_UiRenderPass->End(_renderPass);
}

void VulkanContext::RescaleOffscreenTextureImage(uint32_t _width, uint32_t _height) {
  m_OffScreenTextureImage.reset();
  m_DepthImage.reset();
  m_OffScreenFrameBuffer.reset();

  m_OffScreenTextureImage = VulkanTextureImage::CreateEmptyImage(_width, _height);
  m_DepthImage = std::make_unique<VulkanDepthImage>(_width, _height);

  const std::vector<VkImageView> attachments = {m_OffScreenTextureImage->GetImageView(), m_DepthImage->GetImageView()};
  m_OffScreenFrameBuffer = std::make_unique<VulkanFrameBuffer>(m_Device, *m_SceneRenderPass, attachments, _width, _height);
}

void VulkanContext::RecreateSwapChain(const uint32_t& _frameIndex, RenderPass& _renderPass) {
  DODO_ERROR("Could not retrieve an image from the swapchain, recreating it...");
  vkDeviceWaitIdle(*m_Device);

  m_SwapChain.reset();
  m_SwapChain = std::make_shared<VulkanSwapChain>(*m_Surface, *m_PhysicalDevice, m_Device, m_NativeWindow);
  m_SwapChain->InitFrameBuffers(*_renderPass.m_RenderPass);
  _renderPass.m_Frame.m_Error = true;
}

void VulkanContext::SubmitQueue(const Frame& _frame) {

  const VkCommandBuffer& commandBuffer = *_frame.m_CommandBuffer;
  const uint32_t frameIndex = _frame.m_FrameNumber % MAX_FRAMES_IN_FLIGHT;

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  const VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = &m_ImagesAvailableSemaphores[frameIndex];
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &m_RenderFinishedSemaphores[frameIndex];

  const VulkanDevice::Queues& deviceQueues = m_Device->GetQueues();
  const VkResult submitQueueResult = vkQueueSubmit(deviceQueues.m_GraphicsFamilyQueue, 1, &submitInfo, m_InFlightFences[frameIndex]);
  if (submitQueueResult != VK_SUCCESS) {
    DODO_CRITICAL("Could not submit the queue command buffer");
  }
}

void VulkanContext::PresentQueue(const Frame& _frame) {
  const VkSwapchainKHR swapChains[] = {*m_SwapChain};

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &m_RenderFinishedSemaphores[_frame.m_FrameNumber % MAX_FRAMES_IN_FLIGHT];
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &_frame.m_ImageIndex;

  const VulkanDevice::Queues& deviceQueues = m_Device->GetQueues();
  vkQueuePresentKHR(deviceQueues.m_PresentationQueue, &presentInfo);
}

void VulkanContext::Shutdown() {
  const VkDevice& device = *m_Device;
  vkDeviceWaitIdle(*m_Device);

  m_CommandBuffers.clear();

  vkDestroyFramebuffer(device, *m_OffScreenFrameBuffer, nullptr);

  for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
    vkDestroySemaphore(device, m_ImagesAvailableSemaphores[i], nullptr);
    vkDestroySemaphore(device, m_RenderFinishedSemaphores[i], nullptr);
    vkDestroyFence(device, m_InFlightFences[i], nullptr);
  }

  vkDestroyCommandPool(device, m_CommandPool, nullptr);

  m_SwapChain.reset();
  m_DepthImage.reset();
  m_OffScreenTextureImage.reset();

  vmaDestroyAllocator(m_VmaAllocator);
}

DODO_END_NAMESPACE
