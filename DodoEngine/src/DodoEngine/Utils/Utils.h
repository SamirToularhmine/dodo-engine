#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Utils/Log.h>

#include <fstream>
#include <iostream>
#include <vector>


DODO_BEGIN_NAMESPACE

static std::vector<char> readFile(const char* filePath) {
    std::cout << std::filesystem::current_path().string() << std::endl;
    std::ifstream fileStream(filePath, std::ios::ate | std::ios::binary);


    if(!fileStream.is_open()) {
        DODO_CRITICAL("Could not open file");
    }

    size_t fileSize = fileStream.tellg();
    fileStream.seekg(0);

    std::vector<char> fileContent(fileSize);
    fileStream.read(fileContent.data(), fileSize);
    fileStream.close();

    return fileContent;
}

DODO_END_NAMESPACE