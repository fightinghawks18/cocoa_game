#pragma once

#include <array>
#include <vector>

#include "../math/matrix4x4.h"

namespace Cocoa::Graphics {
    struct MVP {
        Math::Matrix4x4 model;
        Math::Matrix4x4 view;
        Math::Matrix4x4 projection;
    };

    struct Vertex {
        std::array<float, 3> pos;
        std::array<float, 4> col;
    };

    struct MeshData {
        std::vector<Vertex> vertices;
        std::vector<uint16_t> indices;
    };

    struct Offset {
        int x, y;
    };

    struct Extent {
        uint32_t w, h;
    };

    struct Extent3D {
        uint32_t w, h, d;
    };

    struct Viewport {
        Offset offset;
        Extent extent;
        float minDepth, maxDepth;
    };

    struct Rect {
        Offset offset;
        Extent extent;
    };
}