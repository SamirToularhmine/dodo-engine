#pragma once

#include <DodoEngine/Core/Camera.h>
#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Editor/Component/Component.h>
#include <DodoEngine/Editor/Editor.h>
#include <DodoEngine/Editor/Scene.h>
#include <DodoEngine/Renderer/Entity.h>

#include <ImGuizmo.h>
#include <imgui.h>
#include <imgui_impl_vulkan.h>

#include <algorithm>
#include <entt/entt.hpp>
#include <format>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <optional>

DODO_BEGIN_NAMESPACE

namespace editor
{
static bool SCENE_OBJECTS_OPENED = true;

static void ShowSceneObjects(Scene &_scene)
{
  const Ref<EntityComponent> &selectedEntity = _scene.GetSelectedEntity();
  if (ImGui::IsKeyPressed(ImGuiKey_Delete) && selectedEntity)
  {
    _scene.DeleteEntity(selectedEntity->m_EditorEntity);
  }

  // Show scene objects panel
  ImGui::Begin("Scene objects", &SCENE_OBJECTS_OPENED);
  {
    ImGui::BeginChild("Scrolling");

    if (ImGui::BeginPopupContextWindow("Create entity popup"))
    {
      ImGui::Text("Create a new entity");
      ImGui::Dummy(ImVec2(0, 5.0f));

      if (ImGui::MenuItem("Empty"))
      {
        _scene.AddEntity("Entity");
      }

      if (ImGui::MenuItem("Cube"))
      {
        _scene.AddComponentToEntity<MeshComponent>(
            _scene.AddEntity("Cube"),
            {dodo::GltfLoader::LoadFromFile("/animated-cube/AnimatedCube.gltf")});
      }

      if (ImGui::MenuItem("Sphere"))
      {
        _scene.AddComponentToEntity<MeshComponent>(
            _scene.AddEntity("Sphere"), {dodo::GltfLoader::LoadFromFile("/sphere/sphere.gltf")});
      }

      ImGui::EndPopup();
    }

    _scene.GetAll<EntityComponent, TransformComponent, MeshComponent>().each(
        [&](EntityComponent &_entityComponent, TransformComponent &_transformComponent,
            MeshComponent &_meshComponent) {
          const EditorEntity &editorEntity = _entityComponent.m_EditorEntity;
          Entity &entity = *_entityComponent.m_Entity;

          ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.025f, 0.5f));
          if (ImGui::Selectable(
                  std::format("{} #{}", entity.m_Name, static_cast<uint32_t>(editorEntity)).c_str(),
                  &entity.m_Selected, ImGuiSelectableFlags_None, ImVec2(0, 25)))
          {
            _scene.SelectEntity(_entityComponent);
          }
          ImGui::PopStyleVar();
        });

    ImGui::EndChild();
  }
  ImGui::End();
}
} // namespace editor

DODO_END_NAMESPACE
