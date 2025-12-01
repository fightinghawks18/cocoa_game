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

    enum class GPUBindGroupType {
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
        BGRA8Srgb,
        RG32Sfloat,
        RGB32Sfloat,
        RGBA32Sfloat,
        RGBA8Unorm
    };

    enum class GPUTextureDimension {
        One,
        Two,
        Three
    };

    enum class GPUTextureLayout {
        Unknown = 0,
        General,
        ColorAttachment,
        DepthStencilAttachment,
        DepthStencilReadOnly,
        ShaderReadOnly,
        TransferSrc,
        TransferDst,
        Present
    };

    enum class GPUTextureViewType {
        OneDimensional,
        TwoDimensional,
        ThreeDimensional,
        Cube
    };

    enum class GPUFilter {
        Linear,
        Nearest
    };

    enum class GPUMipmapMode {
        Linear,
        Nearest
    };

    enum class GPUWrappingMode {
        Repeat,
        ClampToEdge,
        ClampToBorder,
        MirroredRepeat
    };

    enum class GPUBufferAccess {
        CPUOnly,
        CPUToGPU,
        GPUToCPU,
        GPUOnly
    };
}