#include <DodoEngine/Core/Window.h>
#include <DodoEngine/Debug/PerformanceManager.h>
#include <DodoEngine/Editor/Editor.h>
#include <DodoEngine/Editor/EditorLayer.h>
#include <DodoEngine/Editor/Scene.h>
#include <DodoEngine/Platform/Vulkan/VulkanCommandBuffer.h>
#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Platform/Vulkan/VulkanDescriptorPool.h>
#include <DodoEngine/Platform/Vulkan/VulkanDevice.h>
#include <DodoEngine/Platform/Vulkan/VulkanInstance.h>
#include <DodoEngine/Platform/Vulkan/VulkanPhysicalDevice.h>
#include <DodoEngine/Platform/Vulkan/VulkanRenderPass.h>
#include <DodoEngine/Platform/Vulkan/VulkanRenderer.h>
#include <DodoEngine/Renderer/Renderer.h>

#include <ImGuizmo.h>
#include <imgui.h>

DODO_BEGIN_NAMESPACE

void EditorLayer::Init(const Window &_window)
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForVulkan(_window.GetNativeWindow(), true);

  const VulkanContext &vulkanContext = VulkanContext::Get();
  const Ref<VulkanDevice> vulkanDevice = vulkanContext.GetVulkanDevice();
  const Ref<VulkanPhysicalDevice> vulkanPhysicalDevice = vulkanContext.GetVulkanPhysicalDevice();

  ImGui_ImplVulkan_LoadFunctions(
      [](const char *function_name, void *vulkan_instance)
      {
        return vkGetInstanceProcAddr(*(reinterpret_cast<VkInstance *>(vulkan_instance)),
                                     function_name);
      },
      *vulkanContext.GetInstance());

  // Setup Platform/Renderer backends
  ImGui_ImplVulkan_InitInfo vulkanInitInfo = {};
  vulkanInitInfo.Instance = *vulkanContext.GetInstance();
  vulkanInitInfo.PhysicalDevice = *vulkanPhysicalDevice;
  vulkanInitInfo.Device = *vulkanDevice;
  vulkanInitInfo.QueueFamily =
      vulkanPhysicalDevice->GetQueues().m_QueueFamilyIndices.graphicsFamily.value();
  vulkanInitInfo.Queue = vulkanDevice->GetQueues().m_GraphicsFamilyQueue;
  vulkanInitInfo.DescriptorPool = *vulkanContext.GetDescriptorPool();
  vulkanInitInfo.RenderPass = *vulkanContext.GetUiRenderPass();
  vulkanInitInfo.Subpass = 0;
  vulkanInitInfo.MinImageCount = 2;
  vulkanInitInfo.ImageCount = 2;
  vulkanInitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

  ImGui_ImplVulkan_Init(&vulkanInitInfo);

  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  io.Fonts->AddFontFromFileTTF("fonts/satochi/Satoshi-Variable.ttf", 15.0f, NULL,
                               io.Fonts->GetGlyphRangesDefault());
  ImGui_ImplVulkan_CreateFontsTexture();

  InitTheme();

  editor::InitScenePanel();
}

void EditorLayer::Update(Frame &_frame, const Camera &_camera, Renderer &_renderer) const
{
  Scene &scene = *m_Scene;
  const RenderPass renderPass = _renderer.BeginUIRenderPass(_frame);

  if (renderPass.m_Frame.m_Error)
  {
    return;
  }

  // UI render pass
  {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
    ImGui::DockSpaceOverViewport(ImGuiDockNodeFlags_PassthruCentralNode);

    // Show editor
    {
      editor::ShowConsole();
      editor::ShowProfiler();
      editor::ShowScenePanel(scene, _camera);
      editor::ShowSceneObjects(scene);
      editor::ShowInspector(scene);
    }

    ImGui::ShowDemoWindow();

    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *_frame.m_CommandBuffer);
  }

  _renderer.EndUIRenderPass(renderPass);

  PerformanceManager::Clear();
}

