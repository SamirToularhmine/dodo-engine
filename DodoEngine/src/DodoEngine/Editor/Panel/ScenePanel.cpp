#pragma once

#include <DodoEngine/Core/Camera.h>
#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Editor/Component/Component.h>
#include <DodoEngine/Editor/Scene.h>
#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Platform/Vulkan/VulkanDescriptorSet.h>
#include <DodoEngine/Platform/Vulkan/VulkanDescriptorSetLayout.h>
#include <DodoEngine/Platform/Vulkan/VulkanTextureImage.h>

#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <ImGuizmo.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#define VK_NO_PROTOTYPES
#include <volk.h>

DODO_BEGIN_NAMESPACE

namespace editor
{
static bool SCENE_PANEL_OPENED = true;
static VkDescriptorSet s_ImguiDescriptorSet;

static uint32_t s_ScenePanelWidth = 0;
static uint32_t s_ScenePanelHeight = 0;

static ImGuizmo::OPERATION s_GuizmoMode = ImGuizmo::TRANSLATE;

static bool s_WindowResized = false;

static void InitScenePanel()
{
  VulkanContext &vulkanContext = VulkanContext::Get();
  const VulkanTextureImage &viewportTextureImage = *vulkanContext.GetOffScreenTextureImage();

  s_ImguiDescriptorSet = ImGui_ImplVulkan_AddTexture(viewportTextureImage.GetSampler(),
                                                     viewportTextureImage.GetImageView(),
                                                     VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

static void ShowScenePanel(Scene &_scene, const Camera &_camera)
{
  // Show scene panel
  ImGui::Begin("Scene panel", &SCENE_PANEL_OPENED);

  if (ImGui::IsKeyPressed(ImGuiKey_T))
  {
    s_GuizmoMode = ImGuizmo::TRANSLATE;
  }

  if (ImGui::IsKeyPressed(ImGuiKey_S))
  {
    s_GuizmoMode = ImGuizmo::SCALE;
  }

  if (ImGui::IsKeyPressed(ImGuiKey_R))
  {
    s_GuizmoMode = ImGuizmo::ROTATE;
  }

  ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());

  ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
  s_ScenePanelWidth = viewportPanelSize.x;
  s_ScenePanelHeight = viewportPanelSize.y;

  ImGui::Image((ImTextureID)s_ImguiDescriptorSet, ImVec2{viewportPanelSize.x, viewportPanelSize.y});

  if ((viewportPanelSize.x != s_ScenePanelWidth || viewportPanelSize.y != s_ScenePanelHeight) &&
      !s_WindowResized)
  {
    s_WindowResized = true;
  }

  _scene.GetAll<EntityComponent>().each([&](EntityComponent &_entityComponent) {
    const EditorEntity &editorEntity = _entityComponent.m_EditorEntity;
    Entity &entity = *_entityComponent.m_Entity;
    TransformComponent *entityTransform =
        _scene.TryGetComponentFromEntity<TransformComponent>(editorEntity);

    if (entity.m_Selected && entityTransform != nullptr)
    {
      float *transformMatrix = glm::value_ptr(entityTransform->m_TransformMatrix);
      const float *cameraViewMatrix = glm::value_ptr(_camera.GetViewMatrix());

      glm::mat4 cameraProjectionMatrix = _camera.GetProjectionMatrix();
      cameraProjectionMatrix[1][1] *= -1;
      float *cameraProjectionMatrixPtr = glm::value_ptr(cameraProjectionMatrix);

      ImGuiIO &io = ImGui::GetIO();
      const ImVec2 &windowPos = ImGui::GetWindowPos();
      ImGuizmo::SetRect(windowPos.x, windowPos.y, ImGui::GetWindowWidth(),
                        ImGui::GetWindowHeight());

      ImGuizmo::Manipulate(cameraViewMatrix, cameraProjectionMatrixPtr, s_GuizmoMode,
                           ImGuizmo::WORLD, transformMatrix, NULL, NULL);

      if (ImGuizmo::IsUsing())
      {
        float newPos[3];
        float newRot[3];
        float newScale[3];
        ImGuizmo::DecomposeMatrixToComponents(transformMatrix, newPos, newRot, newScale);

        entityTransform->m_Position = {newPos[0], newPos[1], newPos[2]};
        entityTransform->m_Rotation = {newRot[0], newRot[1], newRot[2]};
        entityTransform->m_Scale = {newScale[0], newScale[1], newScale[2]};
      }
    }
  });

  ImGui::End();
}
} // namespace editor

DODO_END_NAMESPACE
