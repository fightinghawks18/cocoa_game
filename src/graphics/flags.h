#pragma once

#include <cstdint>

namespace Cocoa::Graphics {
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

    enum class GPUBufferUsage {
        Unknown = 0,
        Vertex = 1 << 0,
        Index = 1 << 1,
        Uniform = 1 << 2,
        Storage = 1 << 3,
    };

    inline bool operator&(GPUBufferUsage a, GPUBufferUsage b) {
        return static_cast<GPUBufferUsage>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b)) != GPUBufferUsage::Unknown;
    }

    inline GPUBufferUsage operator|(GPUBufferUsage a, GPUBufferUsage b) {
        return static_cast<GPUBufferUsage>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }
}