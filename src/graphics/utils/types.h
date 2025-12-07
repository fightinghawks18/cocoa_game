#pragma once

#include <array>
#include <vector>

#include "../../common.h"
#include "../../math/matrix4x4.h"

namespace Cocoa::Graphics {
    struct MVP
    {
        Math::Matrix4x4 model;
        Math::Matrix4x4 view;
        Math::Matrix4x4 projection;
    };

    struct Vertex
    {
        std::array<f32, 3> pos;
        std::array<f32, 4> col;
        std::array<f32, 2> uv;
    };

    struct MeshData
    {
        std::vector<Vertex> vertices;
        std::vector<uint16_t> indices;
    };

    struct Offset
    {
        int x, y;
    };

    struct Scale
    {
        u32 w, h;
    };

    struct Scale3D
    {
        u32 w, h, d;
    };

    struct Viewport
    {
        Offset offset;
        Scale scale;
        f32 minDepth, maxDepth;
    };

    using OutputTransform = Viewport;

    struct Rect
    {
        Offset offset;
        Scale scale;
    };

    using RenderArea = Rect;
} // namespace Cocoa::Graphics
