//
// Created by fightinghawks18 on 12/4/2025.
//

#include "swapchain.h"
#include "../../macros.h"

namespace Cocoa::D3D12
{
    SwapChain::SwapChain( RenderDeviceImpl& device, const Graphics::RenderWindowDesc& desc ) : _renderDevice( device )
    {
        CreateSwapChain( desc );
        CreateFences();
    }

    SwapChain::~SwapChain()
    {
        DestroyFences();
        DestroySwapChain();
    }

    void SwapChain::Prepare()
    {
        WaitForPreviousFrame();
    }

    void SwapChain::Present()
    {
        THROW_IF_HR_FAILED( _swapChain->Present( 1, 0 ), "Failed to present swapChain!" );
        _frameIndex = (_frameIndex + 1) % 2;
    }

    void SwapChain::CreateSwapChain( const Graphics::RenderWindowDesc& desc )
    {
        const SDL_PropertiesID props = SDL_GetWindowProperties( desc.window );
        const auto hwnd = static_cast<HWND>( SDL_GetPointerProperty( props , SDL_PROP_WINDOW_WIN32_HWND_POINTER ,
                                                                     nullptr ) );

        DXGI_SWAP_CHAIN_DESC swapChainDesc = {
            .BufferDesc = {
                .Width = 800,
                .Height = 600,
                .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
            },
            .SampleDesc = {
                .Count = 1
            },
            .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
            .OutputWindow = hwnd,
            .Windowed = TRUE,
            .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD
        };

        ComPtr<IDXGISwapChain> tempSwapchain;
        THROW_IF_HR_FAILED(
                           _renderDevice.GetDXGIFactory()->CreateSwapChain( _renderDevice.GetD3D12Device(), &swapChainDesc, &
                               tempSwapchain ) ,
                           "Failed to create DXGISwapChain" );
        THROW_IF_HR_FAILED( tempSwapchain.As( &_swapChain ) , "Failed to cast IDXGISwapchain!" );
    }

    void SwapChain::CreateFences()
    {
        THROW_IF_HR_FAILED(
                           _renderDevice.GetD3D12Device()->CreateFence( 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( &_fence)),
                           "Failed to create D3D12Fence!" );
        _fenceEvent = CreateEvent( nullptr , false , false , nullptr );

        if ( _fenceValues.capacity() == 0 ) _fenceValues.reserve( 2 );
        for ( u32 i = 0; i < 2; i++ )
        {
            _fenceValues.push_back( 0 );
        }
    }

    void SwapChain::WaitForPreviousFrame()
    {
        auto& fenceValue = _fenceValues[ _frameIndex ];
        const auto commandQueue = _renderDevice.GetCommandQueue( Graphics::GPUQueueType::Graphics );
        fenceValue++;
        THROW_IF_HR_FAILED(
                           commandQueue->Signal( _fence.Get(), fenceValue ),
                           "Failed to signal swapChain queue!" );

        if ( _fence->GetCompletedValue() < fenceValue )
        {
            THROW_IF_HR_FAILED(
                               _fence->SetEventOnCompletion( fenceValue, _fenceEvent ),
                               "Failed to set swapChain fence's completion event!" );
            WaitForSingleObject( _fenceEvent , u32Max );
        }
        _backBufferIndex = _swapChain->GetCurrentBackBufferIndex();
    }

    void SwapChain::DestroyFences()
    {
        CloseHandle( _fenceEvent );
        _fenceValues.clear();
        _fence.Reset();
    }

    void SwapChain::DestroySwapChain()
    {
        _swapChain.Reset();
    }
}
