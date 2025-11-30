#pragma once

#include <vector>
#include <cstdint>
#include <vulkan/vulkan.hpp>

namespace Cocoa::Vulkan {
    struct Vertex {
        std::array<float, 3> pos;
        std::array<float, 4> col;
    };

    struct MeshData {
        std::vector<Vertex> vertices;
        std::vector<uint16_t> indices;
    };

    enum class GPUPowerPreference {
        HighPerformance,
        LowPower,
    };

    enum class GPUQueueType {
        Unknown,
        Graphics,
        Compute,
        Transfer
    };

    struct GPUQueue {
        GPUQueueType type;
        uint32_t family;
        vk::Queue queue;
    };

    inline std::string GPUQueueTypeToString(GPUQueueType type) {
        switch (type) {
        case GPUQueueType::Unknown:
            return "Unknown";
        case GPUQueueType::Graphics:
            return "Graphics";
        case GPUQueueType::Compute:
            return "Compute";
        case GPUQueueType::Transfer:
            return "Transfer";
        }
    }

    inline MeshData CreatePlaneIndexed() {
        MeshData mesh;

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