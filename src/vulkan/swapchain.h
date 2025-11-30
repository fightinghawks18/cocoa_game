#pragma once

#include <vulkan/vulkan.hpp>
#include "surface.h"
#include "handles.h"

namespace Cocoa::Vulkan {
    struct SwapchainDesc {
        SurfaceHandle surface;
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

        Swapchain(const Swapchain& other) = delete;
        Swapchain(Swapchain&& other) noexcept = default;
        Swapchain& operator=(const Swapchain& other) = delete;
        Swapchain& operator=(Swapchain&& other) noexcept = default;

        void Submit(vk::CommandBuffer commandBuffer);
        void Present();

        SwapchainBackBuffer GetNextBackBuffer();
        [[nodiscard]] SwapchainBackBuffer GetCurrentBackBuffer();
        [[nodiscard]] vk::Extent2D GetExtent() { return _swapchainExtent; }
        [[nodiscard]] vk::Format GetFormat() { return _swapchainFormat; }
        [[nodiscard]] vk::SwapchainKHR Get() { return _swapchain.get(); }
    private:
        Device* _device;
        Surface* _surface;

        vk::UniqueSwapchainKHR _swapchain;
        vk::Format _swapchainFormat;
        vk::Extent2D _swapchainExtent;
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
        void Resize();
    };
}