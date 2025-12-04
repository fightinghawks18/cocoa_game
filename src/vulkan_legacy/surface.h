#pragma once

#include <vulkan/vulkan.hpp>
#include "SDL3/SDL_video.h"

namespace Cocoa::Vulkan {
    class Device;
    class Surface {
    public:
        Surface(Device* device, SDL_Window* window);
        ~Surface();

        Surface(const Surface& other) = delete;
        Surface(Surface&& other) noexcept = default;
        Surface& operator=(const Surface& other) = delete;
        Surface& operator=(Surface&& other) noexcept = default;

        [[nodiscard]] vk::SurfaceKHR Get() { return _surface; }
    private:
        Device* _device;
        vk::SurfaceKHR _surface;
    };
}