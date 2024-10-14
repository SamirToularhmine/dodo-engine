#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Platform/Vulkan/VulkanDescriptorSet.h>
#include <DodoEngine/Platform/Vulkan/VulkanDescriptorSetLayout.h>

#include <imgui.h>
#include <imgui_impl_vulkan.h>


DODO_BEGIN_NAMESPACE

namespace editor 
{
    static bool SCENE_PANEL_OPENED = true;

    static VkDescriptorSet imguiDescriptorSet;

    static void InitScenePanel() {
      const VulkanContext& vulkanContext = VulkanContext::Get();
      const VulkanTextureImage& viewportTextureImage = vulkanContext.GetOffScreenTextureImage();
      imguiDescriptorSet = ImGui_ImplVulkan_AddTexture(viewportTextureImage.GetSampler(), viewportTextureImage.GetImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    static void ShowScenePanel() {
      ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

      // Show scene panel
      ImGui::Text("Scene Panel");
      ImGui::Image(imguiDescriptorSet, ImVec2{viewportPanelSize.x, viewportPanelSize.y});
    }
}

DODO_END_NAMESPACE
