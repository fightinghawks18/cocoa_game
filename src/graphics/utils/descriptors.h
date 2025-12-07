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
#include "../../common.h"

namespace Cocoa::Graphics
{
    struct RenderDeviceDesc
    {
        SDL_Window* window = nullptr;
        std::vector<GPUQueueType> desiredQueues;
        GPUPowerPreference powerPreference = GPUPowerPreference::HighPerformance;
    };

    struct RenderWindowDesc
    {
        SDL_Window* window;
    };

    struct RenderEncoderDesc
    {
        RenderWindowHandle* swapChain = nullptr;
        GPUQueueType submitQueue = GPUQueueType::Graphics;
    };

    struct GPUBufferDesc
    {
        GPUBufferUsage usage   = GPUBufferUsage::Unknown;
        GPUMemoryAccess access = GPUMemoryAccess::CPUToGPU;
        u64 size               = 0;
        void* mapped           = nullptr;
    };

    struct GPUColorPassDesc
    {
        GPUTextureHandle texture;
        u32 viewSlot                  = 0;
        std::array<f32 ,4> clearColor = { 0.0f , 0.0f , 0.0f , 1.0f };
        GPUPassLoadOp loadOp          = GPUPassLoadOp::Clear;
        GPUPassStoreOp storeOp        = GPUPassStoreOp::Store;
    };

    struct GPUDepthPassDesc
    {
        GPUTextureHandle texture;
        u32 viewSlot           = 0;
        f32 depth              = 1.0f;
        u32 stencil            = 0;
        bool useStencil        = false;
        GPUPassLoadOp loadOp   = GPUPassLoadOp::Clear;
        GPUPassStoreOp storeOp = GPUPassStoreOp::Store;
    };

    struct GPUPassDesc
    {
        std::vector<GPUColorPassDesc> colorPasses;
        const GPUDepthPassDesc* depthPass = nullptr;
        Rect renderArea;
        u32 viewMask   = 0;
        u32 layerCount = 1;
    };

    struct GPUBindGroupLayoutEntry
    {
        u32 binding = UINT32_MAX;
        GPUShaderStage visibility;
        GPUBindGroupType type;
    };

    struct GPUBindGroupLayoutDesc
    {
        std::vector<GPUBindGroupLayoutEntry> entries;
        u32 nextImplicitBinding = 0;

        GPUBindGroupLayoutDesc& Entry( GPUShaderStage visibility, GPUBindGroupType type, u32 binding = UINT32_MAX )
        {
            u32 actualBinding = ( binding == UINT32_MAX ) ? nextImplicitBinding : binding;
            entries.push_back( { actualBinding , visibility , type } );
            nextImplicitBinding = std::max( nextImplicitBinding , actualBinding + 1 );
            return *this;
        }
    };

    using GPUBindGroupEntry = std::variant<GPUBufferHandle ,GPUTextureHandle ,GPUSamplerHandle>;

    struct GPUBindGroupDesc
    {
        GPUBindGroupLayoutHandle layout;
        std::vector<GPUBindGroupEntry> entries;

        GPUBindGroupDesc& Entry( GPUBindGroupEntry entry )
        {
            entries.push_back( entry );
            return *this;
        }
    };

    struct GFXPipelineLayoutDesc
    {
        std::vector<GPUBindGroupLayoutHandle> groupLayouts;

        GFXPipelineLayoutDesc& BindGroup( GPUBindGroupLayoutHandle groupLayout )
        {
            groupLayouts.push_back( groupLayout );
            return *this;
        }
    };

    struct GFXPipelineVertexAttribute
    {
        GPUColorFormat format;
        u32 offset;
    };

    struct GFXPipelineVertexBinding
    {
        u32 binding;
        u32 stride;
        std::vector<GFXPipelineVertexAttribute> attributes;

        GFXPipelineVertexBinding& Attribute( GPUColorFormat format, u32 offset )
        {
            attributes.push_back( {
                                     .format = format ,
                                     .offset = offset
                                 } );
            return *this;
        }
    };

