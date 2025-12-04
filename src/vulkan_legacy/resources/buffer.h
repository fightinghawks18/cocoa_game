#pragma once

#include <vulkan/vulkan.hpp>
#include "../vma.h"

#include "../../graphics/descriptors.h"

namespace Cocoa::Vulkan {

    class Device;
    class Buffer {
    public:
        Buffer(Device* device, Graphics::GFXBufferDesc desc);
        ~Buffer();

        Buffer(const Buffer& other) = delete;
        Buffer(Buffer&& other) noexcept = default;
        Buffer& operator=(const Buffer& other) = delete;
        Buffer& operator=(Buffer&& other) noexcept = default;

        void MapTo(void* mapped, uint64_t size, uint64_t offset);

        [[nodiscard]] VkBuffer Get() { return _buffer; }
        [[nodiscard]] uint64_t GetSize() { return _size; }
    private:
        Device* _device;

        vk::Buffer _buffer;
        void* _mapped = nullptr;
        uint64_t _size;
        VmaAllocation _allocation;
    };
}