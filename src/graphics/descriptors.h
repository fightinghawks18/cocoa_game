#pragma once

#include <unordered_map>
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

    struct PipelineLayoutDesc {
        std::vector<BindGroupHandle> bindGroups;
    };

    struct PipelineVertexAttribute {
        GPUFormat format;
        uint32_t offset;
    };

    struct PipelineVertexBinding {
        uint32_t binding;
        uint32_t stride;
        std::vector<PipelineVertexAttribute> attributes;

        PipelineVertexBinding& Attribute(GPUFormat format, uint32_t offset) {
            attributes.push_back({
                .format = format,
                .offset = offset
            });
            return *this;
        }
    };

    struct PipelineDesc {
        std::unordered_map<GPUShaderStage, ShaderModuleHandle> shaders;
        std::vector<PipelineVertexBinding> vertexLayout;
        GPUTopology topology = GPUTopology::TriangleList;
        GPUCullMode cullMode = GPUCullMode::Backside;
        GPUPolygonMode polygonMode = GPUPolygonMode::Fill;
        GPUFrontFace frontFace = GPUFrontFace::CounterClockwise;
        GPUFormat colorFormat = GPUFormat::BGRA8_SRGB;
        GPUFormat depthFormat = GPUFormat::Unknown;
        GPUFormat stencilFormat = GPUFormat::Unknown;
        PipelineLayoutHandle pipelineLayout;

        void AddShader(GPUShaderStage stage, ShaderModuleHandle shaderModule) {
            shaders[stage] = shaderModule;
        }

        PipelineVertexBinding& Bind(uint32_t binding, uint32_t stride) {
            vertexLayout.push_back({
                .binding = binding,
                .stride = stride,
                .attributes = {}
            });
            return vertexLayout.back();
        }
    };
}