    struct GFXPipelineStencilOpState
    {
        GPUStencilOp failOp      = GPUStencilOp::Keep;
        GPUStencilOp passOp      = GPUStencilOp::Keep;
        GPUStencilOp depthFailOp = GPUStencilOp::Keep;
        GPUCompareOp compareOp   = GPUCompareOp::Always;
        u32 compareMask          = 0xFF;
        u32 writeMask            = 0xFF;
        u32 reference            = 0;
    };

    struct GFXPipelineDesc
    {
        std::unordered_map<GPUShaderStage ,GFXShaderModuleHandle> shaders;
        std::vector<GFXPipelineVertexBinding> vertexLayout;

        GPUTopology topology       = GPUTopology::TriangleList;
        GPUCullMode cullMode       = GPUCullMode::Backside;
        GPUPolygonMode polygonMode = GPUPolygonMode::Fill;
        GPUFrontFace frontFace     = GPUFrontFace::CounterClockwise;

        bool depthTest              = true;
        bool depthWrite             = true;
        GPUCompareOp depthCompareOp = GPUCompareOp::Less;

        bool stencilTest = false;
        GFXPipelineStencilOpState stencilFrontFace;
        GFXPipelineStencilOpState stencilBackFace;

        GPUColorFormat colorFormat               = GPUColorFormat::BGRA8_SRGB;
        GPUDepthStencilFormat depthStencilFormat = GPUDepthStencilFormat::Unknown;
        GFXPipelineLayoutHandle pipelineLayout;
        u32 nextImplicitBinding = 0;

        void Shader( GPUShaderStage stage, GFXShaderModuleHandle shaderModule )
        {
            shaders[ stage ] = shaderModule;
        }

        GFXPipelineVertexBinding& Binding( u32 stride, u32 binding = UINT32_MAX )
        {
            u32 actualBinding = ( binding == UINT32_MAX ) ? nextImplicitBinding : binding;
            vertexLayout.push_back( {
                                       .binding = actualBinding ,
                                       .stride = stride ,
                                       .attributes = { }
                                   } );
            nextImplicitBinding = std::max( nextImplicitBinding , actualBinding + 1 );
            return vertexLayout.back();
        }
    };

    struct GPUTextureDesc
    {
        GPUTextureDimension dimension = GPUTextureDimension::Two;
        GPUTextureUsage usage         = GPUTextureUsage::Unknown;
        GPUMemoryAccess access        = GPUMemoryAccess::CPUToGPU;
        std::variant<GPUColorFormat ,GPUDepthStencilFormat> format;
        GPUTextureLayout initialLayout = GPUTextureLayout::Unknown;
        GPUSamplingCount samples       = GPUSamplingCount::None;
        Scale3D scale                  = { };
        u32 levels                     = 1;
        u32 layers                     = 1;
        void* external                 = nullptr;
    };

    struct GPUTextureViewDesc
    {
        GPUTextureViewType type = GPUTextureViewType::TwoDimensional;
        std::variant<GPUColorFormat ,GPUDepthStencilFormat> format;
        GPUTextureAspect aspect = GPUTextureAspect::Color;
        u32 firstLevel          = 0;
        u32 levels              = 1;
        u32 firstLayer          = 0;
        u32 layers              = 1;
    };

    struct GFXShaderModuleDesc
    {
        std::string shaderPath;
    };

    struct GPUSamplerDesc
    {
        GPUFilter magnification            = GPUFilter::Linear;
        GPUFilter minification             = GPUFilter::Linear;
        GPUMipmapMode mipmapMode           = GPUMipmapMode::Linear;
        GPUWrappingMode horizontalWrapping = GPUWrappingMode::Repeat;
        GPUWrappingMode verticalWrapping   = GPUWrappingMode::Repeat;
        GPUWrappingMode depthWrapping      = GPUWrappingMode::Repeat;
        f32 minLevel                       = 0.0f;
        f32 maxLevel                       = 1000.0f;
        f32 levelBias                      = 0.0f;
    };
}
