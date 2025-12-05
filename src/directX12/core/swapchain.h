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
            void Present();

            [[nodiscard]] u64 GetFenceValue(const usize pos) const { return _fenceValues[pos]; }
        private:
            RenderDeviceImpl& _renderDevice;

            ComPtr<IDXGISwapChain3> _swapChain;
            ComPtr<ID3D12Fence> _fence;
            HANDLE _fenceEvent;
            std::vector<u64> _fenceValues;
            u32 _backBufferIndex;
            u32 _frameIndex;

            void CreateSwapChain( const Graphics::RenderWindowDesc& desc );
            void CreateFences();
            void WaitForPreviousFrame();
            void DestroyFences();
            void DestroySwapChain();
    };
}
