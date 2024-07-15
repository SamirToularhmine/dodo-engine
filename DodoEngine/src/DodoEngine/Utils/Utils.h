#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Utils/ProfileTimer.h>
#include <DodoEngine/Utils/Log.h>

#include <filesystem>
#include <format>
#include <format>
#include <fstream>
#include <iostream>
#include <string>

DODO_BEGIN_NAMESPACE

static std::vector<char> readFile(const char* filePath) {
  std::cout << std::filesystem::current_path().string() << std::endl;
  std::ifstream fileStream(filePath, std::ios::ate | std::ios::binary);

  if (!fileStream.is_open()) {
    DODO_CRITICAL("Could not open file at path {}", filePath);
  }

  size_t fileSize = fileStream.tellg();
  fileStream.seekg(0);

  std::vector<char> fileContent(fileSize);
  fileStream.read(fileContent.data(), fileSize);
  fileStream.close();

  return fileContent;
}

 #define DODO_TRACE(...) ProfileTimer _timer{std::format("{}::{}", #__VA_ARGS__, __func__).c_str()};

DODO_END_NAMESPACE