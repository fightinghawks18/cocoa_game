#pragma once

namespace Cocoa::Graphics {
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
}