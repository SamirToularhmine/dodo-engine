#include <DodoEngine/Core/Types.h>

#include <imgui.h>
#include <imgui_impl_vulkan.h>

DODO_BEGIN_NAMESPACE

namespace editor {
static bool SCENE_OBJECTS_PANEL_OPENED = true;

static void ShowSceneObjectsPanel() {

  // Show scene objects panel
  ImGui::Begin("Scene objects");
  {
    ImGui::BeginChild("Scrolling");
    for (int n = 0; n < 50; n++)
      ImGui::Text("%04d: Some text", n);
    ImGui::EndChild();
  }
  ImGui::End();
}
}  // namespace editor

DODO_END_NAMESPACE
