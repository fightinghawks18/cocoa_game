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

    using SwapchainHandle = Tools::Handle;
    using SurfaceHandle = Tools::Handle;
    using BufferHandle = Tools::Handle;
    using TextureHandle = Tools::Handle;
    using SamplerHandle = Tools::Handle;
    using BindGroupHandle = Tools::Handle;
    using PipelineLayoutHandle = Tools::Handle;
    using RenderPipelineHandle = Tools::Handle;
    using ShaderModuleHandle = Tools::Handle;
}