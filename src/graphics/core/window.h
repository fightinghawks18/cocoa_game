#pragma once

#include <memory>
#include "../utils/descriptors.h"

namespace Cocoa::Graphics {
    class GFXDevice;
    struct GFXWindowImpl {
        GFXWindowImpl(GFXDevice& device, GFXWindowDesc desc);
        virtual ~GFXWindowImpl() {}
        virtual void AcquireNextBuffer() = 0;
        virtual void Present() = 0;
        virtual void Submit() = 0;
    protected:
        GFXDevice& _device;
    };

    class GFXWindow {
    public:
        GFXWindow(GFXDevice& device, GFXWindowDesc desc);
        ~GFXWindow();

        void AcquireNewBuffer();
        void Present();
        void Submit();
        
        [[nodiscard]] GFXWindowImpl* GetImpl() { return _impl.get(); }
    private:
        GFXDevice& _device;

        std::unique_ptr<GFXWindowImpl> _impl;
    };
}