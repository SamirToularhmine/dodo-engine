#pragma once

#include <DodoEngine/Core/Types.h>

#include <DodoEngine/Core/Camera.h>
#include <DodoEngine/Editor/Component.h>
#include <DodoEngine/Editor/Editor.h>
#include <DodoEngine/Editor/Scene.h>
#include <DodoEngine/Renderer/Entity.h>

#include <ImGuizmo.h>
#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <format>

DODO_BEGIN_NAMESPACE

namespace editor {
static bool SCENE_OBJECTS_PANEL_OPENED = true;

static void ShowSceneObjectsPanel(Scene& _scene) {

  Scene::Entities& sceneEntities = _scene.GetEntities();

  // Show scene objects panel
  ImGui::Begin("Scene objects");
  {
    ImGui::BeginChild("Scrolling");

    for (Scene::EntityPair& _entityPair : sceneEntities) {
      const EditorEntity& entityId = _entityPair.first;
      Entity& entity = _entityPair.second;

      ImGui::Selectable(std::format("{} {}", entity.m_Name, static_cast<uint32_t>(_entityPair.first)).c_str(), &entity.m_Selected);
    }

    ImGui::EndChild();
  }
  ImGui::End();
}
}  // namespace editor

DODO_END_NAMESPACE
