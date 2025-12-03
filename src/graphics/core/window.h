#pragma once

#include <memory>
#include <SDL3/SDL.h>

namespace Cocoa::Graphics {
    class GFXWindow {
    public:
        GFXWindow(SDL_Window* window);
        ~GFXWindow();

        void AcquireNewBuffer();
        void Present();
        void Submit();
        
        struct Impl;
        [[nodiscard]] Impl* GetImpl() { return _impl.get(); }
    private:
        std::unique_ptr<Impl> _impl;
    };
}