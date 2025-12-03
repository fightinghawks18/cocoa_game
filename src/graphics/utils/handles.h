#pragma once

#include "../../tools/resource_manager.h"

namespace Cocoa::Graphics {
    using SwapchainHandle = Tools::Handle<struct SwapchainTag>;
    using SurfaceHandle = Tools::Handle<struct SurfaceTag>;
    using BufferHandle = Tools::Handle<struct BufferTag>;
    using TextureHandle = Tools::Handle<struct TextureTag>;
    using SamplerHandle = Tools::Handle<struct SamplerTag>;
    using BindGroupHandle = Tools::Handle<struct BindGroupTag>;
    using BindGroupLayoutHandle = Tools::Handle<struct BindGroupLayoutTag>;
    using PipelineLayoutHandle = Tools::Handle<struct PipelineLayoutTag>;
    using RenderPipelineHandle = Tools::Handle<struct RenderPipelineTag>;
    using ShaderModuleHandle = Tools::Handle<struct ShaderModuleTag>;
}