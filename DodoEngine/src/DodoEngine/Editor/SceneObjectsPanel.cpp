#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Platform/Vulkan/VulkanDescriptorSet.h>
#include <DodoEngine/Platform/Vulkan/VulkanDescriptorSetLayout.h>

#include <imgui.h>
#include <imgui_impl_vulkan.h>

DODO_BEGIN_NAMESPACE

namespace editor {
static bool SCENE_OBJECTS_PANEL_OPENED = true;

static void ShowSceneObjectsPanel() {

  // Show scene panel
  ImGui::Begin("Scene objects");
  ImGui::End();
}
}  // namespace editor

DODO_END_NAMESPACE
