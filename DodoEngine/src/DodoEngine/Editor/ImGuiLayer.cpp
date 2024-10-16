#include <DodoEngine/Editor/ImGuiLayer.h>

#include <DodoEngine/Core/Window.h>
#include <DodoEngine/Debug/PerformanceManager.h>
#include <DodoEngine/Editor/Editor.h>
#include <DodoEngine/Platform/Vulkan/VulkanCommandBuffer.h>
#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Platform/Vulkan/VulkanDescriptorPool.h>
#include <DodoEngine/Platform/Vulkan/VulkanDevice.h>
#include <DodoEngine/Platform/Vulkan/VulkanInstance.h>
#include <DodoEngine/Platform/Vulkan/VulkanPhysicalDevice.h>
#include <DodoEngine/Platform/Vulkan/VulkanRenderPass.h>
#include <DodoEngine/Platform/Vulkan/VulkanRenderer.h>
#include <DodoEngine/Renderer/Renderer.h>

#include <imgui.h>

DODO_BEGIN_NAMESPACE

void ImGuiLayer::Init(const Window& _window) const {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForVulkan(_window.GetNativeWindow(), true);

  const VulkanContext& vulkanContext = VulkanContext::Get();
  const Ref<VulkanDevice> vulkanDevice = vulkanContext.GetVulkanDevice();
  const Ref<VulkanPhysicalDevice> vulkanPhysicalDevice = vulkanContext.GetVulkanPhysicalDevice();

  // Setup Platform/Renderer backends
  ImGui_ImplVulkan_InitInfo vulkanInitInfo = {};
  vulkanInitInfo.Instance = *vulkanContext.GetInstance();
  vulkanInitInfo.PhysicalDevice = *vulkanPhysicalDevice;
  vulkanInitInfo.Device = *vulkanDevice;
  vulkanInitInfo.QueueFamily = vulkanPhysicalDevice->GetQueues().m_QueueFamilyIndices.graphicsFamily.value();
  vulkanInitInfo.Queue = vulkanDevice->GetQueues().m_GraphicsFamilyQueue;
  vulkanInitInfo.DescriptorPool = *vulkanContext.GetDescriptorPool();
  vulkanInitInfo.RenderPass = *vulkanContext.GetUiRenderPass();
  vulkanInitInfo.Subpass = 0;
  vulkanInitInfo.MinImageCount = VulkanContext::MAX_FRAMES_IN_FLIGHT;
  vulkanInitInfo.ImageCount = VulkanContext::MAX_FRAMES_IN_FLIGHT;
  vulkanInitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  ImGui_ImplVulkan_Init(&vulkanInitInfo);

  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  ImGui_ImplVulkan_CreateFontsTexture();

  editor::InitScenePanel();
}

void ImGuiLayer::Update(Frame& _frame, Renderer& _renderer) const {
  const VulkanContext& vulkanContext = VulkanContext::Get();

  if (editor::s_WindowResized) {
    VulkanContext& vulkanContext = VulkanContext::Get();
    vulkanContext.RescaleOffscreenTextureImage(editor::s_ScenePanelWidth, editor::s_ScenePanelHeight);
    editor::s_WindowResized = false;
  }

  const RenderPass renderPass = _renderer.BeginUIRenderPass(_frame);

  if (renderPass.m_Frame.m_Error) {
    return;
  }

  // UI render pass
  {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport(ImGuiDockNodeFlags_PassthruCentralNode);

    // Show editor
    {
      editor::ShowConsole();
      editor::ShowProfilerPanel();
      editor::ShowScenePanel();
      editor::ShowSceneObjectsPanel();
    }

    ImGui::ShowDemoWindow();

    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *_frame.m_CommandBuffer);
  }

  _renderer.EndUIRenderPass(renderPass);

  PerformanceManager::Clear();
}

void ImGuiLayer::Shutdown() const {
  const VulkanContext& vulkanContext = VulkanContext::Get();
  vkDeviceWaitIdle(*vulkanContext.GetVulkanDevice());

  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

DODO_END_NAMESPACE
