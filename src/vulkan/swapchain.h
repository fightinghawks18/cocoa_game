#pragma once

#include <vulkan/vulkan.hpp>
#include "surface.h"

namespace Cocoa::Vulkan {
    struct SwapchainDesc {
        Surface* surface;
    };

    struct SwapchainBackBuffer {
        vk::Image image;
        vk::ImageView imageView;
    };

    class Device;
    class Swapchain {
    public:
        Swapchain(Device* device, SwapchainDesc desc);
        ~Swapchain();

        void Submit(vk::CommandBuffer commandBuffer);
        void Present();

        [[nodiscard]] SwapchainBackBuffer GetNextBackBuffer();
        [[nodiscard]] SwapchainBackBuffer GetCurrentBackBuffer();
        [[nodiscard]] vk::SwapchainKHR Get() { return _swapchain.get(); }
    private:
        Device* _device;
        Surface* _surface;

        vk::UniqueSwapchainKHR _swapchain;
        std::vector<vk::Image> _swapchainImages;
        std::vector<vk::UniqueImageView> _swapchainImageViews;
        uint32_t _imageIndex;

        std::vector<vk::UniqueFence> _fences;
        std::vector<vk::UniqueSemaphore> _renderSemaphores;
        std::vector<vk::UniqueSemaphore> _imageSemaphores;
        uint32_t _frame = 0;

        void CreateSwapchain();
        void CreateFences();
        void CreateSemaphores();
    };
}