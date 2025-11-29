#pragma once

#include <memory>
#include <optional>
#include <unordered_map>
#include <vulkan/vulkan.hpp>
#include <SDL3/SDL.h>

#include "vma.h"
#include "common.h"

#include "swapchain.h"
#include "encoder.h"

namespace Cocoa::Vulkan {
    struct DeviceDesc {
        SDL_Window* window;
        std::vector<GPUQueueType> desiredQueues;
        GPUPowerPreference powerPreference;
    };

    class Device {
    public:
        Device(DeviceDesc desc);
        ~Device();

        std::unique_ptr<Encoder> Encode(Swapchain* swapchain);
        void EndEncoding(std::unique_ptr<Encoder> encoder);

        [[nodiscard]] std::optional<GPUQueue> GetQueue(GPUQueueType queueType);
        [[nodiscard]] vk::Instance GetInstance() { return _instance.get(); }
        [[nodiscard]] vk::PhysicalDevice GetGPU() { return _gpu; }
        [[nodiscard]] vk::Device GetDevice() { return _device.get(); }
        [[nodiscard]] VmaAllocator GetAllocator() { return _allocator; }
    private:
        vk::UniqueInstance _instance;
        vk::PhysicalDevice _gpu;
        vk::UniqueDevice _device;
        std::unordered_map<GPUQueueType, GPUQueue> _queues;
        VmaAllocator _allocator;

        vk::UniqueCommandPool _commandPool;
        std::vector<vk::UniqueCommandBuffer> _commandBuffers;
        uint32_t _frame = 0;

        void CreateInstance();
        void GetPhysicalDevice(DeviceDesc desc);
        GPUQueue GetSupportedQueue(std::vector<vk::QueueFamilyProperties> queueFamilies, GPUQueueType type);
        void DiscoverQueues(DeviceDesc desc);
        void CreateDevice();
        void CreateAllocator();
        void CreateCommandPool();
        void CreateCommandBuffers();
    };
}