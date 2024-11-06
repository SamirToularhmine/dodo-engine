#include <DodoEngine/Platform/Vulkan/VulkanInstance.h>
#include <DodoEngine/Utils/Log.h>

#include <GLFW/glfw3.h>

#include <vector>

VKAPI_ATTR VkBool32 VKAPI_CALL debug_utils_messenger_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT *callback_data, void *user_data)
{
  if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
  {
    DODO_INFO("{} - {}: {}", callback_data->messageIdNumber, callback_data->pMessageIdName,
              callback_data->pMessage);
  }
  else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
  {
    DODO_INFO("{} - {}: {}", callback_data->messageIdNumber, callback_data->pMessageIdName,
              callback_data->pMessage);
  }
  return VK_FALSE;
}

DODO_BEGIN_NAMESPACE

VulkanInstance::VulkanInstance(const VkApplicationInfo &_applicationInfo)
{
  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  m_InstanceExtensions.assign(glfwExtensions, glfwExtensions + glfwExtensionCount);
  m_InstanceExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
  m_InstanceExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  m_ValidationLayers.emplace_back("VK_LAYER_KHRONOS_validation");
  m_ValidationLayers.emplace_back("VK_LAYER_KHRONOS_synchronization2");

  VkDebugUtilsMessengerCreateInfoEXT debug_utils_create_info = {
      VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};

  debug_utils_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
  debug_utils_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
  debug_utils_create_info.pfnUserCallback = debug_utils_messenger_callback;

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &_applicationInfo;
  createInfo.enabledExtensionCount = m_InstanceExtensions.size();
  createInfo.ppEnabledExtensionNames = m_InstanceExtensions.data();
  createInfo.enabledLayerCount = m_ValidationLayers.size();
  createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
  createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
  createInfo.pNext = &debug_utils_create_info;

  VkResult vkCreateResult = vkCreateInstance(&createInfo, nullptr, &m_VkInstance);

  if (vkCreateResult != VK_SUCCESS)
  {
    DODO_CRITICAL("Failed to create Vulkan instance");
  }

  volkLoadInstance(m_VkInstance);

  vkCreateDebugUtilsMessengerEXT(m_VkInstance, &debug_utils_create_info, nullptr,
                                 &m_VulkanMessenger);
}

VulkanInstance::~VulkanInstance()
{
  vkDestroyDebugUtilsMessengerEXT(m_VkInstance, m_VulkanMessenger, nullptr);
  vkDestroyInstance(m_VkInstance, nullptr);
}

DODO_END_NAMESPACE