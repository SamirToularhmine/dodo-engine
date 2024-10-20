#pragma once

#include <DodoEngine/Core/Types.h>

#include <imgui.h>
#include <imgui_impl_vulkan.h>

DODO_BEGIN_NAMESPACE

namespace editor
{
static bool CONSOLE_PANEL_OPENED = true;

static void ShowConsole()
{

  // Show console panel
  ImGui::Begin("Console");
  {
  }
  ImGui::End();
}
} // namespace editor

DODO_END_NAMESPACE
