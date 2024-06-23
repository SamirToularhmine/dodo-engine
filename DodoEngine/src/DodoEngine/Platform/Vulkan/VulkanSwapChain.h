#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Platform/Vulkan/VulkanSurface.h>
#include <DodoEngine/Platform/Vulkan/VulkanPhysicalDevice.h>
#include <DodoEngine/Platform/Vulkan/VulkanDevice.h>

#include <volk.h>


DODO_BEGIN_NAMESPACE

struct VulkanSwapChainData {
    VkSurfaceFormatKHR m_VkSurfaceFormat;
    VkExtent2D m_VkExtent;
    VkPresentModeKHR m_VkPresentMode;
};

using ImageViews = std::vector<VkImageView>;

class VulkanSwapChain
{
    using SurfaceFormats = std::vector<VkSurfaceFormatKHR>;
    using PresentModes = std::vector<VkPresentModeKHR>;
    using SwapChainImages = std::vector<VkImage>;

    struct SwapChainDetails {
        VkSurfaceCapabilitiesKHR m_VkSurfaceCapabilities;
        SurfaceFormats m_VkSurfaceFormats;
        PresentModes m_VkPresentModes;

        const bool isFullFilled() const {
            return !m_VkSurfaceFormats.empty() && !m_VkPresentModes.empty();
        }
    };

public:
    VulkanSwapChain(const VulkanSurface& _vulkanSurface, const VulkanPhysicalDevice& _vulkanPhysicalDevice, Ref<VulkanDevice> _vulkanDevice, GLFWwindow* _window);
    ~VulkanSwapChain();

    const VulkanSwapChainData& GetSpec() { return m_ChosenSwapChainDetails; }

    const ImageViews& GetImagesViews() { return m_ImageViews; }

    operator const VkSwapchainKHR& () const { return m_VkSwapChain; }
private:
    VkSwapchainKHR m_VkSwapChain;
    Ref<VulkanDevice> m_VulkanDevice;
    SwapChainDetails m_Spec;
    SwapChainImages m_Images;
    ImageViews m_ImageViews;
    VulkanSwapChainData m_ChosenSwapChainDetails;
};

DODO_END_NAMESPACE
