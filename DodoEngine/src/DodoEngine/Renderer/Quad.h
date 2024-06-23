#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Renderer/Vertex.h>

#include <vector>


DODO_BEGIN_NAMESPACE

static const std::vector<Vertex> QUAD_VERTICES = {
	{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

const std::vector<uint16_t> QUAD_INDICES = {
    0, 1, 2, 2, 3, 0
};

DODO_END_NAMESPACE
