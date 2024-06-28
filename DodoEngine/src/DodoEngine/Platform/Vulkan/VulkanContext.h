#pragma once

#include <DodoEngine/Core/GraphicContext.h>
#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Platform/Vulkan/VulkanDevice.h>
#include <DodoEngine/Platform/Vulkan/VulkanDescriptorSetLayout.h>
#include <DodoEngine/Platform/Vulkan/VulkanDescriptorSet.h>
#include <DodoEngine/Platform/Vulkan/VulkanDescriptorPool.h>
#include <DodoEngine/Platform/Vulkan/VulkanFrameBuffer.h>
#include <DodoEngine/Platform/Vulkan/VulkanGraphicPipeline.h>
#include <DodoEngine/Platform/Vulkan/VulkanInstance.h>
#include <DodoEngine/Platform/Vulkan/VulkanPhysicalDevice.h>
#include <DodoEngine/Platform/Vulkan/VulkanRenderPass.h>
#include <DodoEngine/Platform/Vulkan/VulkanSurface.h>
#include <DodoEngine/Platform/Vulkan/VulkanSwapChain.h>
#include <DodoEngine/Utils/Utils.h>

#include <volk.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <vector>


DODO_BEGIN_NAMESPACE

struct VulkanRenderPassData
{
    uint32_t m_FrameCount{ 0 };
    uint32_t m_ImageIndex{ 0 };
    uint32_t m_FrameIndex{ 0 };
    VkCommandBuffer m_CommandBuffer;
    VkFramebuffer m_FrameBuffer;
    VulkanSwapChainData m_SwapChainData;
    VkPipelineLayout m_PipelineLayout;
    Ref<VulkanDescriptorSet> m_DescriptorSet;
    bool m_RenderPassStarted{ false };
};

class VulkanContext : public GraphicContext {
    using Framebuffers = std::vector<VulkanFrameBuffer>;

public:
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

public:
        void Init(GLFWwindow* _window) override;

        void Update(uint32_t _frameId) override {}

        void Shutdown() override;

        void BeginRenderPass(VulkanRenderPassData& _vulkanRenderPassData);

        void EndRenderPass(const VulkanRenderPassData& _vulkanRenderPassData) const;

        const Ref<VulkanDevice>& GetVulkanDevice() const { return m_VulkanDevice; }
        const Ref<VulkanPhysicalDevice>& GetVulkanPhysicalDevice() const { return m_VulkanPhysicalDevice; }

		static VulkanContext& Get()
        {
            static VulkanContext instance;
            return instance;
        }

private:
    void DestroyFrameBuffers();

private:
        Ref<VulkanInstance> m_VulkanInstance;
        Ref<VulkanSurface> m_VulkanSurface;
        Ref<VulkanPhysicalDevice> m_VulkanPhysicalDevice;
        Ref<VulkanDevice> m_VulkanDevice;
        Ref<VulkanSwapChain> m_VulkanSwapChain;
        Ref<VulkanRenderPass> m_VulkanRenderPass;
        Ref<VulkanGraphicPipeline> m_VulkanGraphicPipeline;
        Ref<VulkanDescriptorPool> m_VulkanDescriptorPool;
        Ref<VulkanDescriptorSet> m_VulkanDescriptorSet;
        Ref<VulkanDescriptorSetLayout> m_VulkanDescriptorSetLayout;

        GLFWwindow* m_NativeWindow;
        Framebuffers m_VkFramebuffers;
        VkCommandPool m_VkCommandPool;
        std::vector<VkCommandBuffer> m_VkCommandBuffers;
        std::vector<VkSemaphore> m_VkImagesAvailable;
        std::vector<VkSemaphore> m_VkRenderFinishedSemaphores;
        std::vector<VkFence> m_VkInFlightFences;
        std::vector<VkDescriptorSetLayout> m_VkDescriptorSetLayouts;
};

DODO_END_NAMESPACE