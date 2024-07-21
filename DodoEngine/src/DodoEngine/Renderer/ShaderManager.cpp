#include <DodoEngine/Renderer/ShaderManager.h>

#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Platform/Vulkan/VulkanDevice.h>

DODO_BEGIN_NAMESPACE

std::map<std::string, VkShaderModule> ShaderManager::s_Shaders{};

const VkShaderModule& ShaderManager::LoadShader(const char* _filePath) {
  if (s_Shaders.contains(_filePath)) {
    return s_Shaders[_filePath];
  }

  const VkDevice& vkDevice = *VulkanContext::Get().GetVulkanDevice();
  std::vector<char> shaderFile = readFile(_filePath);

  VkShaderModuleCreateInfo shaderModuleCreateInfo{};
  shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shaderModuleCreateInfo.codeSize = shaderFile.size();
  shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(shaderFile.data());

  VkResult shaderModuleCreateResult = vkCreateShaderModule(vkDevice, &shaderModuleCreateInfo, nullptr, &s_Shaders[_filePath]);

  if (shaderModuleCreateResult != VK_SUCCESS) {
    DODO_CRITICAL("Could not create shader module for shader at path {}", _filePath);
  }

  return s_Shaders[_filePath];
}

void ShaderManager::Shutdown() {
  const VkDevice& vkDevice = *VulkanContext::Get().GetVulkanDevice();

  for (const auto& [filePath, shader] : s_Shaders) {
    vkDestroyShaderModule(vkDevice, shader, nullptr);
  }
}

DODO_END_NAMESPACE
