#pragma once

#include <DodoEngine/Core/Types.h>

#include <volk.h>

#include <vector>


DODO_BEGIN_NAMESPACE

using VulkanExtensions = std::vector<const char*>;
using VulkanValidationLayers = std::vector<const char*>;

class VulkanInstance 
{
public:
    VulkanInstance(const VkApplicationInfo& _applicationInfo);
    ~VulkanInstance();

    const VulkanExtensions& GetInstanceExtensions() const { return m_InstanceExtensions; }
    const VulkanValidationLayers& GetValidationLayers() const { return m_ValidationLayers; }

    operator const VkInstance&() const { return m_VkInstance; }
private:
    VkInstance m_VkInstance;
    VulkanExtensions m_InstanceExtensions;
    VulkanValidationLayers m_ValidationLayers;
    VkDebugUtilsMessengerEXT m_VulkanMessenger;
};

DODO_END_NAMESPACE
