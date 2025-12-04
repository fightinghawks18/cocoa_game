#pragma once

#include <memory>
#include "../utils/descriptors.h"

namespace Cocoa::Graphics {
    class GFXDevice;
    struct GFXWindowImpl {
        GFXWindowImpl(GFXDevice& device, GFXWindowDesc desc) : _device(device) {}
        virtual ~GFXWindowImpl() = default;
        virtual void AcquireNextBuffer() = 0;
        virtual void Present() = 0;
        virtual void Submit() = 0;
    protected:
        GFXDevice& _device;
    };

    class GFXWindow {
    public:
        GFXWindow(std::unique_ptr<GFXWindowImpl> impl) : _impl(std::move(impl)) {}
        ~GFXWindow();

        void AcquireNewBuffer() {
            _impl->AcquireNextBuffer();
        }

        void Present() {
            _impl->Present();
        }

        void Submit() {
            _impl->Submit();
        }
        
        [[nodiscard]] GFXWindowImpl* GetImpl() { return _impl.get(); }
    private:
        std::unique_ptr<GFXWindowImpl> _impl;
    };
}