#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Platform/Vulkan/VulkanDescriptorSet.h>
#include <DodoEngine/Platform/Vulkan/VulkanDescriptorSetLayout.h>
#include <DodoEngine/Platform/Vulkan/VulkanTextureImage.h>

#define VK_NO_PROTOTYPES
#include <volk.h>

#include <imgui.h>
#include <imgui_impl_vulkan.h>

DODO_BEGIN_NAMESPACE

namespace editor {
static bool SCENE_PANEL_OPENED = true;
static VkDescriptorSet s_ImguiDescriptorSet;
static uint32_t s_ScenePanelWidth = 0;
static uint32_t s_ScenePanelHeight = 0;
static bool s_WindowResized = false;

static void InitScenePanel() {
  VulkanContext& vulkanContext = VulkanContext::Get();
  const VulkanTextureImage& viewportTextureImage = *vulkanContext.GetOffScreenTextureImage();

  s_ImguiDescriptorSet =
      ImGui_ImplVulkan_AddTexture(viewportTextureImage.GetSampler(), viewportTextureImage.GetImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

static void ShowScenePanel() {
  if (s_WindowResized) {
    s_WindowResized = false;
  }

  // Show scene panel
  ImGui::Begin("Scene panel");

  const ImVec2 windowSize = ImGui::GetWindowContentRegionMin();
  if (windowSize.x != s_ScenePanelWidth || windowSize.y != s_ScenePanelHeight) {
    s_ScenePanelWidth = windowSize.x;
    s_ScenePanelHeight = windowSize.y;
  }

  ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
  ImGui::Image((ImTextureID)s_ImguiDescriptorSet, ImVec2{viewportPanelSize.x, viewportPanelSize.y});

  ImGui::End();
}
}  // namespace editor

DODO_END_NAMESPACE
