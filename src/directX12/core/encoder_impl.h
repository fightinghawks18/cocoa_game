//
// Created by fightinghawks18 on 12/5/2025.
//

#pragma once

#include "swapchain.h"
#include "../../graphics/core/encoder.h"
#include "../utils/common.h"

namespace Cocoa::D3D12
{
    class RenderDeviceImpl;
    class EncoderImpl final : public Graphics::RenderEncoderImpl
    {
        public:
            explicit EncoderImpl( RenderDeviceImpl& renderDevice, const Graphics::RenderEncoderDesc& desc, ID3D12GraphicsCommandList* list );
            ~EncoderImpl() override = default;

            void StartRenderPass( const Graphics::GPUPassDesc& renderPassDescriptor ) override;

            void SetRenderPipeline( const Graphics::GFXRenderPipelineHandle& renderPipeline ) override;

            void SetOutputTransform( const Graphics::OutputTransform& outputTransform ) override;

            void SetRenderArea( const Graphics::RenderArea& renderArea ) override;

            void SetVertexBuffer( const Graphics::GPUBufferHandle& vertexBuffer ) override;

            void SetIndexBuffer( const Graphics::GPUBufferHandle& indexBuffer ) override;

            void SetBindGroup( const Graphics::GPUBindGroupHandle& bindGroup ) override;

            void EndRenderPass() override;

            void Stop() override;

            [[nodiscard]] ID3D12GraphicsCommandList* GetCommandList() const { return _list; }
            [[nodiscard]] SwapChain* GetSwapChain() const { return _swapChain; }
            [[nodiscard]] Graphics::GPUQueueType GetQueueType() const { return _queueType; }
        private:
            ID3D12GraphicsCommandList* _list;
            SwapChain* _swapChain;
            Graphics::GPUQueueType _queueType;
    };
}
