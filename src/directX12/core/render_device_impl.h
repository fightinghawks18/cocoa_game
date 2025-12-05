#pragma once

#include <unordered_map>
#include "../../graphics/core/render_device.h"
#include "../../graphics/utils/descriptors.h"
#include "../utils/common.h"

namespace Cocoa::D3D12 {
    class RenderDeviceImpl final : public Graphics::RenderDeviceImpl {
        public:
            RenderDeviceImpl( const Graphics::RenderDeviceDesc& desc );
            ~RenderDeviceImpl() override;

            void RegisterResources( Graphics::RenderDevice& device ) override;

            Graphics::RenderWindowHandle ConnectWindow( const Graphics::RenderWindowDesc& desc ) override;

            Graphics::GPUBufferHandle CreateBuffer( const Graphics::GPUBufferDesc& desc ) override;

            Graphics::GPUTextureHandle CreateTexture( const Graphics::GPUTextureDesc& desc ) override;

            Graphics::GPUTextureViewHandle CreateTextureView( const Graphics::GPUTextureViewDesc& desc ) override;

            Graphics::GPUSamplerHandle CreateSampler( const Graphics::GPUSamplerDesc& desc ) override;

            Graphics::GPUBindGroupHandle CreateBindGroup( const Graphics::GPUBindGroupDesc& desc ) override;

            Graphics::GPUBindGroupLayoutHandle
                CreateBindGroupLayout( const Graphics::GPUBindGroupLayoutDesc& desc ) override;

            Graphics::GFXRenderPipelineHandle CreateRenderPipeline( const Graphics::GFXPipelineDesc& desc ) override;

            Graphics::GFXPipelineLayoutHandle
                CreatePipelineLayout( const Graphics::GFXPipelineLayoutDesc& desc ) override;

            Graphics::GFXShaderModuleHandle CreateShaderModule( const Graphics::GFXShaderModuleDesc& desc ) override;

            void DisconnectWindow( Graphics::RenderWindowHandle& handle ) override;

            void DestroyBuffer( Graphics::GPUBufferHandle& handle ) override;

            void DestroyTexture( Graphics::GPUTextureHandle& handle ) override;

            void DestroyTextureView( Graphics::GPUTextureViewHandle& handle ) override;

            void DestroySampler( Graphics::GPUSamplerHandle& handle ) override;

            void DestroyBindGroup( Graphics::GPUBindGroupHandle& handle ) override;

            void DestroyBindGroupLayout( Graphics::GPUBindGroupLayoutHandle& handle ) override;

            void DestroyRenderPipeline( Graphics::GFXRenderPipelineHandle& handle ) override;

            void DestroyPipelineLayout( Graphics::GFXPipelineLayoutHandle& handle ) override;

            void DestroyShaderModule( Graphics::GFXShaderModuleHandle& handle ) override;

            Graphics::RenderEncoder Encode( Graphics::RenderEncoderDesc encoderDesc ) override;

            void EndEncoding( Graphics::RenderEncoder& encoder ) override;

            void EncodeImmediateCommands( Graphics::EncodeImmediateFun encodeFun,
                                          Graphics::RenderEncoderDesc encoderDesc ) override;

            [[nodiscard]] ID3D12Device* GetD3D12Device() const { return _device.Get(); }
            [[nodiscard]] IDXGIFactory* GetDXGIFactory() const { return _dxgiFactory.Get(); }
            [[nodiscard]] ID3D12CommandQueue* GetCommandQueue( const Graphics::GPUQueueType queueType ) const {
                if (!_queues.contains( queueType )) return nullptr;
                return _queues.at( queueType ).Get();
            }
        private:
            ComPtr<ID3D12Device> _device;
            ComPtr<IDXGIAdapter1> _adapter;
            ComPtr<IDXGIFactory4> _dxgiFactory;
            std::unordered_map<Graphics::GPUQueueType, ComPtr<ID3D12CommandQueue>> _queues;
            std::vector<ComPtr<ID3D12CommandAllocator>> _commandAllocators;

            void CreateDXGIFactory();
            void FindDXGIAdapter();
            void CreateD3D12Device();
            void CreateCommandQueues( const Graphics::RenderDeviceDesc& desc );
            void CreateCommandAllocators();
            void DestroyCommandAllocators();
            void DestroyCommandQueues();
            void DestroyD3D12Device();
            void DestroyDXGIAdapter();
            void DestroyDXGIFactory();
    };
}
