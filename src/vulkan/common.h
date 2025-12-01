#pragma once

#include <cstdint>
#include <vulkan/vulkan.hpp>

#include "../graphics/enums.h"
#include "../graphics/types.h"

namespace Cocoa::Vulkan {    
    struct GPUQueue {
        Graphics::GPUQueueType type;
        uint32_t family;
        vk::Queue queue;
    };

    inline Graphics::MeshData CreatePlaneIndexed() {
        Graphics::MeshData mesh;

        mesh.vertices = {
            {{-1.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}}, 
            {{ 1.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
            {{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},  
            {{ 1.0f,  1.0f, 0.0f}, {1.0f, 1.0f, 0.0f, 1.0f}}   
        };

        mesh.indices = {
            0, 1, 2,
            2, 1, 3
        };

        return mesh;
    }
}