#include "surface.h"

#include <SDL3/SDL_vulkan.h>

#include "device.h"

namespace Cocoa::Vulkan {
    Surface::Surface(Device* device, SDL_Window* window) : _device(device) {
        VkSurfaceKHR surface;
        SDL_Vulkan_CreateSurface(window, _device->GetInstance(), nullptr,  &surface);
        _surface = surface;
    }

    Surface::~Surface() {
        _device->GetDevice().waitIdle();
        SDL_Vulkan_DestroySurface(_device->GetInstance(), _surface, nullptr);
    }
}