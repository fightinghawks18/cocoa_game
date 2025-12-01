#pragma once

#include <vector>
#include <SDL3/SDL.h>

#include "enums.h"
#include "handles.h"
#include "types.h"
#include "flags.h"

namespace Cocoa::Graphics {
    struct DeviceDesc {
        SDL_Window* window;
        std::vector<GPUQueueType> desiredQueues;
        GPUPowerPreference powerPreference;
    };

    struct SwapchainDesc {
        Graphics::SurfaceHandle surface;
    };

    struct BufferDesc {
        GPUBufferUsage usage;
        uint64_t size;
        void* mapped;
    };

    struct GPUColorPassDesc {
        TextureHandle texture;
        std::array<float, 4> clearColor;
        GPUPassLoadOp loadOp;
        GPUPassStoreOp storeOp;
    };

    struct GPUDepthPassDesc {
        TextureHandle texture;
        float depth;
        uint32_t stencil;
        GPUPassLoadOp loadOp;
        GPUPassStoreOp storeOp;
    };

    struct GPUPassDesc {
        std::vector<GPUColorPassDesc> colorPasses;
        const GPUDepthPassDesc* depthPass = nullptr;
        Rect renderArea;
        uint32_t viewMask;
        uint32_t layerCount;
    };

    struct BindGroupLayoutEntry {
        uint32_t binding;
        GPUShaderStage visibility;
        BindGroupType type;
    };

    struct BindGroupEntry {
        uint32_t binding;
        union {
            BufferHandle buffer;
            TextureHandle texture;
            SamplerHandle sampler;
        };
    };

    struct BindGroupLayoutDesc {
        std::vector<BindGroupLayoutEntry> entries;
    };

    struct BindGroupDesc {
        BindGroupLayoutDesc* layout;
        std::vector<BindGroupEntry> entries;
    };
}