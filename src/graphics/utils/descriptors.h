#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <SDL3/SDL.h>

#include "enums.h"
#include "handles.h"
#include "types.h"
#include "flags.h"

namespace Cocoa::Graphics {
    struct DeviceDesc {
        SDL_Window* window = nullptr;
        std::vector<GPUQueueType> desiredQueues;
        GPUPowerPreference powerPreference = GPUPowerPreference::HighPerformance;
    };

    struct SwapchainDesc {
        Graphics::SurfaceHandle surface;
    };

    struct BufferDesc {
        GPUBufferUsage usage = GPUBufferUsage::Unknown;
        GPUMemoryAccess access = GPUMemoryAccess::CPUToGPU;
        uint64_t size = 0;
        void* mapped = nullptr;
    };

    struct GPUColorPassDesc {
        TextureHandle texture;
        uint32_t viewSlot = 0;
        std::array<float, 4> clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
        GPUPassLoadOp loadOp = GPUPassLoadOp::Clear;
        GPUPassStoreOp storeOp = GPUPassStoreOp::Store;
    };

    struct GPUDepthPassDesc {
        TextureHandle texture;
        uint32_t viewSlot = 0;
        float depth = 1.0f;
        uint32_t stencil = 0;
        bool useStencil = false;
        GPUPassLoadOp loadOp = GPUPassLoadOp::Clear;
        GPUPassStoreOp storeOp = GPUPassStoreOp::Store;
    };

    struct GPUPassDesc {
        std::vector<GPUColorPassDesc> colorPasses;
        const GPUDepthPassDesc* depthPass = nullptr;
        Rect renderArea;
        uint32_t viewMask = 0;
        uint32_t layerCount = 1;
    };

    struct BindGroupLayoutEntry {
        uint32_t binding = UINT32_MAX;
        GPUShaderStage visibility;
        GPUBindGroupType type;
    };

    struct BindGroupLayoutDesc {
        std::vector<BindGroupLayoutEntry> entries;
        uint32_t nextImplicitBinding = 0;

        BindGroupLayoutDesc& Entry(GPUShaderStage visibility, GPUBindGroupType type, uint32_t binding = UINT32_MAX) {
            uint32_t actualBinding = (binding == UINT32_MAX) ? nextImplicitBinding : binding;
            entries.push_back({actualBinding, visibility, type});
            nextImplicitBinding = std::max(nextImplicitBinding, actualBinding + 1);
            return *this;
        }
    };

    using BindGroupEntry = std::variant<BufferHandle, TextureHandle, SamplerHandle>;
    struct BindGroupDesc {
        BindGroupLayoutHandle layout;
        std::vector<BindGroupEntry> entries;

        BindGroupDesc& Entry(BindGroupEntry entry) {
            entries.push_back(entry);
            return *this;
        }
    };

    struct PipelineLayoutDesc {
        std::vector<BindGroupLayoutHandle> groupLayouts;

        PipelineLayoutDesc& BindGroup(BindGroupLayoutHandle groupLayout) {
            groupLayouts.push_back(groupLayout);
            return *this;
        }
    };

    struct PipelineVertexAttribute {
        GPUColorFormat format;
        uint32_t offset;
    };

    struct PipelineVertexBinding {
        uint32_t binding;
        uint32_t stride;
        std::vector<PipelineVertexAttribute> attributes;

        PipelineVertexBinding& Attribute(GPUColorFormat format, uint32_t offset) {
            attributes.push_back({
                .format = format,
                .offset = offset
            });
            return *this;
        }
    };

    struct PipelineStencilOpState {
        GPUStencilOp failOp = GPUStencilOp::Keep;
        GPUStencilOp passOp = GPUStencilOp::Keep;
        GPUStencilOp depthFailOp = GPUStencilOp::Keep;
        GPUCompareOp compareOp = GPUCompareOp::Always;
        uint32_t compareMask = 0xFF;
        uint32_t writeMask = 0xFF;
        uint32_t reference = 0;
    };

    struct PipelineDesc {
        std::unordered_map<GPUShaderStage, ShaderModuleHandle> shaders;
        std::vector<PipelineVertexBinding> vertexLayout;

        GPUTopology topology = GPUTopology::TriangleList;
        GPUCullMode cullMode = GPUCullMode::Backside;
        GPUPolygonMode polygonMode = GPUPolygonMode::Fill;
        GPUFrontFace frontFace = GPUFrontFace::CounterClockwise;

        bool depthTest = true;
        bool depthWrite = true;
        GPUCompareOp depthCompareOp = GPUCompareOp::Less;

        bool stencilTest = false;
        PipelineStencilOpState stencilFrontFace;
        PipelineStencilOpState stencilBackFace;

        GPUColorFormat colorFormat = GPUColorFormat::BGRA8_SRGB;
        GPUDepthStencilFormat depthStencilFormat = GPUDepthStencilFormat::Unknown;
        PipelineLayoutHandle pipelineLayout;
        uint32_t nextImplicitBinding = 0;

        void Shader(GPUShaderStage stage, ShaderModuleHandle shaderModule) {
            shaders[stage] = shaderModule;
        }

        PipelineVertexBinding& Binding(uint32_t stride, uint32_t binding = UINT32_MAX) {
            uint32_t actualBinding = (binding == UINT32_MAX) ? nextImplicitBinding : binding;
            vertexLayout.push_back({
                .binding = actualBinding,
                .stride = stride,
                .attributes = {}
            });
            nextImplicitBinding = std::max(nextImplicitBinding, actualBinding + 1);
            return vertexLayout.back();
        }
    };

    struct TextureDesc {
        GPUTextureDimension dimension = GPUTextureDimension::Two;
        GPUTextureUsage usage = GPUTextureUsage::Unknown;
        GPUMemoryAccess access = GPUMemoryAccess::CPUToGPU;
        std::variant<GPUColorFormat, GPUDepthStencilFormat> format;
        GPUTextureLayout initialLayout = GPUTextureLayout::Unknown;
        GPUSamplingCount samples = GPUSamplingCount::None;
        Extent3D extent = {};
        uint32_t levels = 1;
        uint32_t layers = 1;
        void* external = nullptr;
    };

    struct TextureViewDesc {
        GPUTextureViewType type = GPUTextureViewType::TwoDimensional;
        std::variant<GPUColorFormat, GPUDepthStencilFormat> format;
        GPUTextureAspect aspect = GPUTextureAspect::Color;
        uint32_t firstLevel = 0;
        uint32_t levels = 1;
        uint32_t firstLayer = 0;
        uint32_t layers = 1;
    };

    struct ShaderModuleDesc {
        std::string shaderPath;
    };

    struct SamplerDesc {
        GPUFilter magnification = GPUFilter::Linear;
        GPUFilter minification = GPUFilter::Linear;
        GPUMipmapMode mipmapMode = GPUMipmapMode::Linear;
        GPUWrappingMode horizontalWrapping = GPUWrappingMode::Repeat;
        GPUWrappingMode verticalWrapping = GPUWrappingMode::Repeat;
        GPUWrappingMode depthWrapping = GPUWrappingMode::Repeat;
        float minLevel = 0.0f;
        float maxLevel = 1000.0f;
        float levelBias = 0.0f;
    };
}