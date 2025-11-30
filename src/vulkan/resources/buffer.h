#pragma once

#include <vulkan/vulkan.hpp>
#include "../vma.h"

namespace Cocoa::Vulkan {
    struct BufferDesc {
        vk::BufferUsageFlags usage;
        uint64_t size;
        void* mapped;
    };

    class Device;
    class Buffer {
    public:
        Buffer(Device* device, BufferDesc desc);
        ~Buffer();

        Buffer(const Buffer& other) = delete;
        Buffer(Buffer&& other) noexcept = default;
        Buffer& operator=(const Buffer& other) = delete;
        Buffer& operator=(Buffer&& other) noexcept = default;

        void MapTo(void* mapped, uint64_t size, uint64_t offset);

        [[nodiscard]] VkBuffer Get() { return _buffer; }
    private:
        Device* _device;

        vk::Buffer _buffer;
        VmaAllocation _allocation;
    };
}