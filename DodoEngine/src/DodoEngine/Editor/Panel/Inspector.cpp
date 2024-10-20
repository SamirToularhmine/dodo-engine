#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Editor/Scene.h>
#include <DodoEngine/Editor/Component/Component.h>

#include <imgui.h>
#include <imgui_impl_vulkan.h>

#include <DodoEngine/Editor/Component/ComponentControl.cpp>
#include <DodoEngine/Editor/Panel/SceneObjects.cpp>

DODO_BEGIN_NAMESPACE

namespace editor
{
static bool INSPECTOR_OPENED = true;
extern std::optional<EntityComponent> s_SelectedEntity;

template <typename ComponentType>
static void ShowComponent(const EditorEntity &_editorEntity, Scene &_scene)
{
  ComponentType* component = _scene.TryGetComponentFromEntity<ComponentType>(_editorEntity);
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

    if (editor::s_SelectedEntity.has_value())
    {
      const EditorEntity &editorEntity = editor::s_SelectedEntity.value().m_EditorEntity;

      ShowComponent<EntityComponent>(editorEntity, _scene);
      ShowComponent<TransformComponent>(editorEntity, _scene);
      ShowComponent<MeshComponent>(editorEntity, _scene);
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