void EditorLayer::Shutdown() const
{
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void EditorLayer::LoadScene(Ref<Scene> &_scene) { m_Scene = _scene; }

void EditorLayer::InitTheme()
{
  auto &colors = ImGui::GetStyle().Colors;
  colors[ImGuiCol_WindowBg] = ImVec4{0.1f, 0.1f, 0.13f, 1.0f};
  colors[ImGuiCol_MenuBarBg] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

  // Border
  colors[ImGuiCol_Border] = ImVec4{0.44f, 0.37f, 0.61f, 0.29f};
  colors[ImGuiCol_BorderShadow] = ImVec4{0.0f, 0.0f, 0.0f, 0.24f};

  // Text
  colors[ImGuiCol_Text] = ImVec4{1.0f, 1.0f, 1.0f, 1.0f};
  colors[ImGuiCol_TextDisabled] = ImVec4{0.5f, 0.5f, 0.5f, 1.0f};

  // Headers
  colors[ImGuiCol_Header] = ImVec4{0.13f, 0.13f, 0.17, 1.0f};
  colors[ImGuiCol_HeaderHovered] = ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
  colors[ImGuiCol_HeaderActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

  // Buttons
  colors[ImGuiCol_Button] = ImVec4{0.13f, 0.13f, 0.17, 1.0f};
  colors[ImGuiCol_ButtonHovered] = ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
  colors[ImGuiCol_ButtonActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
  colors[ImGuiCol_CheckMark] = ImVec4{0.74f, 0.58f, 0.98f, 1.0f};

  // Popups
  colors[ImGuiCol_PopupBg] = ImVec4{0.1f, 0.1f, 0.13f, 0.92f};

  // Slider
  colors[ImGuiCol_SliderGrab] = ImVec4{0.44f, 0.37f, 0.61f, 0.54f};
  colors[ImGuiCol_SliderGrabActive] = ImVec4{0.74f, 0.58f, 0.98f, 0.54f};

  // Frame BG
  colors[ImGuiCol_FrameBg] = ImVec4{0.13f, 0.13, 0.17, 1.0f};
  colors[ImGuiCol_FrameBgHovered] = ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
  colors[ImGuiCol_FrameBgActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

  // Tabs
  colors[ImGuiCol_Tab] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
  colors[ImGuiCol_TabHovered] = ImVec4{0.24, 0.24f, 0.32f, 1.0f};
  colors[ImGuiCol_TabActive] = ImVec4{0.2f, 0.22f, 0.27f, 1.0f};
  colors[ImGuiCol_TabUnfocused] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
  colors[ImGuiCol_TabUnfocusedActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

  // Title
  colors[ImGuiCol_TitleBg] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
  colors[ImGuiCol_TitleBgActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
  colors[ImGuiCol_TitleBgCollapsed] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

  // Scrollbar
  colors[ImGuiCol_ScrollbarBg] = ImVec4{0.1f, 0.1f, 0.13f, 1.0f};
  colors[ImGuiCol_ScrollbarGrab] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
  colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
  colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{0.24f, 0.24f, 0.32f, 1.0f};

  // Seperator
  colors[ImGuiCol_Separator] = ImVec4{0.44f, 0.37f, 0.61f, 1.0f};
  colors[ImGuiCol_SeparatorHovered] = ImVec4{0.74f, 0.58f, 0.98f, 1.0f};
  colors[ImGuiCol_SeparatorActive] = ImVec4{0.84f, 0.58f, 1.0f, 1.0f};

  // Resize Grip
  colors[ImGuiCol_ResizeGrip] = ImVec4{0.44f, 0.37f, 0.61f, 0.29f};
  colors[ImGuiCol_ResizeGripHovered] = ImVec4{0.74f, 0.58f, 0.98f, 0.29f};
  colors[ImGuiCol_ResizeGripActive] = ImVec4{0.84f, 0.58f, 1.0f, 0.29f};

  // Docking
  colors[ImGuiCol_DockingPreview] = ImVec4{0.44f, 0.37f, 0.61f, 1.0f};

  auto &style = ImGui::GetStyle();
  style.TabRounding = 0;
  style.ScrollbarRounding = 9;
  style.WindowRounding = 0;
  style.GrabRounding = 10;
  style.FrameRounding = 3;
  style.PopupRounding = 4;
  style.ChildRounding = 4;
  style.FramePadding = ImVec2(10, 10);
  style.SeparatorTextAlign = ImVec2(0.5f, 0.5f);
  style.SeparatorTextBorderSize = 5;
}

DODO_END_NAMESPACE
