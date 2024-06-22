#include <DodoEngine/Platform/VulkanContext.h>
#include <DodoEngine/Utils/Log.h>
#include <DodoEngine/Utils/Utils.h>

#include <algorithm>
#include <iostream>
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

    // TODO: perform device suitability checks (extensions, queues)

    Queues& deviceQueues = m_VulkanPhysicalDevice->GetQueues();
    VkPhysicalDeviceFeatures deviceFeatures{};
    QueueFamilyIndices queueFamilyIndices = deviceQueues.m_QueueFamilyIndices;

    std::vector<VkDeviceQueueCreateInfo> deviceQueuesCreateInfo{};
    std::set<QueueIndex> supportedQueueTypesIndex = {
        queueFamilyIndices.graphicsFamily.value(), 
        queueFamilyIndices.presentFamily.value() 
    };

    for(const QueueIndex& queueFamily : supportedQueueTypesIndex) {
        VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
        deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        deviceQueueCreateInfo.queueFamilyIndex = queueFamily;
        deviceQueueCreateInfo.queueCount = 1;
        // TODO: Handle different priorities
        deviceQueueCreateInfo.pQueuePriorities = &QueueFamilyPriorities::k_DEFAULT_PRIORITY;
        deviceQueuesCreateInfo.emplace_back(deviceQueueCreateInfo);
    }

    VulkanInstance::VulkanExtensions requiredDeviceExtension = {
        "VK_KHR_portability_subset", VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VulkanInstance::VulkanValidationLayers requestedValidationLayers = m_VulkanInstance->GetValidationLayers();

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = deviceQueuesCreateInfo.data();
    deviceCreateInfo.queueCreateInfoCount = deviceQueuesCreateInfo.size();
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.enabledExtensionCount = requiredDeviceExtension.size();
    deviceCreateInfo.ppEnabledExtensionNames = requiredDeviceExtension.data();
    deviceCreateInfo.enabledLayerCount = requestedValidationLayers.size();
    deviceCreateInfo.ppEnabledLayerNames = requestedValidationLayers.data();

    VkResult createDeviceResult = vkCreateDevice(*m_VulkanPhysicalDevice, &deviceCreateInfo, nullptr, &m_VkDevice);
    if(createDeviceResult != VK_SUCCESS)
    {
        DODO_CRITICAL("Cannot create the logical vulkan device");
    }

    // Graphics queue handle
    QueueIndex graphicsFamilyQueueIndex = queueFamilyIndices.graphicsFamily.value();
    vkGetDeviceQueue(m_VkDevice, graphicsFamilyQueueIndex, 0, &deviceQueues.m_GraphicsFamilyQueue);

    // Presentation queue handle
    QueueIndex presentationQueueIndex = queueFamilyIndices.presentFamily.value();
    vkGetDeviceQueue(m_VkDevice, presentationQueueIndex, 0, &deviceQueues.m_PresentationQueue);

    // Filling SwapChain details

    // Surface capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*m_VulkanPhysicalDevice, *m_VulkanSurface, &m_SwapChainDetails.m_VkSurfaceCapabilities);

    // Surface formats
    uint32_t surfaceFormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(*m_VulkanPhysicalDevice, *m_VulkanSurface, &surfaceFormatCount, nullptr);

    if(surfaceFormatCount > 0) {
        std::vector<VkSurfaceFormatKHR>& surfaceFormats = m_SwapChainDetails.m_VkSurfaceFormats;
        surfaceFormats.resize((surfaceFormatCount));
        vkGetPhysicalDeviceSurfaceFormatsKHR(*m_VulkanPhysicalDevice, *m_VulkanSurface, &surfaceFormatCount, surfaceFormats.data());
    }

    // Presentation modes
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(*m_VulkanPhysicalDevice, *m_VulkanSurface, &presentModeCount, nullptr);

    if(presentModeCount > 0) {
        std::vector<VkPresentModeKHR>& presentModes = m_SwapChainDetails.m_VkPresentModes;
        presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(*m_VulkanPhysicalDevice, *m_VulkanSurface, &presentModeCount, presentModes.data());
    }

    if(!m_SwapChainDetails.isFullFilled()) {
        DODO_CRITICAL("Could not fullfill swapchain details");
    }

    // Chosing the right surface format
    SurfaceFormats& surfaceFormats = m_SwapChainDetails.m_VkSurfaceFormats;
    VkSurfaceFormatKHR chosenSurfaceFormat = surfaceFormats.front();
    for(const VkSurfaceFormatKHR& surfaceFormat : surfaceFormats) {
        if(surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB && surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            chosenSurfaceFormat = surfaceFormat;
            break;
        }
    }

    // Chosing the present mode : refacto to chose an available one
    VkPresentModeKHR chosenPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;

    // Chosing the swap chain extent
    const VkSurfaceCapabilitiesKHR& capabilities = m_SwapChainDetails.m_VkSurfaceCapabilities;
    VkExtent2D chosenExtent;
    if(capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        chosenExtent = capabilities.currentExtent;
    }

    int width, height;
    glfwGetFramebufferSize(_window, &width, &height);

    chosenExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
    chosenExtent.width = std::clamp(chosenExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    chosenExtent.height = std::clamp(chosenExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    // Plus one from the min so the app is not waiting for work
    uint32_t imageCount = capabilities.minImageCount + 1;

    VkSwapchainCreateInfoKHR swapChainCreateInfo{};
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface = *m_VulkanSurface;
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

    VkResult swapChainCreateResult = vkCreateSwapchainKHR(m_VkDevice, &swapChainCreateInfo, nullptr, &m_VkSwapChain);
    if(swapChainCreateResult != VK_SUCCESS) {
        DODO_CRITICAL("Could not create the Swap Chain");
    }

    // Filling in the swapchain images
    uint32_t swapChainImageCount = 0;
    vkGetSwapchainImagesKHR(m_VkDevice, m_VkSwapChain, &swapChainImageCount, nullptr);
    m_VkSwapChainImages.resize(swapChainImageCount);
    vkGetSwapchainImagesKHR(m_VkDevice, m_VkSwapChain, &swapChainImageCount, m_VkSwapChainImages.data());

    m_ChosenSwapChainDetails = { chosenSurfaceFormat, chosenExtent, chosenPresentMode };

    std::for_each(std::begin(m_VkSwapChainImages), std::end(m_VkSwapChainImages), [&](const VkImage& _image) {
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
        VkResult createImageViewResult = vkCreateImageView(m_VkDevice, &imageViewCreateInfo, nullptr, &imageView);
        if(createImageViewResult != VK_SUCCESS) {
            DODO_CRITICAL("Could not create image view");
        }

        m_VkImagesViews.emplace_back(imageView);
    });

    // Creating the render pass
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = m_ChosenSwapChainDetails.m_VkSurfaceFormat.format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency subpassDependency{};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.srcAccessMask = 0;
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = 1;
    renderPassCreateInfo.pAttachments = &colorAttachment;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpass;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &subpassDependency;

    VkResult createRenderPassResult = vkCreateRenderPass(m_VkDevice, &renderPassCreateInfo, nullptr, &m_VkRenderPass);
    if(createRenderPassResult != VK_SUCCESS) {
        DODO_CRITICAL("Could not create the render pass");
    }

    // Loading shader
    std::vector<char> vertexShaderFile = readFile("vert.spv");
    std::vector<char> fragmentShaderFile = readFile("frag.spv");

    // Vertex Shader
    VkShaderModuleCreateInfo vertexShaderModuleCreateInfo{};
    vertexShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    vertexShaderModuleCreateInfo.codeSize = vertexShaderFile.size();
    vertexShaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(vertexShaderFile.data());

    // Fragment Shader
    VkShaderModuleCreateInfo fragmentShaderModuleCreateInfo{};
    fragmentShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    fragmentShaderModuleCreateInfo.codeSize = fragmentShaderFile.size();
    fragmentShaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(fragmentShaderFile.data());

    VkShaderModule vertexShaderModule;
    VkResult vertexShaderModuleCreateResult = vkCreateShaderModule(m_VkDevice, &vertexShaderModuleCreateInfo, nullptr, &vertexShaderModule);

    VkShaderModule fragmentShaderModule;
    VkResult fragmentShaderModuleCreateResult = vkCreateShaderModule(m_VkDevice, &fragmentShaderModuleCreateInfo, nullptr, &fragmentShaderModule);

    if(vertexShaderModuleCreateResult != VK_SUCCESS || fragmentShaderModuleCreateResult != VK_SUCCESS) {
        DODO_CRITICAL("Could not create shader modules");
    }

    VkPipelineShaderStageCreateInfo vertexShaderStageCreateInfo{};
    vertexShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShaderStageCreateInfo.module = vertexShaderModule;
    vertexShaderStageCreateInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragmentShaderStageCreateInfo{};
    fragmentShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentShaderStageCreateInfo.module = fragmentShaderModule;
    fragmentShaderStageCreateInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStageCreateInfo[] = { vertexShaderStageCreateInfo, fragmentShaderStageCreateInfo };

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
    dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.dynamicStateCount = dynamicStates.size();
    dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();
    
    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
    vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateCreateInfo.vertexBindingDescriptionCount = 0;
    vertexInputStateCreateInfo.pVertexBindingDescriptions = nullptr;
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0;
    vertexInputStateCreateInfo.pVertexBindingDescriptions = nullptr;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
    inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

    m_VkViewPort.x = 0.0f;
    m_VkViewPort.y = 0.0f;
    m_VkViewPort.width = m_ChosenSwapChainDetails.m_VkExtent.width;
    m_VkViewPort.height = m_ChosenSwapChainDetails.m_VkExtent.height;
    m_VkViewPort.minDepth = 0.0f;
    m_VkViewPort.maxDepth = 1.0f;


    m_VkScissor.offset = {0, 0};
    m_VkScissor.extent = m_ChosenSwapChainDetails.m_VkExtent;

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.pViewports = &m_VkViewPort;
    viewportStateCreateInfo.scissorCount = 1;
    viewportStateCreateInfo.pScissors = &m_VkScissor;

    VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo{};
    rasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizerCreateInfo.depthClampEnable = VK_FALSE;
    rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizerCreateInfo.lineWidth = 1.0f;
    rasterizerCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizerCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizerCreateInfo.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampleCreateInfo{};
    multisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleCreateInfo.sampleShadingEnable = VK_FALSE;
    multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
    colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
    colorBlendStateCreateInfo.attachmentCount = 1;
    colorBlendStateCreateInfo.pAttachments = &colorBlendAttachment;

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    VkResult pipeLayoutCreateResult = vkCreatePipelineLayout(m_VkDevice, &pipelineLayoutCreateInfo, nullptr, &m_VkPipelineLayout);
    if(pipeLayoutCreateResult != VK_SUCCESS){
        DODO_CRITICAL("Could not create pipeline layout");
    }

    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
    graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsPipelineCreateInfo.stageCount = 2;
    graphicsPipelineCreateInfo.pStages = shaderStageCreateInfo;
    graphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
    graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    graphicsPipelineCreateInfo.pRasterizationState = &rasterizerCreateInfo;
    graphicsPipelineCreateInfo.pMultisampleState = &multisampleCreateInfo;
    graphicsPipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
    graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
    graphicsPipelineCreateInfo.layout = m_VkPipelineLayout;
    graphicsPipelineCreateInfo.renderPass = m_VkRenderPass;
    graphicsPipelineCreateInfo.subpass = 0;

    VkResult graphicsPipelineCreateResult = vkCreateGraphicsPipelines(m_VkDevice, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &m_VkPipeline);
    if(graphicsPipelineCreateResult != VK_SUCCESS){
        DODO_CRITICAL("Could not create graphics pipeline");
    }

    std::for_each(std::begin(m_VkImagesViews), std::end(m_VkImagesViews), [&](const VkImageView& _imageView) {
        VkImageView attachments[] = {
            _imageView
        };

        VkFramebufferCreateInfo framebufferCreateInfo{};
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.renderPass = m_VkRenderPass;
        framebufferCreateInfo.attachmentCount = 1;
        framebufferCreateInfo.pAttachments = attachments;
        framebufferCreateInfo.width = m_ChosenSwapChainDetails.m_VkExtent.width;
        framebufferCreateInfo.height = m_ChosenSwapChainDetails.m_VkExtent.height;
        framebufferCreateInfo.layers = 1;

        VkFramebuffer framebuffer;
        VkResult createFramebufferResult = vkCreateFramebuffer(m_VkDevice, &framebufferCreateInfo, nullptr, &framebuffer);
        if(createFramebufferResult != VK_SUCCESS) {
            DODO_CRITICAL("Could not create frame buffer");
        }

        m_VkFramebuffers.emplace_back(framebuffer);
    });

    VkCommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolCreateInfo.queueFamilyIndex = deviceQueues.m_QueueFamilyIndices.graphicsFamily.value();

    VkResult createCommandPoolResult = vkCreateCommandPool(m_VkDevice, &commandPoolCreateInfo, nullptr, &m_VkCommandPool);
    if(createCommandPoolResult != VK_SUCCESS) {
        DODO_CRITICAL("Could not create the command pool");
    }

    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = m_VkCommandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;

    VkResult allocateCommandBufferResult = vkAllocateCommandBuffers(m_VkDevice, &commandBufferAllocateInfo, &m_VkCommandBuffer);
    if(allocateCommandBufferResult != VK_SUCCESS) {
        DODO_CRITICAL("Could not allocate a command buffer");
    }

    VkSemaphoreCreateInfo semaphoreCreateInfo{};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkResult createImageAvailableSemaphoreResult = vkCreateSemaphore(m_VkDevice, &semaphoreCreateInfo, nullptr, &m_VkImageAvailable);
    VkResult createRenderFinishedSemaphoreResult = vkCreateSemaphore(m_VkDevice, &semaphoreCreateInfo, nullptr, &m_VkRenderFinished);
    VkResult createInFlightFenceResult = vkCreateFence(m_VkDevice, &fenceCreateInfo, nullptr, &m_VkInFlightFence);

    if(createImageAvailableSemaphoreResult != VK_SUCCESS || createRenderFinishedSemaphoreResult != VK_SUCCESS || createInFlightFenceResult != VK_SUCCESS) {
        DODO_CRITICAL("Could not create synchronisation objects");
    }

    DODO_INFO("Vulkan context initialized successfully");
}

void VulkanContext::Update()
{
    vkWaitForFences(m_VkDevice, 1, &m_VkInFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(m_VkDevice, 1, &m_VkInFlightFence);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(m_VkDevice, m_VkSwapChain, UINT64_MAX, m_VkImageAvailable, VK_NULL_HANDLE, &imageIndex);

    vkResetCommandBuffer(m_VkCommandBuffer, 0);

    RecordCommandBuffer(m_VkCommandBuffer, imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { m_VkImageAvailable };
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_VkCommandBuffer;

    VkSemaphore signalSemaphores[] = { m_VkRenderFinished };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    const Queues& deviceQueues = m_VulkanPhysicalDevice->GetQueues();
    VkResult submitQueueResult = vkQueueSubmit(deviceQueues.m_GraphicsFamilyQueue, 1, &submitInfo, m_VkInFlightFence);
    if(submitQueueResult != VK_SUCCESS) {
        DODO_CRITICAL("Could not submit the queue command buffer");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {m_VkSwapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(deviceQueues.m_PresentationQueue, &presentInfo);
}

void VulkanContext::RecordCommandBuffer(VkCommandBuffer _commandBuffer, uint32_t _imageIndex)
{
    VkCommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkResult beginCommandBufferResult = vkBeginCommandBuffer(m_VkCommandBuffer, &commandBufferBeginInfo);
    if(beginCommandBufferResult != VK_SUCCESS) {
        DODO_CRITICAL("Could not begin recording command buffer");
    }

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = m_VkRenderPass;
    renderPassBeginInfo.framebuffer = m_VkFramebuffers[_imageIndex];
    renderPassBeginInfo.renderArea.offset = {0, 0};
    renderPassBeginInfo.renderArea.extent = m_ChosenSwapChainDetails.m_VkExtent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(m_VkCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(m_VkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_VkPipeline);

        vkCmdSetViewport(m_VkCommandBuffer, 0, 1, &m_VkViewPort);
        vkCmdSetScissor(m_VkCommandBuffer, 0, 1, &m_VkScissor);

        vkCmdDraw(m_VkCommandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(m_VkCommandBuffer);

    VkResult endCommandBufferResult = vkEndCommandBuffer(m_VkCommandBuffer);
    if(endCommandBufferResult != VK_SUCCESS) {
        DODO_CRITICAL("Could not end the render pass");
    }
}

void VulkanContext::Shutdown() {
    
}

DODO_END_NAMESPACE
