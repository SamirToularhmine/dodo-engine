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
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <optional>

DODO_BEGIN_NAMESPACE

namespace editor
{
static bool SCENE_OBJECTS_OPENED = true;
static std::optional<EntityComponent> s_SelectedEntity;

static void ShowSceneObjects(Scene &_scene)
{
  if (ImGui::IsKeyPressed(ImGuiKey_Delete) && s_SelectedEntity.has_value())
  {
    _scene.DeleteEntity(s_SelectedEntity.value().m_EditorEntity);
    s_SelectedEntity.reset();
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
        const EditorEntity &cubeEntity = _scene.AddEntity("Entity");
        _scene.AddComponentToEntity<MeshComponent>(
            cubeEntity, {dodo::GltfLoader::LoadFromFile("/animated-cube/AnimatedCube.gltf")});
      }

      if (ImGui::MenuItem("Sphere"))
      {
        const EditorEntity &sphereEntity = _scene.AddEntity("Entity");
        _scene.AddComponentToEntity<MeshComponent>(
            sphereEntity, {dodo::GltfLoader::LoadFromFile("/sphere/sphere.gltf")});
      }

      ImGui::EndPopup();
    }

    _scene.GetAll<EntityComponent>().each([&](EntityComponent &_entityComponent) {
      const EditorEntity &editorEntity = _entityComponent.m_EditorEntity;
      Entity &entity = *_entityComponent.m_Entity;

      if (ImGui::Selectable(std::format("{}", entity.m_Name).c_str(), &entity.m_Selected))
      {
        if (entity.m_Selected)
        {
          s_SelectedEntity = _entityComponent;
        }
        else
        {
          s_SelectedEntity.reset();
        }
      }
    });

    ImGui::EndChild();
  }
  ImGui::End();
}
} // namespace editor

DODO_END_NAMESPACE
