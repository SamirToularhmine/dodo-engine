#pragma once

#include <DodoEngine/Core/Types.h>

#define VK_NO_PROTOTYPES
#include <volk.h>

#include <map>
#include <string>

DODO_BEGIN_NAMESPACE

class ShaderManager
{
 public:
  static const VkShaderModule& LoadShader(const char* _filePath);
  static void Shutdown();

 private:
  static std::map<std::string, VkShaderModule> s_Shaders;
};

DODO_END_NAMESPACE