#pragma once

#include <vector>
#include <cstdint>
#include <vulkan/vulkan.hpp>

#include "../math/matrix4x4.h"

namespace Cocoa::Vulkan {    
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

    struct Viewport {
        Offset offset;
        Extent extent;
        float minDepth, maxDepth;
    };

    struct Rect {
        Offset offset;
        Extent extent;
    };

    enum class GPUShaderStage {
        Unknown = 0,
        Vertex = 1 << 0,
        Pixel = 1 << 1
    };

    inline bool operator&(GPUShaderStage a, GPUShaderStage b) {
        return static_cast<GPUShaderStage>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b)) != GPUShaderStage::Unknown;
    }

    inline GPUShaderStage operator|(GPUShaderStage a, GPUShaderStage b) {
        return static_cast<GPUShaderStage>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

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

    inline vk::ShaderStageFlags GPUShaderStageToVk(GPUShaderStage stage) {
        vk::ShaderStageFlags flags;
        if (stage & GPUShaderStage::Vertex) flags |= vk::ShaderStageFlagBits::eVertex;
        if (stage & GPUShaderStage::Pixel) flags |= vk::ShaderStageFlagBits::eFragment;
        return flags;
    }

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