//
// Created by fightinghawks18 on 12/4/2025.
//

#include "render_device_impl.h"
#include "swapchain.h"
#include "../utils/d3d12_types.h"


namespace Cocoa::D3D12 {
    RenderDeviceImpl::RenderDeviceImpl( const Graphics::RenderDeviceDesc& desc )
    {
        CreateDXGIFactory();
        FindDXGIAdapter();
        CreateD3D12Device();
        CreateCommandQueues( desc );
        CreateCommandAllocators();
    }

    RenderDeviceImpl::~RenderDeviceImpl()
    {
        DestroyCommandAllocators();
        DestroyCommandQueues();
        DestroyD3D12Device();
        DestroyDXGIFactory();
    }

    void RenderDeviceImpl::RegisterResources(Graphics::RenderDevice& device) {
        AddManager<SwapChain>();
    }

    Graphics::RenderWindowHandle RenderDeviceImpl::ConnectWindow(const Graphics::RenderWindowDesc& desc) {
        return GetManager<SwapChain>()->Create( *this, desc );
    }

    Graphics::GPUBufferHandle RenderDeviceImpl::CreateBuffer(const Graphics::GPUBufferDesc& desc) {
    }

    Graphics::GPUTextureHandle RenderDeviceImpl::CreateTexture(const Graphics::GPUTextureDesc& desc) {
    }

    Graphics::GPUTextureViewHandle RenderDeviceImpl::CreateTextureView(const Graphics::GPUTextureViewDesc& desc) {
    }

    Graphics::GPUSamplerHandle RenderDeviceImpl::CreateSampler(const Graphics::GPUSamplerDesc& desc) {
    }

    Graphics::GPUBindGroupHandle RenderDeviceImpl::CreateBindGroup(const Graphics::GPUBindGroupDesc& desc) {
    }

    Graphics::GPUBindGroupLayoutHandle RenderDeviceImpl::CreateBindGroupLayout(const Graphics::GPUBindGroupLayoutDesc& desc) {
    }

    Graphics::GFXRenderPipelineHandle RenderDeviceImpl::CreateRenderPipeline(const Graphics::GFXPipelineDesc& desc) {
    }

    Graphics::GFXPipelineLayoutHandle RenderDeviceImpl::CreatePipelineLayout(const Graphics::GFXPipelineLayoutDesc& desc) {
    }

    Graphics::GFXShaderModuleHandle RenderDeviceImpl::CreateShaderModule(const Graphics::GFXShaderModuleDesc& desc) {
    }

    void RenderDeviceImpl::DisconnectWindow(Graphics::RenderWindowHandle& handle) {
        GetManager<SwapChain>()->Destroy( handle );
    }

    void RenderDeviceImpl::DestroyBuffer(Graphics::GPUBufferHandle& handle) {
    }

    void RenderDeviceImpl::DestroyTexture(Graphics::GPUTextureHandle& handle) {
    }

    void RenderDeviceImpl::DestroyTextureView(Graphics::GPUTextureViewHandle& handle) {
    }

    void RenderDeviceImpl::DestroySampler(Graphics::GPUSamplerHandle& handle) {
    }

    void RenderDeviceImpl::DestroyBindGroup(Graphics::GPUBindGroupHandle& handle) {
    }

    void RenderDeviceImpl::DestroyBindGroupLayout(Graphics::GPUBindGroupLayoutHandle& handle) {
    }

    void RenderDeviceImpl::DestroyRenderPipeline(Graphics::GFXRenderPipelineHandle& handle) {
    }

    void RenderDeviceImpl::DestroyPipelineLayout(Graphics::GFXPipelineLayoutHandle& handle) {
    }

    void RenderDeviceImpl::DestroyShaderModule(Graphics::GFXShaderModuleHandle& handle) {
    }

    Graphics::RenderEncoder RenderDeviceImpl::Encode(Graphics::RenderEncoderDesc encoderDesc) {
    }

    void RenderDeviceImpl::EndEncoding(Graphics::RenderEncoder& encoder) {
    }

    void RenderDeviceImpl::EncodeImmediateCommands(Graphics::EncodeImmediateFun encodeFun,
                                             Graphics::RenderEncoderDesc encoderDesc) {
    }

    void RenderDeviceImpl::CreateDXGIFactory()
    {
        THROW_IF_HR_FAILED( CreateDXGIFactory2( 0, IID_PPV_ARGS( &_dxgiFactory ) ), "Failed to create DXGIFactory!" );
    }

    void RenderDeviceImpl::FindDXGIAdapter()
    {
        ComPtr<IDXGIAdapter1> adapter;
        for ( u32 i = 0; _dxgiFactory->EnumAdapters1( i, &adapter ) != DXGI_ERROR_NOT_FOUND; i++ )
        {
            DXGI_ADAPTER_DESC1 desc;
            THROW_IF_HR_FAILED( adapter->GetDesc1( &desc ), "Failed to get DXGIAdapter description!" );

            if ( desc.Flags &~ DXGI_ADAPTER_FLAG_SOFTWARE )
            {
                // Choose any non-software adapter for now
                break;
            }
        }

        THROW_IF_FAILED( !adapter, "Failed to enumerate any DXGIAdapters!" );
        _adapter = std::move(adapter);
    }

    void RenderDeviceImpl::CreateD3D12Device()
    {
        THROW_IF_HR_FAILED(
            D3D12CreateDevice( _adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS( &_device ) ),
            "Failed to create D3D12Device!" );
    }

    void RenderDeviceImpl::CreateCommandQueues( const Graphics::RenderDeviceDesc& desc )
    {
        if ( _queues.bucket_count() == 0 )
            _queues.reserve( desc.desiredQueues.size() );

        for ( const auto& queueType : desc.desiredQueues )
        {
            D3D12_COMMAND_QUEUE_DESC queueDesc = {
                .Type = TranslateToD3D12CommandListType( queueType ),
                .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
                .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
                .NodeMask = 0,
            };

            THROW_IF_HR_FAILED(
                _device->CreateCommandQueue( &queueDesc, IID_PPV_ARGS( &_queues[ queueType ] ) ),
                "Failed to create D3D12CommandQueue for type %d", static_cast<int>(queueType) );
        }
    }

    void RenderDeviceImpl::CreateCommandAllocators()
    {
        for ( u32 i = 0; i < 2; i++ )
        {
            THROW_IF_HR_FAILED(
                _device->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( &_commandAllocators[i] ) ),
                "Failed to create D3D12CommandAllocator!" );
        }
    }

    void RenderDeviceImpl::DestroyCommandAllocators()
    {
        _commandAllocators.clear();
    }

    void RenderDeviceImpl::DestroyCommandQueues()
    {
        _queues.clear();
    }

    void RenderDeviceImpl::DestroyD3D12Device()
    {
        _device.Reset();
    }

    void RenderDeviceImpl::DestroyDXGIAdapter()
    {
        _adapter.Reset();
    }

    void RenderDeviceImpl::DestroyDXGIFactory()
    {
        _dxgiFactory.Reset();
    }
}
