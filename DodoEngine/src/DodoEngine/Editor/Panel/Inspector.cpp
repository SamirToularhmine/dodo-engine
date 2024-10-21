#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Editor/Component/Component.h>
#include <DodoEngine/Editor/Scene.h>

#include <imgui.h>
#include <imgui_impl_vulkan.h>

#include <DodoEngine/Editor/Component/ComponentControl.cpp>
#include <DodoEngine/Editor/Panel/SceneObjects.cpp>

DODO_BEGIN_NAMESPACE

namespace editor
{
static bool INSPECTOR_OPENED = true;

template <typename ComponentType>
static void ShowComponent(const EditorEntity &_editorEntity, Scene &_scene)
{
  ComponentType *component = _scene.TryGetComponentFromEntity<ComponentType>(_editorEntity);
  if (component != nullptr)
  {
    ShowComponentControl<ComponentType>(*component, _scene);

    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
  }
}

static void ShowInspector(Scene &_scene)
{
  // Show inspector
  ImGui::Begin("Inspector", &INSPECTOR_OPENED);
  {
    ImGui::BeginChild("Scrolling");

    const Ref<EntityComponent> &selectedEntity = _scene.GetSelectedEntity();
    if (selectedEntity)
    {
      const EditorEntity &editorEntity = selectedEntity->m_EditorEntity;

      ShowComponent<EntityComponent>(editorEntity, _scene);
      ShowComponent<TransformComponent>(editorEntity, _scene);
      ShowComponent<MeshComponent>(editorEntity, _scene);

      if (ImGui::Button("+ Add a new component", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
      {
        ImGui::OpenPopup("CreateComponentPopup");
      }

      if (ImGui::BeginPopup("CreateComponentPopup"))
      {
        if (ImGui::Selectable("Mesh Component"))
        {
        }

        ImGui::EndPopup();
      }
    }
    else
    {
      ImGui::Text("No entity selected");
    }

    ImGui::EndChild();
  }
  ImGui::End();
}
} // namespace editor

DODO_END_NAMESPACE
