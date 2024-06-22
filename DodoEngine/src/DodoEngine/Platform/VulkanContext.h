#pragma once

#include <DodoEngine/Core/GraphicContext.h>
#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Platform/VulkanInstance.h>
#include <DodoEngine/Platform/VulkanPhysicalDevice.h>
#include <DodoEngine/Platform/VulkanSurface.h>
#include <DodoEngine/Utils/Utils.h>

#include <volk.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <optional>
#include <vector>


DODO_BEGIN_NAMESPACE

class VulkanContext : public GraphicContext {
    using SurfaceFormats = std::vector<VkSurfaceFormatKHR>;
    using PresentModes = std::vector<VkPresentModeKHR>;
    using SwapChainImages = std::vector<VkImage>;
    using ImageViews = std::vector<VkImageView>;
    using Framebuffers = std::vector<VkFramebuffer>;

    struct SwapChainDetails {
        VkSurfaceCapabilitiesKHR m_VkSurfaceCapabilities;
        SurfaceFormats m_VkSurfaceFormats;
        PresentModes m_VkPresentModes;

        const bool isFullFilled() const {
            return !m_VkSurfaceFormats.empty() && !m_VkPresentModes.empty();
        }
    };

    struct ChosenSwapChainDetails {
        VkSurfaceFormatKHR m_VkSurfaceFormat;
        VkExtent2D m_VkExtent;
        VkPresentModeKHR m_VkPresentMode;
    };

    public:
        void Init(GLFWwindow* _window) override;

        void Update() override;

        void Shutdown() override;

    private:
        void RecordCommandBuffer(VkCommandBuffer _commandBuffer, uint32_t _imageIndex);
        
    private:
        Ref<VulkanInstance> m_VulkanInstance;
        Ref<VulkanSurface> m_VulkanSurface;
        Ref<VulkanPhysicalDevice> m_VulkanPhysicalDevice;
        VkDevice m_VkDevice;
        SwapChainDetails m_SwapChainDetails;
        VkSwapchainKHR m_VkSwapChain;
        SwapChainImages m_VkSwapChainImages;
        ChosenSwapChainDetails m_ChosenSwapChainDetails;
        ImageViews m_VkImagesViews;
        VkPipelineLayout m_VkPipelineLayout;
        VkRenderPass m_VkRenderPass;
        VkPipeline m_VkPipeline;
        Framebuffers m_VkFramebuffers;
        VkCommandPool m_VkCommandPool;
        VkCommandBuffer m_VkCommandBuffer;
        VkSemaphore m_VkImageAvailable;
        VkSemaphore m_VkRenderFinished;
        VkFence m_VkInFlightFence;
        VkRect2D m_VkScissor;
        VkViewport m_VkViewPort;
};

DODO_END_NAMESPACE