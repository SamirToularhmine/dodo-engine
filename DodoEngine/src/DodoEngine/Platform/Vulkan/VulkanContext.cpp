#include <DodoEngine/Platform/Vulkan/VulkanContext.h>

#include <DodoEngine/Utils/Log.h>
#include <DodoEngine/Utils/Utils.h>

#include <algorithm>
#include <ranges>
#include <set>


DODO_BEGIN_NAMESPACE

void VulkanContext::Init(GLFWwindow* _window)
{
    DODO_INFO("Initializing Vulkan context...");

    VkResult volkInitResult = volkInitialize();
    if(volkInitResult != VK_SUCCESS)
    {
        DODO_CRITICAL("Can't initialize Volk");
    }

    VkApplicationInfo applicationInfo{};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = DODO_ENGINE_NAME;
    applicationInfo.applicationVersion = VK_MAKE_API_VERSION(0, 0, 1, 0);
    applicationInfo.pEngineName = DODO_ENGINE_NAME;
    applicationInfo.engineVersion = VK_MAKE_API_VERSION(0, 0, 1, 0);
    applicationInfo.apiVersion = VK_API_VERSION_1_3;

    m_VulkanInstance = std::make_shared<VulkanInstance>(applicationInfo);
    m_VulkanSurface = std::make_shared<VulkanSurface>(m_VulkanInstance, _window);
    m_VulkanPhysicalDevice = std::make_shared<VulkanPhysicalDevice>(*m_VulkanInstance, *m_VulkanSurface);
    m_VulkanDevice = std::make_shared<VulkanDevice>(*m_VulkanPhysicalDevice, *m_VulkanInstance);
    m_VulkanSwapChain = std::make_shared<VulkanSwapChain>(*m_VulkanSurface, *m_VulkanPhysicalDevice, m_VulkanDevice, _window);

    VulkanSwapChainData swapChainData = m_VulkanSwapChain->GetSpec();

    m_VulkanRenderPass = std::make_shared<VulkanRenderPass>(m_VulkanDevice, swapChainData);
    VulkanDescriptorSetLayout vulkanDescriptorSetLayout{m_VulkanDevice };
    m_VulkanGraphicPipeline = std::make_shared<VulkanGraphicPipeline>(m_VulkanDevice, vulkanDescriptorSetLayout, swapChainData, *m_VulkanRenderPass);

    m_VulkanSwapChain->InitFrameBuffers(m_VkFramebuffers, *m_VulkanRenderPass);

    const QueuesInfo& deviceQueuesInfo = m_VulkanPhysicalDevice->GetQueues();

    VkCommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolCreateInfo.queueFamilyIndex = deviceQueuesInfo.m_QueueFamilyIndices.graphicsFamily.value();

    const VkResult createCommandPoolResult = vkCreateCommandPool(*m_VulkanDevice, &commandPoolCreateInfo, nullptr, &m_VkCommandPool);
    if(createCommandPoolResult != VK_SUCCESS) {
        DODO_CRITICAL("Could not create the command pool");
    }

    m_VkCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = m_VkCommandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

    const VkResult allocateCommandBufferResult = vkAllocateCommandBuffers(*m_VulkanDevice, &commandBufferAllocateInfo, m_VkCommandBuffers.data());
    if(allocateCommandBufferResult != VK_SUCCESS) {
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

    for(uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        const VkResult createImageAvailableSemaphoreResult = vkCreateSemaphore(*m_VulkanDevice, &semaphoreCreateInfo, nullptr, &m_VkImagesAvailable[i]);
        const VkResult createRenderFinishedSemaphoreResult = vkCreateSemaphore(*m_VulkanDevice, &semaphoreCreateInfo, nullptr, &m_VkRenderFinishedSemaphores[i]);
        const VkResult createInFlightFenceResult = vkCreateFence(*m_VulkanDevice, &fenceCreateInfo, nullptr, &m_VkInFlightFences[i]);

        if (createImageAvailableSemaphoreResult != VK_SUCCESS || createRenderFinishedSemaphoreResult != VK_SUCCESS || createInFlightFenceResult != VK_SUCCESS) {
            DODO_CRITICAL("Could not create synchronisation objects");
        }
    }

    m_NativeWindow = _window;

    DODO_INFO("Vulkan context initialized successfully");
}

void VulkanContext::BeginRenderPass(VulkanRenderPassData& _vulkanRenderPassData)
{
    const uint32_t currentFrameIndex = _vulkanRenderPassData.m_FrameCount % MAX_FRAMES_IN_FLIGHT;
    _vulkanRenderPassData.m_FrameIndex = currentFrameIndex;

    vkWaitForFences(*m_VulkanDevice, 1, &m_VkInFlightFences[currentFrameIndex], VK_TRUE, UINT64_MAX);
    
    if(!m_VulkanSwapChain->AcquireNextImage(m_VkImagesAvailable[currentFrameIndex], _vulkanRenderPassData.m_ImageIndex))
    {
        DODO_ERROR("Could not acquire a new Swap Chain image, trying to recreate the Swap Chain...");
        m_VulkanSwapChain.reset();
        DestroyFrameBuffers();
        m_VulkanSwapChain = std::make_shared<VulkanSwapChain>(*m_VulkanSurface, *m_VulkanPhysicalDevice, m_VulkanDevice, m_NativeWindow);
        m_VulkanSwapChain->InitFrameBuffers(m_VkFramebuffers, *m_VulkanRenderPass);
        return;
    }

    vkResetFences(*m_VulkanDevice, 1, &m_VkInFlightFences[currentFrameIndex]);

    const VkCommandBuffer chosenCommandBuffer = m_VkCommandBuffers[currentFrameIndex];
	vkResetCommandBuffer(chosenCommandBuffer, 0);
    
    VkCommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    const VkResult beginCommandBufferResult = vkBeginCommandBuffer(chosenCommandBuffer, &commandBufferBeginInfo);
    if (beginCommandBufferResult != VK_SUCCESS) {
        DODO_CRITICAL("Could not begin recording command buffer");
    }

    _vulkanRenderPassData.m_CommandBuffer = chosenCommandBuffer;
    _vulkanRenderPassData.m_FrameBuffer = m_VkFramebuffers[_vulkanRenderPassData.m_ImageIndex];
    _vulkanRenderPassData.m_SwapChainData = m_VulkanSwapChain->GetSpec();

    m_VulkanRenderPass->Begin(_vulkanRenderPassData);

    vkCmdBindPipeline(chosenCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_VulkanGraphicPipeline);

    vkCmdSetViewport(chosenCommandBuffer, 0, 1, &m_VulkanGraphicPipeline->GetViewPort());
    vkCmdSetScissor(chosenCommandBuffer, 0, 1, &m_VulkanGraphicPipeline->GetScissor());

    _vulkanRenderPassData.m_RenderPassStarted = true;
}

void VulkanContext::EndRenderPass(const VulkanRenderPassData& _vulkanRenderPassData) const
{
    const uint32_t currentFrameIndex = _vulkanRenderPassData.m_FrameIndex;

    m_VulkanRenderPass->End(_vulkanRenderPassData);

    VkResult endCommandBufferResult = vkEndCommandBuffer(m_VkCommandBuffers[currentFrameIndex]);
    if (endCommandBufferResult != VK_SUCCESS) {
        DODO_CRITICAL("Could not end the render pass");
    }

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    const VkSemaphore waitSemaphores[] = { m_VkImagesAvailable[currentFrameIndex] };
    const VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_VkCommandBuffers[currentFrameIndex];

    VkSemaphore signalSemaphores[] = { m_VkRenderFinishedSemaphores[currentFrameIndex] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    const VulkanDevice::Queues& deviceQueues = m_VulkanDevice->GetQueues();
    const VkResult submitQueueResult = vkQueueSubmit(deviceQueues.m_GraphicsFamilyQueue, 1, &submitInfo, m_VkInFlightFences[currentFrameIndex]);
    if (submitQueueResult != VK_SUCCESS) {
        DODO_CRITICAL("Could not submit the queue command buffer");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    const VkSwapchainKHR swapChains[] = { *m_VulkanSwapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &_vulkanRenderPassData.m_ImageIndex;

    vkQueuePresentKHR(deviceQueues.m_PresentationQueue, &presentInfo);
}

void VulkanContext::DestroyFrameBuffers()
{
    for (uint32_t i = 0; i < m_VkFramebuffers.size(); ++i)
    {
        vkDestroyFramebuffer(*m_VulkanDevice, m_VkFramebuffers[i], nullptr);
    }

    m_VkFramebuffers.clear();
}

void VulkanContext::Shutdown()
{
    vkDeviceWaitIdle(*m_VulkanDevice);

    DestroyFrameBuffers();

    for(uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        vkDestroySemaphore(*m_VulkanDevice, m_VkImagesAvailable[i], nullptr);
        vkDestroySemaphore(*m_VulkanDevice, m_VkRenderFinishedSemaphores[i], nullptr);
        vkDestroyFence(*m_VulkanDevice, m_VkInFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(*m_VulkanDevice, m_VkCommandPool, nullptr);
}

DODO_END_NAMESPACE
