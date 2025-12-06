//
// Created by fightinghawks18 on 12/4/2025.
//

#pragma once

#include <vector>
#include "render_device_impl.h"
#include "../utils/common.h"

namespace Cocoa::D3D12 {
    class SwapChain {
        public:

            explicit SwapChain( RenderDeviceImpl& device,  const Graphics::RenderWindowDesc& desc );
            ~SwapChain();

            void Prepare();
            void Submit( ID3D12CommandList* list, Graphics::GPUQueueType queue );
            void Present();

            [[nodiscard]] u32 GetBackBufferIndex() const { return _backBufferIndex; }
            [[nodiscard]] u64 GetFenceValue(const usize pos) const { return _fenceValues[pos]; }
            [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetRTV(const usize pos) const
            {
                D3D12_CPU_DESCRIPTOR_HANDLE handle;
                _rtvHeap->GetCPUDescriptorHandleForHeapStart( &handle );

                handle.ptr += pos * _rtvDescriptorSize;
                return handle;
            }

            [[nodiscard]] ID3D12Resource* GetRTVResource(const usize pos) const
            {
                return _rtvs[pos].Get();
            }
        private:
            RenderDeviceImpl& _renderDevice;

            ComPtr<IDXGISwapChain3> _swapChain;
            ComPtr<ID3D12DescriptorHeap> _rtvHeap;
            std::vector<ComPtr<ID3D12Resource>> _rtvs;
            u32 _rtvDescriptorSize;

            ComPtr<ID3D12Fence> _fence;
            HANDLE _fenceEvent;
            std::vector<u64> _fenceValues;
            u32 _backBufferIndex;
            u32 _frameIndex = 0;

            void CreateSwapChain( const Graphics::RenderWindowDesc& desc );
            void CreateRenderTargetViewHeap();
            void CreateRenderTargetViews();
            void CreateFences();
            void WaitForFrame();
            void SignalFrame();
            void DestroyFences();
            void DestroyRenderTargetHeap();
            void DestroyRenderTargetViews();
            void DestroySwapChain();
    };
}
