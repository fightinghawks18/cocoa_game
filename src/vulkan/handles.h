#pragma once

#include "../tools/resource_manager.h"

namespace Cocoa::Vulkan {
    class Swapchain;
    class Surface;
    class Buffer;
    class Texture;
    class Sampler;
    class BindGroup;
    class PipelineLayout;
    class RenderPipeline;
    class ShaderModule;

    using SwapchainHandle = Tools::Handle<Swapchain>;
    using SurfaceHandle = Tools::Handle<Surface>;
    using BufferHandle = Tools::Handle<Buffer>;
    using TextureHandle = Tools::Handle<Texture>;
    using SamplerHandle = Tools::Handle<Sampler>;
    using BindGroupHandle = Tools::Handle<BindGroup>;
    using PipelineLayoutHandle = Tools::Handle<PipelineLayout>;
    using RenderPipelineHandle = Tools::Handle<RenderPipeline>;
    using ShaderModuleHandle = Tools::Handle<ShaderModule>;
}