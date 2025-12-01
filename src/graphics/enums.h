#pragma once

namespace Cocoa::Graphics {
    enum class GPUPowerPreference {
        HighPerformance,
        LowPower,
    };

    enum class GPUQueueType {
        Unknown = 0,
        Graphics,
        Compute,
        Transfer
    };

    enum class GPUPassLoadOp {
        Clear,
        Load,
        DontCare
    };

    enum class GPUPassStoreOp {
        Store,
        DontCare
    };

    enum class BindGroupType {
        UniformBuffer,
        StorageBuffer,
        Texture,
        Sampler
    };

    enum class GPUTopology {
        TriangleList
    };

    enum class GPUPolygonMode {
        Fill,
        Line
    };

    enum class GPUCullMode {
        None = 0,
        Frontside,
        Backside,
        Both
    };
    
    enum class GPUFrontFace {
        Clockwise,
        CounterClockwise
    };

    enum class GPUFormat {
        Unknown = 0,
        BGRA8_SRGB,
        RGB32_SFLOAT,
        RGBA32_SFLOAT
    };
}