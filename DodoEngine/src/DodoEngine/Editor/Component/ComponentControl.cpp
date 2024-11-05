#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Editor/Component/Component.h>
#include <DodoEngine/Editor/Scene.h>
#include <DodoEngine/Renderer/Entity.h>
#include <DodoEngine/Renderer/GltfLoader.h>

#include <imgui.h>
#include <imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

DODO_BEGIN_NAMESPACE

namespace editor
{

static std::string s_TempEntityName;
static std::string s_TempEntityModelName;

bool Validate()
{
  return ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsItemDeactivatedAfterEdit();
}

template <typename ComponentType>
void ShowComponentControl(ComponentType &_component, Scene &_scene)
{
  ImGui::Text("No control panel registered for this component type");
}

template <>
void ShowComponentControl<EntityComponent>(EntityComponent &_entityComponent, Scene &_scene)
{
  Entity &entity = *_entityComponent.m_Entity;
  s_TempEntityName = entity.m_Name;

  ImGui::Text("Entity");
  ImGui::Dummy(ImVec2(0.0f, 5.0f));

  ImGui::BeginTable("entityPanelTable", 2, ImGuiTableFlags_SizingFixedFit);
  {
    ImGui::TableNextColumn();
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Name");

    ImGui::TableNextColumn();
    ImGui::PushItemWidth(-1);
    ImGui::InputText(entity.m_Name.c_str(), &s_TempEntityName);
    ImGui::PopItemWidth();
  }
  ImGui::EndTable();

  if (Validate() && !s_TempEntityName.empty())
  {
    entity.m_Name = s_TempEntityName;
  }
}

template <>
void ShowComponentControl<TransformComponent>(TransformComponent &_transformComponent,
                                              Scene &_scene)
{
  ImGui::Text("Transform");
  ImGui::Dummy(ImVec2(0.0f, 5.0f));

  ImGui::BeginTable("transformTable", 2, ImGuiTableFlags_SizingFixedFit);

  // Position
  {
    ImGui::TableNextColumn();
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Position");

    ImGui::TableNextColumn();
    ImGui::PushItemWidth(-1);
    ImGui::InputFloat3("positionInputFloat3", glm::value_ptr(_transformComponent.m_Position));
    ImGui::PopItemWidth();
  }

  // Rotation
  {
    ImGui::TableNextColumn();
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Rotation");

    ImGui::TableNextColumn();
    ImGui::PushItemWidth(-1);
    ImGui::InputFloat3("rotationInputFloat3", glm::value_ptr(_transformComponent.m_Rotation));
    ImGui::PopItemWidth();
  }

  // Scale
  {
    ImGui::TableNextColumn();
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Scale");

    ImGui::TableNextColumn();
    ImGui::PushItemWidth(-1);
    ImGui::InputFloat3("scaleInputFloat3", glm::value_ptr(_transformComponent.m_Scale));
    ImGui::PopItemWidth();
  }

  ImGui::EndTable();

  if (Validate() && !s_TempEntityName.empty())
  {
    _transformComponent.UpdateTransformMatrix();
  }
}

template <> void ShowComponentControl<MeshComponent>(MeshComponent &_meshComponent, Scene &_scene)
{
  bool removeComponent = true;

  if (ImGui::CollapsingHeader("Mesh", &removeComponent))
  {
    s_TempEntityModelName = _meshComponent.m_Model->m_ModelPath;

    ImGui::PushItemWidth(-1);
    ImGui::InputText("modelNameInputText", &s_TempEntityModelName);
    ImGui::PopItemWidth();
  }

  if (!removeComponent)
  {
    const Ref<EntityComponent> &selectedEntity = _scene.GetSelectedEntity();
    _scene.RemoveComponentFromEntity<MeshComponent>(selectedEntity->m_EditorEntity);
  }

  if (Validate() && !s_TempEntityModelName.empty())
  {
    _meshComponent.m_Model = dodo::GltfLoader::LoadFromFile(s_TempEntityModelName.c_str());
  }
}

} // namespace editor

DODO_END_NAMESPACE