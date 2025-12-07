//
// Created by fightinghawks18 on 12/7/2025.
//

#include "render_device_impl.h"

namespace Cocoa::Vulkan {
    RenderDeviceImpl::RenderDeviceImpl(const Graphics::RenderDeviceDesc& desc) {}

    RenderDeviceImpl::~RenderDeviceImpl() {}

    Graphics::RenderWindowHandle RenderDeviceImpl::ConnectWindow(const Graphics::RenderWindowDesc& desc) {}

    Graphics::GPUBufferHandle RenderDeviceImpl::CreateBuffer(const Graphics::GPUBufferDesc& desc) {}

    Graphics::GPUTextureHandle RenderDeviceImpl::CreateTexture(const Graphics::GPUTextureDesc& desc) {}

    Graphics::GPUTextureViewHandle RenderDeviceImpl::CreateTextureView(const Graphics::GPUTextureViewDesc& desc) {}

    Graphics::GPUSamplerHandle RenderDeviceImpl::CreateSampler(const Graphics::GPUSamplerDesc& desc) {}

    Graphics::GPUBindGroupHandle RenderDeviceImpl::CreateBindGroup(const Graphics::GPUBindGroupDesc& desc) {}

    Graphics::GPUBindGroupLayoutHandle
    RenderDeviceImpl::CreateBindGroupLayout(const Graphics::GPUBindGroupLayoutDesc& desc)
    {
    }

    Graphics::GFXRenderPipelineHandle RenderDeviceImpl::CreateRenderPipeline(const Graphics::GFXPipelineDesc& desc) {}

    Graphics::GFXPipelineLayoutHandle
    RenderDeviceImpl::CreatePipelineLayout(const Graphics::GFXPipelineLayoutDesc& desc)
    {
    }

    Graphics::GFXShaderModuleHandle RenderDeviceImpl::CreateShaderModule(const Graphics::GFXShaderModuleDesc& desc) {}

    void RenderDeviceImpl::DisconnectWindow(Graphics::RenderWindowHandle& handle) {}

    void RenderDeviceImpl::DestroyBuffer(Graphics::GPUBufferHandle& handle) {}

    void RenderDeviceImpl::DestroyTexture(Graphics::GPUTextureHandle& handle) {}

    void RenderDeviceImpl::DestroyTextureView(Graphics::GPUTextureViewHandle& handle) {}

    void RenderDeviceImpl::DestroySampler(Graphics::GPUSamplerHandle& handle) {}

    void RenderDeviceImpl::DestroyBindGroup(Graphics::GPUBindGroupHandle& handle) {}

    void RenderDeviceImpl::DestroyBindGroupLayout(Graphics::GPUBindGroupLayoutHandle& handle) {}

    void RenderDeviceImpl::DestroyRenderPipeline(Graphics::GFXRenderPipelineHandle& handle) {}

    void RenderDeviceImpl::DestroyPipelineLayout(Graphics::GFXPipelineLayoutHandle& handle) {}

    void RenderDeviceImpl::DestroyShaderModule(Graphics::GFXShaderModuleHandle& handle) {}

    void RenderDeviceImpl::WaitForIdle() {}

    std::unique_ptr<Graphics::RenderEncoder> RenderDeviceImpl::Encode(const Graphics::RenderEncoderDesc& encoderDesc) {}

    void RenderDeviceImpl::EndEncoding(std::unique_ptr<Graphics::RenderEncoder> encoder) {}

    void RenderDeviceImpl::EncodeImmediateCommands(Graphics::EncodeImmediateFun encodeFun,
                                                   const Graphics::RenderEncoderDesc& encoderDesc)
    {
    }
} // namespace Cocoa::Vulkan
