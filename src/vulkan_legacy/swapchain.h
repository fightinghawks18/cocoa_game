#pragma once

#include <vulkan/vulkan.hpp>
#include "surface.h"

#include "../graphics/descriptors.h"
#include "../graphics/handles.h"
#include "../graphics/types.h"

namespace Cocoa::Vulkan {

    class Device;
    class Swapchain {
    public:
        Swapchain(Device* device, Graphics::SwapchainDesc desc);
        ~Swapchain();

        Swapchain(const Swapchain& other) = delete;
        Swapchain(Swapchain&& other) noexcept = default;
        Swapchain& operator=(const Swapchain& other) = delete;
        Swapchain& operator=(Swapchain&& other) noexcept = default;

        void Submit(vk::CommandBuffer commandBuffer);
        void Present();

        Graphics::TextureHandle GetNextBackBuffer();
        [[nodiscard]] Graphics::TextureHandle GetCurrentBackBuffer();
        [[nodiscard]] Graphics::Scale GetExtent() { return _swapchainExtent; }
        [[nodiscard]] vk::Format GetFormat() { return _swapchainFormat; }
        [[nodiscard]] vk::SwapchainKHR Get() { return _swapchain.get(); }
    private:
        Device* _device;
        Surface* _surface;

        vk::UniqueSwapchainKHR _swapchain;
        vk::Format _swapchainFormat;
        Graphics::Scale _swapchainExtent;
        std::vector<Graphics::TextureHandle> _swapchainImages;
        uint32_t _imageIndex;

        std::vector<vk::UniqueFence> _fences;
        std::vector<vk::UniqueSemaphore> _renderSemaphores;
        std::vector<vk::UniqueSemaphore> _imageSemaphores;
        uint32_t _frame = 0;

        void CreateSwapchain();
        void CreateFences();
        void CreateSemaphores();
        void Resize();

        void DestroySwapchainImages();
    };
}