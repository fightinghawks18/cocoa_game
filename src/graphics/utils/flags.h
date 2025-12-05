#pragma once

#include "../../common.h"

namespace Cocoa::Graphics
{
    enum class GPUShaderStage
    {
        Unknown = 0,
        Vertex  = 1 << 0,
        Pixel   = 1 << 1
    };

    inline bool operator&( GPUShaderStage a, GPUShaderStage b )
    {
        return static_cast<GPUShaderStage>( static_cast<u32>( a ) & static_cast<u32>( b ) ) != GPUShaderStage::Unknown;
    }

    inline GPUShaderStage operator|( GPUShaderStage a, GPUShaderStage b )
    {
        return static_cast<GPUShaderStage>( static_cast<u32>( a ) | static_cast<u32>( b ) );
    }

    enum class GPUBufferUsage
    {
        Unknown     = 0,
        Vertex      = 1 << 0,
        Index       = 1 << 1,
        Uniform     = 1 << 2,
        Storage     = 1 << 3,
        TransferSrc = 1 << 4,
        TransferDst = 1 << 5
    };

    inline bool operator&( GPUBufferUsage a, GPUBufferUsage b )
    {
        return static_cast<GPUBufferUsage>( static_cast<u32>( a ) & static_cast<u32>( b ) ) != GPUBufferUsage::Unknown;
    }

    inline GPUBufferUsage operator|( GPUBufferUsage a, GPUBufferUsage b )
    {
        return static_cast<GPUBufferUsage>( static_cast<u32>( a ) | static_cast<u32>( b ) );
    }

    enum class GPUTextureUsage
    {
        Unknown      = 0,
        RenderTarget = 1 << 0,
        DepthStencil = 1 << 1,
        ShaderUsage  = 1 << 2,
        Storage      = 1 << 3,
        TransferSrc  = 1 << 4,
        TransferDst  = 1 << 5
    };

    inline bool operator&( GPUTextureUsage a, GPUTextureUsage b )
    {
        return static_cast<GPUTextureUsage>( static_cast<u32>( a ) & static_cast<u32>( b ) ) !=
               GPUTextureUsage::Unknown;
    }

    inline GPUTextureUsage operator|( GPUTextureUsage a, GPUTextureUsage b )
    {
        return static_cast<GPUTextureUsage>( static_cast<u32>( a ) | static_cast<u32>( b ) );
    }

    enum class GPUSamplingCount
    {
        None    = 0,
        Count1  = 1 << 0,
        Count2  = 2 << 0,
        Count4  = 4 << 0,
        Count8  = 8 << 0,
        Count16 = 1 << 1,
        Count32 = 2 << 1,
        Count64 = 4 << 1
    };

    inline bool operator&( GPUSamplingCount a, GPUSamplingCount b )
    {
        return static_cast<GPUSamplingCount>( static_cast<u32>( a ) & static_cast<u32>( b ) ) != GPUSamplingCount::None;
    }

    inline GPUSamplingCount operator|( GPUSamplingCount a, GPUSamplingCount b )
    {
        return static_cast<GPUSamplingCount>( static_cast<u32>( a ) | static_cast<u32>( b ) );
    }

    enum class GPUTextureAspect
    {
        None    = 0,
        Color   = 1 << 0,
        Depth   = 1 << 1,
        Stencil = 1 << 2,
    };

    inline bool operator&( GPUTextureAspect a, GPUTextureAspect b )
    {
        return static_cast<GPUTextureAspect>( static_cast<u32>( a ) & static_cast<u32>( b ) ) != GPUTextureAspect::None;
    }

    inline GPUTextureAspect operator|( GPUTextureAspect a, GPUTextureAspect b )
    {
        return static_cast<GPUTextureAspect>( static_cast<u32>( a ) | static_cast<u32>( b ) );
    }
}
