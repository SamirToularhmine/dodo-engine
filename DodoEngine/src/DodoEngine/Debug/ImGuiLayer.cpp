#include <DodoEngine/Debug/ImGuiLayer.h>

#include <DodoEngine/Core/Window.h>
#include <DodoEngine/Debug/PerformanceManager.h>
#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Platform/Vulkan/VulkanDescriptorPool.h>
#include <DodoEngine/Platform/Vulkan/VulkanDevice.h>
#include <DodoEngine/Platform/Vulkan/VulkanInstance.h>
#include <DodoEngine/Platform/Vulkan/VulkanPhysicalDevice.h>
#include <DodoEngine/Platform/Vulkan/VulkanRenderPass.h>

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
  vulkanInitInfo.RenderPass = *vulkanContext.GetRenderPass();
  vulkanInitInfo.Subpass = 0;
  vulkanInitInfo.MinImageCount = VulkanContext::MAX_FRAMES_IN_FLIGHT;
  vulkanInitInfo.ImageCount = VulkanContext::MAX_FRAMES_IN_FLIGHT;
  vulkanInitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  ImGui_ImplVulkan_Init(&vulkanInitInfo);

  ImGui_ImplVulkan_CreateFontsTexture();
}

void ImGuiLayer::Update(const uint32_t& _frameIndex) const {
  const VulkanContext& vulkanContext = VulkanContext::Get();
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  bool performanceStatsOpened = true;
  ImGuiWindowFlags performanceStatsWindowFlags = 0;

  // Performance stats window
  {
    ImGui::SetNextWindowPos({0, 0});
    ImGui::Begin("Dodo Engine Performance stats", &performanceStatsOpened, performanceStatsWindowFlags);
    ImGui::Text("Frame time: %f ms", PerformanceManager::GetFrameTime());
    for (const auto& [_name, _time] : PerformanceManager::GetTraces()) {
      ImGui::Text("%s: %f ms", _name.c_str(), _time);
    }
    ImGui::End();
  }

  ImGui::ShowDemoWindow();  // Show demo window! :)

  ImGui::Render();
  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), vulkanContext.GetCommandBuffer(_frameIndex));

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
