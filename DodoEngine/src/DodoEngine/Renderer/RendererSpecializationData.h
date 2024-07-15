#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Renderer/Renderer.h>

DODO_BEGIN_NAMESPACE

struct RendererSpecializationData
{
    uint32_t MAX_MODEL_COUNT{ Renderer::MAX_MODEL_COUNT };

    std::vector<VkSpecializationMapEntry> GetMapEntries()
    {
        VkSpecializationMapEntry vertexShaderStageMapEntries{};
        vertexShaderStageMapEntries.size = sizeof(uint32_t);
        vertexShaderStageMapEntries.offset = 0;
        vertexShaderStageMapEntries.constantID = 0;

        return {vertexShaderStageMapEntries};
    }
};

DODO_END_NAMESPACE