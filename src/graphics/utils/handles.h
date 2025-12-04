#pragma once

#include "../core/resource_manager.h"

namespace Cocoa::Graphics {
    using GFXWindowHandle = Handle<struct SwapchainTag>;
    using GFXBufferHandle = Handle<struct BufferTag>;
    using GFXTextureHandle = Handle<struct TextureTag>;
    using GFXSamplerHandle = Handle<struct SamplerTag>;
    using GFXBindGroupHandle = Handle<struct BindGroupTag>;
    using GFXBindGroupLayoutHandle = Handle<struct BindGroupLayoutTag>;
    using GFXPipelineLayoutHandle = Handle<struct PipelineLayoutTag>;
    using GFXRenderPipelineHandle = Handle<struct RenderPipelineTag>;
    using GFXShaderModuleHandle = Handle<struct ShaderModuleTag>;
}