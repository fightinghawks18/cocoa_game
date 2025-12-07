#pragma once

namespace Cocoa::Graphics {
    enum class GPUPowerPreference
    {
        HighPerformance,
        LowPower,
    };

    enum class GPUQueueType
    {
        Unknown = 0,
        Graphics,
        Compute,
        Transfer
    };

    enum class GPUPassLoadOp
    {
        Clear,
        Load,
        DontCare
    };

    enum class GPUPassStoreOp
    {
        Store,
        DontCare
    };

    enum class GPUBindGroupType
    {
        UniformBuffer,
        StorageBuffer,
        Texture,
        Sampler
    };

    enum class GPUTopology
    {
        TriangleList
    };

    enum class GPUPolygonMode
    {
        Fill,
        Line
    };

    enum class GPUCullMode
    {
        None = 0,
        Frontside,
        Backside,
        Both
    };

    enum class GPUFrontFace
    {
        Clockwise,
        CounterClockwise
    };

    enum class GPUColorFormat
    {
        Unknown = 0,
        BGRA8_SRGB,
        RG32_Float,
        RGB32_Float,
        RGBA32_Float,
        RGBA8_Unorm
    };

    enum class GPUDepthStencilFormat
    {
        Unknown = 0,
        DepthFloat32_NoStencil,
        DepthUnorm16_NoStencil,
        DepthUnorm24_StencilUint8,
        DepthFloat32_StencilUint8,
        DepthUnorm16_StencilUint8
    };

    enum class GPUTextureDimension
    {
        One,
        Two,
        Three
    };

    enum class GPUTextureState
    {
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

    enum class GPUTextureViewType
    {
        OneDimensional,
        TwoDimensional,
        ThreeDimensional,
        Cube
    };

    enum class GPUFilter
    {
        Linear,
        Nearest
    };

    enum class GPUMipmapMode
    {
        Linear,
        Nearest
    };

    enum class GPUWrappingMode
    {
        Repeat,
        ClampToEdge,
        ClampToBorder,
        MirroredRepeat
    };

    enum class GPUMemoryAccess
    {
        CPUOnly,
        CPUToGPU,
        GPUToCPU,
        GPUOnly
    };

    enum class GPUCompareOp
    {
        Never,
        Less,
        Equal,
        LessOrEqual,
        Greater,
        NotEqual,
        GreaterOrEqual,
        Always
    };

    enum class GPUStencilOp
    {
        Keep,
        Zero,
        Replace,
        IncrementAndClamp,
        DecrementAndClamp,
        Invert,
        IncrementAndWrap,
        DecrementAndWrap
    };

    inline std::string GPUQueueTypeAsString(const GPUQueueType queueType)
    {
        switch (queueType) {
        case GPUQueueType::Graphics: return "Graphics";
        case GPUQueueType::Compute:  return "Compute";
        case GPUQueueType::Transfer: return "Transfer";
        default:                     return "Unknown";
        }
    }
} // namespace Cocoa::Graphics
