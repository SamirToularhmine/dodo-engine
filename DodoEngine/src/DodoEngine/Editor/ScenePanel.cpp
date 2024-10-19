#pragma once

#include <DodoEngine/Core/Camera.h>
#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Editor/Component.h>
#include <DodoEngine/Editor/Scene.h>
#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Platform/Vulkan/VulkanDescriptorSet.h>
#include <DodoEngine/Platform/Vulkan/VulkanDescriptorSetLayout.h>
#include <DodoEngine/Platform/Vulkan/VulkanTextureImage.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <ImGuizmo.h>
#define VK_NO_PROTOTYPES
#include <volk.h>


DODO_BEGIN_NAMESPACE

namespace editor {
static bool SCENE_PANEL_OPENED = true;
static VkDescriptorSet s_ImguiDescriptorSet;

static uint32_t s_ScenePanelWidth = 0;
static uint32_t s_ScenePanelHeight = 0;

static ImGuizmo::OPERATION s_GuizmoMode = ImGuizmo::TRANSLATE;

static bool s_WindowResized = false;

static void InitScenePanel() {
  VulkanContext& vulkanContext = VulkanContext::Get();
  const VulkanTextureImage& viewportTextureImage = *vulkanContext.GetOffScreenTextureImage();

  s_ImguiDescriptorSet =
      ImGui_ImplVulkan_AddTexture(viewportTextureImage.GetSampler(), viewportTextureImage.GetImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

static void ShowScenePanel(Scene& _scene, const Camera& _camera) {
  if (ImGui::IsKeyPressed(ImGuiKey_T)) {
    s_GuizmoMode = ImGuizmo::TRANSLATE;
  }

  if (ImGui::IsKeyPressed(ImGuiKey_S)) {
    s_GuizmoMode = ImGuizmo::SCALE;
  }

  if (ImGui::IsKeyPressed(ImGuiKey_R)) {
    s_GuizmoMode = ImGuizmo::ROTATE;
  }

  // Show scene panel
  ImGui::Begin("Scene panel");
 
  ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
  
  ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
  s_ScenePanelWidth = viewportPanelSize.x;
  s_ScenePanelHeight = viewportPanelSize.y;

  ImGui::Image((ImTextureID)s_ImguiDescriptorSet, ImVec2{viewportPanelSize.x, viewportPanelSize.y});

  if ((viewportPanelSize.x != s_ScenePanelWidth || viewportPanelSize.y != s_ScenePanelHeight) && !s_WindowResized) {
    s_WindowResized = true;
  }

  Scene::Entities& sceneEntities = _scene.GetEntities();

  for (Scene::EntityPair& _entityPair : sceneEntities) {
    Entity& entity = _entityPair.second;
    const EditorEntity& entityId = _entityPair.first;

    if (entity.m_Selected) {
      TransformComponent& entityTransform = _scene.GetComponentFromEntity<TransformComponent>(entityId);

      float* transformMatrix = glm::value_ptr(entityTransform.m_TransformMatrix);
      const float* cameraViewMatrix = glm::value_ptr(_camera.GetViewMatrix());

      glm::mat4 cameraProjectionMatrix = _camera.GetProjectionMatrix();
      cameraProjectionMatrix[1][1] *= -1;
      float* cameraProjectionMatrixPtr = glm::value_ptr(cameraProjectionMatrix);

      ImGuiIO& io = ImGui::GetIO();
      const ImVec2& windowPos = ImGui::GetWindowPos();
      ImGuizmo::SetRect(windowPos.x, windowPos.y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

      ImGuizmo::Manipulate(cameraViewMatrix, cameraProjectionMatrixPtr, s_GuizmoMode, ImGuizmo::WORLD, transformMatrix, NULL, NULL);
    }
  }

  ImGui::End();
}
}  // namespace editor

DODO_END_NAMESPACE
