//
// Created by fightinghawks18 on 12/5/2025.
//

#include "encoder_impl.h"

namespace Cocoa::D3D12
{
    EncoderImpl::EncoderImpl( RenderDeviceImpl& renderDevice, const Graphics::RenderEncoderDesc& desc, ID3D12GraphicsCommandList* list )
        : _list( list ), _queueType( desc.submitQueue )
    {
        const auto windowImpl = renderDevice.GetWindowImpl( *desc.swapChain );
        const auto swapChain  = static_cast<SwapChain*>( windowImpl );
        _swapChain = swapChain;
    }

    void EncoderImpl::StartRenderPass( const Graphics::GPUPassDesc& renderPassDescriptor )
    {
        const auto rtv = _swapChain->GetRTV( _swapChain->GetBackBufferIndex() );
        const auto rtvResource = _swapChain->GetRTVResource( _swapChain->GetBackBufferIndex() );

        const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            rtvResource,
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_RENDER_TARGET );
        _list->ResourceBarrier( 1, &barrier );


        D3D12_RESOURCE_DESC rtvDesc;
        rtvResource->GetDesc( &rtvDesc );

        const float actualWidth = static_cast<float>( rtvDesc.Width );
        const float actualHeight = static_cast<float>( rtvDesc.Height );

        const D3D12_VIEWPORT viewport = {
            .TopLeftX = static_cast<float>( renderPassDescriptor.renderArea.offset.x ) * actualWidth,
            .TopLeftY = static_cast<float>( renderPassDescriptor.renderArea.offset.y ) * actualHeight,
            .Width = static_cast<float>( renderPassDescriptor.renderArea.scale.w ) * actualWidth,
            .Height = static_cast<float>( renderPassDescriptor.renderArea.scale.h ) * actualHeight,
            .MinDepth = 0.0f,
            .MaxDepth = 1.0f
        };

        const D3D12_RECT scissor = {
            .left = static_cast<LONG>( viewport.TopLeftX ),
            .top = static_cast<LONG>( viewport.TopLeftY ),
            .right = static_cast<LONG>( viewport.TopLeftX + viewport.Width ),
            .bottom = static_cast<LONG>( viewport.TopLeftY + viewport.Height )
        };

        const FLOAT clearColor[] = { 0.3f, 0.0f, 0.0f, 1.0f };

        _list->RSSetViewports( 1, &viewport );
        _list->RSSetScissorRects( 1, &scissor );
        _list->ClearRenderTargetView( rtv, clearColor, 1, &scissor );
    }

    void EncoderImpl::SetRenderPipeline( const Graphics::GFXRenderPipelineHandle& renderPipeline )
    {
    }

    void EncoderImpl::SetOutputTransform( const Graphics::OutputTransform& outputTransform )
    {
    }

    void EncoderImpl::SetRenderArea( const Graphics::RenderArea& renderArea )
    {
    }

    void EncoderImpl::SetVertexBuffer( const Graphics::GPUBufferHandle& vertexBuffer )
    {
    }

    void EncoderImpl::SetIndexBuffer( const Graphics::GPUBufferHandle& indexBuffer )
    {
    }

    void EncoderImpl::SetBindGroup( const Graphics::GPUBindGroupHandle& bindGroup )
    {
    }

    void EncoderImpl::EndRenderPass()
    {
        const auto rtvResource = _swapChain->GetRTVResource( _swapChain->GetBackBufferIndex() );
        const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            rtvResource,
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT );
        _list->ResourceBarrier( 1, &barrier );
    }

    void EncoderImpl::Stop()
    {
        THROW_IF_HR_FAILED( _list->Close(), "Failed to close D3D12CommandList after encoding!" );
    }
}
