#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Renderer/Vertex.h>

#include <vector>

DODO_BEGIN_NAMESPACE

static const std::vector<Vertex> QUAD_VERTICES = {
    Vertex{{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, Vertex{{1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
    Vertex{{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}, Vertex{{-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}};

const std::vector<uint32_t> QUAD_INDICES = {0, 1, 2, 2, 3, 0};

const std::vector<Vertex> CUBE_VERTICES = {
    // Face avant
    {{-1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},  // Sommet 0
    {{1.0f, -1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},   // Sommet 1
    {{1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},    // Sommet 2
    {{-1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},   // Sommet 3

    // Face arrière
    {{-1.0f, -1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},  // Sommet 4
    {{1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},   // Sommet 5
    {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},    // Sommet 6
    {{-1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},   // Sommet 7

    // Face gauche
    {{-1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},  // Sommet 0
    {{-1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},   // Sommet 3
    {{-1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},    // Sommet 7
    {{-1.0f, -1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},   // Sommet 4

    // Face droite
    {{1.0f, -1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},  // Sommet 1
    {{1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},   // Sommet 2
    {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},    // Sommet 6
    {{1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},   // Sommet 5

    // Face supérieure
    {{-1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},  // Sommet 3
    {{1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},   // Sommet 2
    {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},    // Sommet 6
    {{-1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},   // Sommet 7

    // Face inférieure
    {{-1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},  // Sommet 0
    {{1.0f, -1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},   // Sommet 1
    {{1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},    // Sommet 5
    {{-1.0f, -1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}}    // Sommet 4
};

const std::vector<uint32_t> CUBE_INDICES = {
    // Face avant
    0, 1, 2, 2, 3, 0,
    // Face arrière
    4, 5, 6, 6, 7, 4,
    // Face gauche
    8, 9, 10, 10, 11, 8,
    // Face droite
    12, 13, 14, 14, 15, 12,
    // Face supérieure
    16, 17, 18, 18, 19, 16,
    // Face inférieure
    20, 21, 22, 22, 23, 20};

DODO_END_NAMESPACE
